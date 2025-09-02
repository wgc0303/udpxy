#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    // Windows-specific includes and definitions
    #define _WIN32_WINNT 0x0600 // Required for GetAdaptersAddresses
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <iphlpapi.h>
    #include <process.h>
    #include <stdint.h>
    #include <time.h>
    #include <io.h>
    #include <sys/stat.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <limits.h>
    #include <assert.h>

    // POSIX types
    typedef SSIZE_T ssize_t;
    typedef unsigned long u_long;
    typedef unsigned short u_short;
    typedef u_short flag_t;

    // POSIX functions mapping
    #define sleep(seconds) Sleep((seconds) * 1000)
    #define strcasecmp _stricmp
    #define ftruncate(fd, length) _chsize(fd, length)
    #define creat(path, mode) _creat(path, _S_IREAD | _S_IWRITE)
    #define pipe(fds) _pipe(fds, 512, _O_BINARY)
    #define unlink _unlink
    #define open _open
    #define read _read
    #define write _write
    #define close _close
    #define access _access
    #define R_OK 4

    // Dummy signal-related definitions
    typedef int sig_atomic_t;
    #define SIGQUIT 3
    #define SIGPIPE 13
    #define SIGCHLD 17
    #define SIGTERM 15
    #define SIGINT  2
    #define SIGHUP 1
    #define SIG_IGN (void (*)(int))1
    #define SIG_ERR ((void (*)(int))-1)
    #define SIG_UNBLOCK 1
    #define SIG_BLOCK   2
    #define SIG_SETMASK 3
    #define WNOHANG 1
    typedef int sigset_t;
    struct sigaction { void (*sa_handler)(int); int sa_mask; int sa_flags; };
    static inline int sigaction(int s, const struct sigaction *a, struct sigaction *o) { (void)s;(void)a;(void)o; return 0; }
    static inline int sigemptyset(int *s) { (void)s; return 0; }
    static inline int sigaddset(int *s, int sn) { (void)s; (void)sn; return 0; }
    static inline int sigprocmask(int h, const int *s, int *os) { (void)h;(void)s;(void)os; return 0; }
    static inline int signal(int signum, void (*handler)(int)) { return 0; }

    // Dummy process-related definitions
    static inline pid_t fork(void) { return -1; }
    static inline pid_t waitpid(pid_t p, int *s, int o) { (void)p;(void)s;(void)o; return -1; }
    static inline int kill(pid_t p, int s) { (void)p;(void)s; return 0; }
    #define setsid() 0
    #define nice(n) 0
    #define daemon(a,b) 0
    #define geteuid() 1

    // Dummy syslog
    #define LOG_CONS 0
    #define LOG_PID 0
    #define LOG_LOCAL0 0
    #define LOG_NOTICE 0
    #define openlog(i,o,f)
    #define syslog(p, f, ...) fprintf(stderr, f, ##__VA_ARGS__)
    #define closelog()

    // Structs and functions for platform differences
    struct iovec { void *iov_base; size_t iov_len; };
    static inline int writev(int fd, const struct iovec *iov, int iovcnt) { (void)fd;(void)iov;(void)iovcnt; return -1; }

    // gettimeofday is already in MinGW's time.h
    // fcntl wrapper
    #define F_GETFL 3
    #define F_SETFL 4
    #define O_NONBLOCK 0x0004
    int fcntl(int fd, int cmd, long arg);

#else // POSIX BUILD
    // POSIX-specific includes
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <net/if.h>
    #include <sys/wait.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <signal.h>
    #include <syslog.h>
    #include <libgen.h>
    #include <sys/uio.h>
    #include <sys/time.h>
    #include <sys/utsname.h>
    #include <fcntl.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <strings.h>
    #include <ctype.h>
    #include <limits.h>
    #include <errno.h>
    #include <assert.h>
    #include <time.h>
#endif

// Platform-independent function prototypes
void init_platform(void);
void cleanup_platform(void);

#endif // PLATFORM_H
