#ifndef COMPAT_NETINET_IN_H
#define COMPAT_NETINET_IN_H

// Windows compatibility shim for <netinet/in.h>
// On non-Windows compilers, defer to the system header.

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  if defined(__has_include_next)
#    include_next <netinet/in.h>
#  else
#    include <netinet/in.h>
#  endif
#endif

#endif




