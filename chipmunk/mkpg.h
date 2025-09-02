#ifndef UDPXY_MKPG_H_0115081656
#define UDPXY_MKPG_H_0115081656

#include "platform.h"
#include "ctx.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* generate service's (HTML) status page
 *
 * @param ctx       server context
 * @param buf       destination buffer
 * @param len       destination buffer's length
 * @param options   options as a bitset of flags
 *
 * @return 0 if success, len gets updated with page text's size
 *         -1 in case of failure
 */

#define MSO_HTTP_HEADER      1      /* prepend page with HTTP header */
#define MSO_SKIP_CLIENTS     2      /* do not output client info     */
#define MSO_RESTART          4      /* use restart-page format       */


int
mk_status_page( const struct server_ctx* ctx,
                char* buf, size_t* len, int options );


#ifdef __cplusplus
}
#endif

#endif /* UDPXY_MKPG_H_0115081656 */
