#ifndef COMPRESSION_ALGORITHM_H
#define COMPRESSION_ALGORITHM_H

#include <vector>

/**
 * @brief Abstract base class for compression algorithms.
 *
 * Demonstrates inheritance + polymorphism via a pure virtual interface.
 */
class CompressionAlgorithm {
public:
    virtual ~CompressionAlgorithm() = default;

    /**
     * @brief Compress input bytes.
     */
    virtual std::vector<char> compress(const std::vector<char>& data) = 0;

    /**
     * @brief Decompress input bytes.
     */
    virtual std::vector<char> decompress(const std::vector<char>& data) = 0;
};

#endif


