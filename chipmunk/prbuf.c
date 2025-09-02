#include "platform.h"
#include "prbuf.h"

struct printbuf
{
    char*   buf;
    size_t  len,
            pos;
};


int
prbuf_open( prbuf_t* pb, void* buf, size_t n )
{
    prbuf_t p = NULL;

    assert( pb && buf && (n > (size_t)0) );

    p = malloc( sizeof(struct printbuf) );
    if( NULL == p ) return -1;

    *pb = p;

    p->buf = (char*)buf;
    p->len = n;
    p->pos = 0;

    return 0;
}


void
prbuf_rewind( prbuf_t pb )
{
    assert( pb );

    pb->pos = 0;
    pb->buf[0] = '\0';
}



int
prbuf_close( prbuf_t pb )
{
    assert( pb );

    free( pb );
    return 0;
}

size_t
prbuf_len( prbuf_t pb )
{
    assert( pb );
    return pb->pos;
}

int
prbuf_printf( prbuf_t pb, const char* format, ... )
{
    va_list ap;
    char* p = NULL;
    int n = -1;
    size_t left;

    assert( pb && format );

    if( pb->pos >= pb->len ) return 0;

    left = pb->len - pb->pos - 1;
    p = pb->buf + pb->pos;

    va_start( ap, format );
    errno = 0;
    n = vsnprintf( p, left, format, ap );
    va_end( ap );

    if( n < 0 ) return -1;

    if( (size_t)n >= left ) {
        n = left;
    }

    p[ n ] = '\0';
    pb->pos += (size_t)n;
    return n;
}


/* __EOF__ */
