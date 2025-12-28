#ifndef HTTP_TYPES_H
#define HTTP_TYPES_H

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Minimal HTTP request/response types for our embedded API server.
 *
 * Intentionally small: enough to support POST /compress and POST /decompress
 * with binary request bodies.
 */
struct HttpRequest {
    std::string method;   // "POST"
    std::string path;     // "/compress"
    std::string version;  // "HTTP/1.1"
    std::unordered_map<std::string, std::string> headers;
    std::vector<char> body;
};

struct HttpResponse {
    int statusCode = 200;
    std::string statusText = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::vector<char> body;
};

#endif


