#include "RLECompression.h"
#include <stdexcept>

/**
 * TODO: Implement Run-Length Encoding compression
 * 
 * This method should compress the input data using the RLE algorithm.
 * RLE works by replacing consecutive repeated characters with a pair: the character and its count.
 * 
 * Format: For each run of identical characters, output [character, count]
 * 
 * Important considerations:
 * - Handle empty input (return empty vector)
 * - Count should be limited to 255 (use unsigned char for count)
 * - Process data in a single pass
 * 
 * Example: "aaabbc" → ['a',3, 'b',2, 'c',1]
 * 
 * @param data The input data to compress
 * @return The compressed data as a vector of chars
 */
std::vector<char> RLECompression::compress(const std::vector<char>& data) {
    std::vector<char> out;
    if (data.empty()) {
        return out;
    }

    out.reserve(data.size()); // best-effort (may grow if many short runs)

    char current = data[0];
    unsigned int run = 1;

    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current && run < 255) {
            ++run;
            continue;
        }

        // flush current run
        out.push_back(current);
        out.push_back(static_cast<char>(static_cast<unsigned char>(run)));

        // start new run
        current = data[i];
        run = 1;
    }

    // flush last run
    out.push_back(current);
    out.push_back(static_cast<char>(static_cast<unsigned char>(run)));
    return out;
}

/**
 * TODO: Implement Run-Length Encoding decompression
 * 
 * This method should decompress RLE-encoded data back to the original format.
 * 
 * Format: Input is pairs of [character, count] that need to be expanded.
 * 
 * Important considerations:
 * - Handle empty input (return empty vector)
 * - Validate that data has even length (each run needs character + count)
 * - Check for zero counts (invalid data)
 * - Expand each pair by repeating the character 'count' times
 * 
 * Example: ['a',3, 'b',2, 'c',1] → "aaabbc"
 * 
 * @param data The compressed data to decompress
 * @return The decompressed data as a vector of chars
 * @throws std::runtime_error if data is malformed (odd length or zero count)
 */
std::vector<char> RLECompression::decompress(const std::vector<char>& data) {
    std::vector<char> out;
    if (data.empty()) {
        return out;
    }
    if ((data.size() % 2) != 0) {
        throw std::runtime_error("RLECompression::decompress: malformed input (odd length)");
    }

    // conservative reserve: cannot know exact size cheaply without a pre-pass, so do a quick one
    size_t total = 0;
    for (size_t i = 1; i < data.size(); i += 2) {
        unsigned char count = static_cast<unsigned char>(data[i]);
        if (count == 0) {
            throw std::runtime_error("RLECompression::decompress: malformed input (zero count)");
        }
        total += static_cast<size_t>(count);
    }
    out.reserve(total);

    for (size_t i = 0; i < data.size(); i += 2) {
        char value = data[i];
        unsigned char count = static_cast<unsigned char>(data[i + 1]);
        if (count == 0) {
            throw std::runtime_error("RLECompression::decompress: malformed input (zero count)");
        }
        out.insert(out.end(), static_cast<size_t>(count), value);
    }
    return out;
}
