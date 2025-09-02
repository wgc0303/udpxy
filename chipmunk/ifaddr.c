#include "platform.h"
#include "ifaddr.h"
#include "util.h"
#include "mtrace.h"

#ifdef _WIN32

// Implementation for Windows using GetAdaptersAddresses
int if2addr(const char* ifname, struct sockaddr *addr, size_t addrlen) {
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    ULONG ulOutBufLen = 0;
    DWORD dwRetVal = 0;
    int ret = -1;
    char friendlyNameAnsi[MAX_ADAPTER_NAME_LENGTH * 2];

    if (addrlen < sizeof(struct sockaddr_in)) {
        return -1;
    }

    ulOutBufLen = 15000;
    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(ulOutBufLen);
        if (pAddresses == NULL) {
            return -1;
        }
        dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &ulOutBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }
    } while (dwRetVal == ERROR_BUFFER_OVERFLOW);

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            wcstombs(friendlyNameAnsi, pCurrAddresses->FriendlyName, sizeof(friendlyNameAnsi));
            if (stricmp(friendlyNameAnsi, ifname) == 0) {
                pUnicast = pCurrAddresses->FirstUnicastAddress;
                while (pUnicast) {
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        memcpy(addr, pUnicast->Address.lpSockaddr, sizeof(struct sockaddr_in));
                        ret = 0;
                        goto cleanup;
                    }
                    pUnicast = pUnicast->Next;
                }
            }
            pCurrAddresses = pCurrAddresses->Next;
        }
    }

cleanup:
    if (pAddresses) {
        free(pAddresses);
    }
    return ret;
}

#else // POSIX implementation below

static int
chkifr( const struct ifreq* ifr, const char* ifname,
        const size_t addrlen, size_t* offset )
{
    size_t sa_len = 0;
    assert(ifr && ifname && offset);
#ifdef NO_SOCKADDR_SA_LEN
    switch( ifr->ifr_addr.sa_family )
    {
    #ifndef NO_INET6_SUPPORT
        case AF_INET6: sa_len = sizeof(struct sockaddr_in6); break;
   #endif
        case AF_INET: sa_len = sizeof(struct sockaddr); break;
        default: sa_len = 0; break;
    }
#else
    sa_len = ifr->ifr_addr.sa_len;
#endif
    if( sa_len > 0 ) {
        if ( (ifr->ifr_addr.sa_family == AF_INET) &&
            (0 == strncmp(ifname, ifr->ifr_name, sizeof(struct ifreq))) &&
            (addrlen >= sa_len) ) {
            *offset = sa_len;
            return 0;
        }
    }
#if defined(__linux)
    *offset = sizeof(*ifr);
#else
    *offset = (sa_len + sizeof( ifr->ifr_name ));
#endif
    return -1;
}

int
if2addr( const char* ifname,
             struct sockaddr *addr, size_t addrlen )
{
    int rc, sockfd;
    char *buf, *rec;
    size_t buflen, offset;
    int last_len;
    struct ifconf  ifc;
    struct ifreq   ifr;
    static size_t IFC_TABLE_SIZE;
    static const size_t IFC_ENTRIES = 32;
    static const size_t MAX_IFCBUF_SIZE = (1024 * 256);
    IFC_TABLE_SIZE = sizeof(struct ifreq) * IFC_ENTRIES;
    assert( ifname && addr && addrlen );
    rc = 0;
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if( -1 == sockfd ) return -1;
    buf = NULL; buflen = IFC_TABLE_SIZE; last_len = 0;
    for( ; buflen < MAX_IFCBUF_SIZE; buflen += IFC_TABLE_SIZE ) {
        if( NULL == (buf = malloc( buflen )) ) {
            rc = -1;
            break;
        }
        ifc.ifc_len = buflen;
        ifc.ifc_buf = buf;
        if( ioctl( sockfd, SIOCGIFCONF, &ifc ) < 0 ) {
            if( (EINTR != errno) || (last_len != 0) ) {
                rc = errno;
                break;
            }
        }
        else {
            if( ifc.ifc_len == last_len )
                break;
            else
                last_len = ifc.ifc_len;
        }
        free( buf );
        buf = NULL;
    } /* for */
    (void) close( sockfd );
    if( buflen > MAX_IFCBUF_SIZE ) rc = -1;
    if( 0 != rc ) {
        if( NULL != buf ) free( buf );
        return rc;
    }
    assert( ifc.ifc_buf );
    for( rec = ifc.ifc_buf; rec < (ifc.ifc_buf + ifc.ifc_len); ) {
        (void) memcpy( &ifr, rec, sizeof(struct ifreq) );
        offset = 0;
        rc = chkifr( &ifr, ifname, addrlen, &offset );
        if ( 0 == rc ) {
            (void) memcpy( addr, &(ifr.ifr_addr), offset );
            break;
        }
        if( 0 == offset ) break;
        rec += offset;
    }
    if( rec >= (buf + ifc.ifc_len) ) {
        rc = -1;
    }
    free( buf );
    return rc;
}

#endif

int
get_ipv4_address( const char* s, char* buf, size_t len )
{
    struct sockaddr_in saddr;
    int rc = 0;
    assert( s && buf && len );
    saddr.sin_addr.s_addr = inet_addr(s);
    if( INADDR_NONE != saddr.sin_addr.s_addr ) {
        (void) strncpy( buf, s, len );
    }
    else {
        rc = if2addr( s, (struct sockaddr*)&saddr, sizeof(saddr) );
        if( 0 != rc ) return rc;
        (void) strncpy( buf, inet_ntoa(saddr.sin_addr), len );
    }
    buf[ len - 1 ] = 0;
    return rc;
}

int
get_addrport( const char* s, char* addr, size_t len, int* port )
{
    struct sockaddr_in saddr;
    size_t i = 0;
    int iport = 0;
    static const int ERR_NOPORT     = -1;
    static const int ERR_BADADDR    = -2;
    static const int ERR_BADPORT    = -3;
    static const int ERR_OVERFLOW   = -4;
    assert( s && addr && len && port );
    for( i = 0; (i < len) && s[i] && (':' != s[i]); ++i )
        addr[ i ] = s[ i ];
    if( i >= len )
        return ERR_OVERFLOW;
    else
        addr[i] = '\0';
    if( ':' != s[ i ] ) return ERR_NOPORT;
    saddr.sin_addr.s_addr = inet_addr(addr);
    if( INADDR_NONE == saddr.sin_addr.s_addr )
        return ERR_BADADDR;
    ++i;
    if( i >= len || !s[i] ) return ERR_NOPORT;
    errno = 0;
    iport = atoi( s + i );
    if( errno || (iport <= 0) || (iport > (int)USHRT_MAX) )
        return ERR_BADPORT;
    *port = iport;
    return 0;
}
