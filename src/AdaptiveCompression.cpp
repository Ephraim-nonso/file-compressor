#include "AdaptiveCompression.h"

#include <stdexcept>

std::vector<char> AdaptiveCompression::compress(const std::vector<char>& data) {
    if (data.empty()) {
        return {};
    }

    auto rleBytes = rle.compress(data);
    // Identity payload is just the raw bytes (but we still add a 1-byte header).
    const size_t identitySize = 1 + data.size();
    const size_t rleSize = 1 + rleBytes.size();

    std::vector<char> out;
    if (rleSize < identitySize) {
        out.reserve(rleSize);
        out.push_back('R');
        out.insert(out.end(), rleBytes.begin(), rleBytes.end());
        return out;
    }

    out.reserve(identitySize);
    out.push_back('I');
    out.insert(out.end(), data.begin(), data.end());
    return out;
}

std::vector<char> AdaptiveCompression::decompress(const std::vector<char>& data) {
    if (data.empty()) {
        return {};
    }

    const char tag = data[0];
    std::vector<char> payload(data.begin() + 1, data.end());

    if (tag == 'R') {
        return rle.decompress(payload);
    }
    if (tag == 'I') {
        return identity.decompress(payload);
    }
    throw std::runtime_error("AdaptiveCompression::decompress: unknown algorithm tag");
}


