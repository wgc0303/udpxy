#ifndef IFADDR_H_12262007__
#define IFADDR_H_12262007__

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* retrieve IPv4 address of the given network interface
 */
int
if2addr( const char* ifname,
             struct sockaddr *addr, size_t addrlen );

/* convert input parameter into an IPv4-address string
 */
int
get_ipv4_address( const char* s, char* buf, size_t len );


/* split input string into IP address and port
 */
int
get_addrport( const char* s, char* addr, size_t len, int* port );


#ifdef __cplusplus
}
#endif

#endif
