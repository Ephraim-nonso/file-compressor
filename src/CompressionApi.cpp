#include "CompressionApi.h"

#include <stdexcept>
#include <string>

namespace {
HttpResponse textError(int code, const std::string& msg) {
    HttpResponse res;
    res.statusCode = code;
    res.statusText = (code == 404) ? "Not Found" : (code == 405) ? "Method Not Allowed" : "Bad Request";
    res.headers["Content-Type"] = "text/plain; charset=utf-8";
    // Allow browser-based frontends (different origin) to call this API.
    res.headers["Access-Control-Allow-Origin"] = "*";
    res.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
    // Browser preflight includes both Content-Type and Accept for our frontend fetch().
    res.headers["Access-Control-Allow-Headers"] = "Content-Type, Accept";
    res.body.assign(msg.begin(), msg.end());
    return res;
}
} // namespace

HttpResponse CompressionApi::handle(const HttpRequest& req) const {
    // Handle CORS preflight from browsers.
    if (req.method == "OPTIONS") {
        HttpResponse res;
        res.statusCode = 204;
        res.statusText = "No Content";
        res.headers["Access-Control-Allow-Origin"] = "*";
        res.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
        res.headers["Access-Control-Allow-Headers"] = "Content-Type, Accept";
        return res;
    }

    // Simple health check for debugging / deployment probes.
    if (req.method == "GET" && req.path == "/health") {
        HttpResponse res;
        res.statusCode = 200;
        res.statusText = "OK";
        res.headers["Content-Type"] = "text/plain; charset=utf-8";
        res.headers["Access-Control-Allow-Origin"] = "*";
        res.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS, GET";
        res.headers["Access-Control-Allow-Headers"] = "Content-Type, Accept";
        const std::string body = "ok\n";
        res.body.assign(body.begin(), body.end());
        return res;
    }

    if (req.method != "POST") {
        return textError(405, "Only POST is supported.\n");
    }

    try {
        if (req.path == "/compress") {
            HttpResponse res;
            res.headers["Content-Type"] = "application/octet-stream";
            res.headers["Access-Control-Allow-Origin"] = "*";
            res.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
            res.headers["Access-Control-Allow-Headers"] = "Content-Type, Accept";
            res.body = algo.compress(req.body);
            return res;
        }
        if (req.path == "/decompress") {
            HttpResponse res;
            res.headers["Content-Type"] = "application/octet-stream";
            res.headers["Access-Control-Allow-Origin"] = "*";
            res.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
            res.headers["Access-Control-Allow-Headers"] = "Content-Type, Accept";
            res.body = algo.decompress(req.body);
            return res;
        }
        return textError(404, "Unknown endpoint.\n");
    } catch (const std::exception& e) {
        return textError(400, std::string("Error: ") + e.what() + "\n");
    }
}


