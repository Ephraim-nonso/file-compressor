#include "CompressionApi.h"

#include <stdexcept>
#include <string>

namespace {
HttpResponse textError(int code, const std::string& msg) {
    HttpResponse res;
    res.statusCode = code;
    res.statusText = (code == 404) ? "Not Found" : (code == 405) ? "Method Not Allowed" : "Bad Request";
    res.headers["Content-Type"] = "text/plain; charset=utf-8";
    res.body.assign(msg.begin(), msg.end());
    return res;
}
} // namespace

HttpResponse CompressionApi::handle(const HttpRequest& req) const {
    if (req.method != "POST") {
        return textError(405, "Only POST is supported.\n");
    }

    try {
        if (req.path == "/compress") {
            HttpResponse res;
            res.headers["Content-Type"] = "application/octet-stream";
            res.body = algo.compress(req.body);
            return res;
        }
        if (req.path == "/decompress") {
            HttpResponse res;
            res.headers["Content-Type"] = "application/octet-stream";
            res.body = algo.decompress(req.body);
            return res;
        }
        return textError(404, "Unknown endpoint.\n");
    } catch (const std::exception& e) {
        return textError(400, std::string("Error: ") + e.what() + "\n");
    }
}


