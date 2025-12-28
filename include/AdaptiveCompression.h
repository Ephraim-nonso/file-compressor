#ifndef ADAPTIVE_COMPRESSION_H
#define ADAPTIVE_COMPRESSION_H

#include "CompressionAlgorithm.h"
#include "IdentityCompression.h"
#include "RLECompression.h"

/**
 * @brief Adaptive lossless compressor that chooses the smaller of:
 * - RLE-compressed payload
 * - raw (identity) payload
 *
 * Encoding format:
 * - empty input -> empty output
 * - non-empty: [1 byte algorithm id][payload...]
 *   - 'R' => payload is RLE-compressed bytes
 *   - 'I' => payload is uncompressed original bytes
 */
class AdaptiveCompression : public CompressionAlgorithm {
public:
    std::vector<char> compress(const std::vector<char>& data) override;
    std::vector<char> decompress(const std::vector<char>& data) override;

private:
    RLECompression rle;
    IdentityCompression identity;
};

#endif


