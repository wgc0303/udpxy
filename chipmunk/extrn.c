#include "platform.h"

FILE* g_flog = NULL;
volatile sig_atomic_t g_quit = 0;

const char g_udpxy_app[] = "udpxy";

const char   CMD_UDP[]      = "udp";
const char   CMD_RTP[]      = "rtp";
const char   CMD_STATUS[]   = "status";
const char   CMD_RESTART[]  = "restart";

const size_t CMD_UDP_LEN    = sizeof(CMD_UDP) - 1;
const size_t CMD_RTP_LEN    = sizeof(CMD_RTP) - 1;
const size_t CMD_STATUS_LEN = sizeof(CMD_STATUS) - 1;
const size_t CMD_RESTART_LEN= sizeof(CMD_RESTART) - 1;

const char   IPv4_ALL[]     = "0.0.0.0";

const char  UDPXY_COPYRIGHT_NOTICE[] =
    "Copyright (C) 2008-2018 Pavel V. Cherenkov";
const char  UDPXY_CONTACT[] =
    "For more information, visit http://udpxy.com";

const char  COMPILE_MODE[] =
#ifdef NDEBUG
    "release";
#else
    "debug";
#endif

const char  VERSION[]       = "1.0";
const int   BUILDNUM        = 23;
const char  BUILD_TYPE[]    = "production";
const int   PATCH           = 12;
