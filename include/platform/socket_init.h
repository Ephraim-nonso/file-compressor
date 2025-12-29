#ifndef PLATFORM_SOCKET_INIT_H
#define PLATFORM_SOCKET_INIT_H

/**
 * Cross-platform socket initialization.
 *
 * - POSIX: no-op
 * - Windows: ensures WSAStartup has been called exactly once per process
 */

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>

#  include <mutex>
#  include <stdexcept>

inline void ensure_socket_init() {
    static std::once_flag once;
    std::call_once(once, []() {
        WSADATA wsa{};
        const int rc = WSAStartup(MAKEWORD(2, 2), &wsa);
        if (rc != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    });
}
#else
inline void ensure_socket_init() {}
#endif

#endif


