#ifndef DPKT_H_021308
#define DPKT_H_021308

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int upxfmt_t;

const char*
fmt2str( upxfmt_t fmt );

upxfmt_t
get_mstream_type( const char* data, size_t len, FILE* log );

upxfmt_t
get_fstream_type( int fd, FILE* log );

ssize_t
read_frecord( int fd, char* data, const size_t len,
             upxfmt_t* stream_type, FILE* log );

ssize_t
write_frecord( int fd, const char* data, size_t len,
              upxfmt_t sfmt, upxfmt_t dfmt, FILE* log );


struct dstream_ctx {
    upxfmt_t stype;
    int      flags;
    struct iovec*
             pkt;
    size_t   pkt_count,
             max_pkt,
             mtu;
};

#define F_CHECK_FMT     1
#define F_SCATTERED     2
#define F_FILE_INPUT    4
#define F_DROP_PACKET   8


void
reset_pkt_registry( struct dstream_ctx* ds );

void
free_dstream_ctx( struct dstream_ctx* ds );

int
init_dstream_ctx( struct dstream_ctx* ds, const char* cmd, const char* fname,
                  ssize_t nmsgs );

ssize_t
read_data( struct dstream_ctx* spc, int fd, char* data,
           const ssize_t data_len, const struct rdata_opt* opt );

ssize_t
write_data( const struct dstream_ctx* spc,
            const char* data,
            const ssize_t len,
            int fd );

struct rdata_opt {
    ssize_t max_frgs;
    time_t  buf_tmout;
};


#ifdef __cplusplus
}
#endif

#endif /* DPKT_H_021308 */
