#ifndef RLE_COMPRESSION_H
#define RLE_COMPRESSION_H

#include "CompressionAlgorithm.h"
#include <vector>

/**
 * @brief Run-Length Encoding (RLE) compression implementation.
 *
 * Encodes runs as pairs: [byte, count] where count is 1..255.
 */
class RLECompression : public CompressionAlgorithm {
public:
    std::vector<char> compress(const std::vector<char>& data) override;
    std::vector<char> decompress(const std::vector<char>& data) override;
};

#endif


