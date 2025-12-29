#ifndef COMPAT_SYS_SOCKET_H
#define COMPAT_SYS_SOCKET_H

// Windows compatibility shim for <sys/socket.h>
// On non-Windows compilers, defer to the system header.

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <BaseTsd.h>
typedef SSIZE_T ssize_t;

// POSIX shutdown() constants used in the code/tests
#  ifndef SHUT_RD
#    define SHUT_RD SD_RECEIVE
#  endif
#  ifndef SHUT_WR
#    define SHUT_WR SD_SEND
#  endif
#  ifndef SHUT_RDWR
#    define SHUT_RDWR SD_BOTH
#  endif

#else
#  if defined(__has_include_next)
#    include_next <sys/socket.h>
#  else
#    include <sys/socket.h>
#  endif
#endif

#endif




