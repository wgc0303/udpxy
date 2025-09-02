#ifndef NETOP_H_0215082300
#define NETOP_H_0215082300

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

int
setup_listener( const char* ipaddr, int port, int* sockfd, int bklog );

int
setup_mcast_listener( struct sockaddr_in*   s_address,
                      struct sockaddr_in*   m_address,
                      const struct in_addr* mifaddr,
                      int*                  mcastfd,
                      int                   sockbuflen );

void
close_mcast_listener( int msockfd, const struct in_addr* mifaddr, const struct in_addr* saddr );

int
renew_multicast( int msockfd, const struct in_addr* mifaddr, const struct in_addr* s_in_addr );

int
set_timeouts( int rsock, int ssock,
              u_short rcv_tmout_sec, u_short rcv_tmout_usec,
              u_short snd_tmout_sec, u_short snd_tmout_usec );

int
set_sendbuf( int sockfd, const size_t len );

int
set_rcvbuf( int sockfd, const size_t len );

int
get_sendbuf( int sockfd, size_t* const len );

int
get_rcvbuf( int sockfd, size_t* const len );

int
set_nblock( int fd, int set );

int
get_sockinfo (int sockfd, char* addr, size_t alen, int* port);

int
get_peerinfo (int sockfd, char* addr, size_t alen, int* port);

#ifdef __cplusplus
}
#endif

#endif /* NETOP_H_0215082300 */
