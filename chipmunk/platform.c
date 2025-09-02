#include "platform.h"

#ifdef _WIN32

// Platform initialization
void init_platform(void) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        exit(1);
    }
}

void cleanup_platform(void) {
    WSACleanup();
}

// A real implementation for fcntl's non-blocking socket functionality
int fcntl(int fd, int cmd, long arg) {
    if (cmd == F_SETFL) {
        u_long mode = (arg & O_NONBLOCK) ? 1 : 0;
        return ioctlsocket(fd, FIONBIO, &mode);
    }
    if (cmd == F_GETFL) {
        // This is a simplification. F_GETFL returns more than just O_NONBLOCK.
        // For this project, we only care about the non-blocking status.
        return 0; // Assume blocking by default
    }
    fprintf(stderr, "fcntl command %d not supported on Windows.\n", cmd);
    return -1;
}

int uname(struct utsname *buf) {
    if (!buf) {
        return -1;
    }
    strncpy(buf->sysname, "Windows", sizeof(buf->sysname) - 1);
    buf->sysname[sizeof(buf->sysname) - 1] = '\0';
    strncpy(buf->release, "10.0", sizeof(buf->release) - 1);
    buf->release[sizeof(buf->release) - 1] = '\0';
    strncpy(buf->version, "10.0", sizeof(buf->version) - 1);
    buf->version[sizeof(buf->version) - 1] = '\0';
    strncpy(buf->machine, "x86_64", sizeof(buf->machine) - 1);
    buf->machine[sizeof(buf->machine) - 1] = '\0';
    return 0;
}


#else

// On non-Windows platforms, these functions are empty because
// the necessary initialization/cleanup is not required or is handled elsewhere.
void init_platform(void) {
    // No-op on POSIX
}

void cleanup_platform(void) {
    // No-op on POSIX
}

#endif
