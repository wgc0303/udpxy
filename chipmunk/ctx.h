#ifndef UDPXY_CTX_H_0111081738
#define UDPXY_CTX_H_0111081738

#include "platform.h"
#include "udpxy.h"
#include "dpkt.h"

#ifdef __cpluspplus
    extern "C" {
#endif

/* throughput statistics */
struct tput_stat {
#ifdef _WIN32
    DWORD       sender_id;
#else
    pid_t       sender_id;
#endif
    double      nbytes;
    double      nsec;
};

/* context of a relay client */
struct client_ctx
{
#ifdef _WIN32
    HANDLE      hThread;
    DWORD       dwThreadId;
#else
    pid_t       pid;
#endif
    char        mcast_addr[ IPADDR_STR_SIZE ];
    uint16_t    mcast_port;
    char        src_addr[ IPADDR_STR_SIZE ];
    uint16_t    src_port;
    struct tput_stat tstat;
    char        tail[ MAX_TAIL_LEN + 1 ];
};

/* statistics on traffic relay & data gathering */
struct tps_data {
#ifdef _WIN32
    DWORD  pid;
#else
    pid_t  pid;
#endif
    time_t tm_from;
    double niter;
    double nbytes;
};

/* server request components */
struct srv_request {
    char        cmd[ MAX_CMD_LEN + 1 ];
    char        param[ MAX_PARAM_LEN + 1 ];
    char        tail[ MAX_TAIL_LEN + 1 ];
};

/* context of the server */
struct server_ctx
{
    int         lsockfd;
    char        listen_addr[ IPADDR_STR_SIZE ];
    uint16_t    listen_port;
    char        mcast_ifc_addr[ IPADDR_STR_SIZE ];
    struct in_addr mcast_inaddr;
    struct srv_request rq;
    size_t      clfree, clmax;
    struct client_ctx* cl;
    u_short     rcv_tmout, snd_tmout;
    int         cpipe[ 2 ];
#ifdef _WIN32
    CRITICAL_SECTION    cl_lock;
#endif
};

int init_server_ctx( struct server_ctx* ctx, const size_t max, const char* laddr, uint16_t lport, const char* mifc_addr );
void free_server_ctx( struct server_ctx* ctx );

#ifdef _WIN32
int find_client( const struct server_ctx* ctx, DWORD tid );
int add_client( struct server_ctx* ctx, HANDLE hThread, DWORD dwThreadId, const char* maddr, uint16_t mport, int sockfd );
int delete_client( struct server_ctx* ctx, DWORD tid );
#else
int find_client( const struct server_ctx* ctx, pid_t pid );
int add_client( struct server_ctx* ctx, pid_t cpid, const char* maddr, uint16_t mport, int sockfd );
int delete_client( struct server_ctx* ctx, pid_t cpid );
#endif

void tpstat_init( struct tps_data* d, int setpid );
void tpstat_update( struct server_ctx* ctx, struct tps_data* d, ssize_t nbytes );
int tpstat_read( struct server_ctx* ctx );

#ifdef __cpluspplus
}
#endif

#endif /* UDPXY_CTX_H_0111081738 */
