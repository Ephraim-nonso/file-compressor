#ifndef COMPAT_UNISTD_H
#define COMPAT_UNISTD_H

// Windows compatibility shim for <unistd.h> used by tests for read()/close().
// On non-Windows compilers, defer to the system header.

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <BaseTsd.h>
typedef SSIZE_T ssize_t;

// Provide POSIX-like read/close wrappers for SOCKET usage in tests.
inline ssize_t read(SOCKET s, void* buf, size_t len) {
    return ::recv(s, static_cast<char*>(buf), static_cast<int>(len), 0);
}

inline int close(SOCKET s) {
    return ::closesocket(s);
}

#else
#  if defined(__has_include_next)
#    include_next <unistd.h>
#  else
#    include <unistd.h>
#  endif
#endif

#endif


