#ifndef IDENTITY_COMPRESSION_H
#define IDENTITY_COMPRESSION_H

#include "CompressionAlgorithm.h"

/**
 * @brief A no-op (identity) compression algorithm.
 *
 * This is useful as a safe fallback when a real compressor would expand the data.
 * It is always lossless and always succeeds.
 */
class IdentityCompression : public CompressionAlgorithm {
public:
    std::vector<char> compress(const std::vector<char>& data) override;
    std::vector<char> decompress(const std::vector<char>& data) override;
};

#endif


