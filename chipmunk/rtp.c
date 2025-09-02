#include "platform.h"
#include "rtp.h"
#include "mtrace.h"
#include "util.h"

extern FILE* g_flog;


/* check if the buffer is an RTP packet
 */
int
RTP_check( const char* buf, const size_t len, int* is_rtp, FILE* log )
{
    assert( buf && is_rtp && log );

    if( len < RTP_MIN_SIZE ) {
        (void)tmfprintf( log, "RTP_check: buffer size [%lu] is "
                    "less than minimum [%lu]\n" , (u_long)len, (u_long)RTP_MIN_SIZE );
        return -1;
    }

    /* version must be 2 */
    if( ((buf[0] >> 6) & 0x03) != RTP_VER2 ) {
        *is_rtp = 0;
        return 0;
    }

    *is_rtp = 1;
    return 0;
}


/* verify if buffer contains an RTP packet, 0 otherwise
 */
int
RTP_verify( const char* buf, const size_t len, FILE* log )
{
    int is_rtp = 0;

    if( 0 != RTP_check( buf, len, &is_rtp, log ) )
        return 0;

    if( !is_rtp ) {
        TRACE( (void)tmfprintf( log, "RTP_verify: version mismatch\n" ) );
        return 0;
    }

    if( len < RTP_HDR_SIZE ) {
        TRACE( (void)tmfprintf( log, "RTP_verify: inappropriate size=[%lu] "
                "of RTP packet\n", (u_long)len ) );
        return 0;
    }

    return 1;
}


/* calculate length of an RTP header
 */
int
RTP_hdrlen( const char* buf, const size_t len, size_t* hdrlen,
                    FILE* log )
{
    size_t xthdrlen = 0;
    int cc = 0;

    assert( buf && hdrlen && log );

    if( !RTP_verify( buf, len, log ) )
        return -1;

    /* number of CSRC identifiers */
    cc = buf[0] & 0x0F;

    /* if header extension bit is set */
    if( (buf[0] >> 4) & 0x01 ) {
        if( len < RTP_XTHDRLEN )
            return ENOMEM;
        xthdrlen =
            sizeof(int32_t) * ((buf[ XTLEN_OFFSET ] << 8) + buf[ XTLEN_OFFSET + 1 ]);
    }

    *hdrlen = RTP_HDR_SIZE + (cc * CSRC_SIZE) + xthdrlen;

    return 0;
}


/* process RTP package to retrieve the payload
 */
int
RTP_process( void** pbuf, size_t* len, int verify, FILE* log )
{
    const char* buf = NULL;
    size_t pkt_len, hdr_len = 0;

    assert( pbuf && len && log );

    buf = *pbuf;
    pkt_len = *len;

    if( verify && !RTP_verify( buf, pkt_len, log ) )
        return -1;

    if( 0 != RTP_hdrlen( buf, pkt_len, &hdr_len, log ) )
        return -1;

    if( pkt_len < hdr_len ) {
        (void) tmfprintf( log, "RTP_process: invalid header "
                "length [%lu] vs packet length [%lu]\n",
                (u_long)hdr_len, (u_long)pkt_len );
        return -1;
    }

    *pbuf = (char*)(*pbuf) + hdr_len;
    *len = pkt_len - hdr_len;

    return 0;
}
