#ifndef COMPAT_ARPA_INET_H
#define COMPAT_ARPA_INET_H

// Windows compatibility shim for <arpa/inet.h>
// On non-Windows compilers, defer to the system header.

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  if defined(__has_include_next)
#    include_next <arpa/inet.h>
#  else
#    include <arpa/inet.h>
#  endif
#endif

#endif




