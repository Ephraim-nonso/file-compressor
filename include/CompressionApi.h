#ifndef COMPRESSION_API_H
#define COMPRESSION_API_H

#include "HttpTypes.h"
#include "AdaptiveCompression.h"

/**
 * @brief HTTP handler that exposes compression/decompression endpoints.
 *
 * Endpoints (binary body):
 * - POST /compress   -> returns compressed bytes (application/octet-stream)
 * - POST /decompress -> returns decompressed bytes (application/octet-stream)
 *
 * Uses AdaptiveCompression by default (RLE when beneficial, otherwise identity).
 */
class CompressionApi {
public:
    HttpResponse handle(const HttpRequest& req) const;

private:
    mutable AdaptiveCompression algo;
};

#endif




