#include "FileHandler.h"
#include <fstream>
#include <stdexcept>

std::vector<char> FileHandler::readFile(const std::string& filename) {
    // Open in binary mode so bytes are read verbatim (no newline translation).
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        // Fail fast with a descriptive error instead of returning partial data.
        throw std::runtime_error("FileHandler::readFile: failed to open '" + filename + "'");
    }

    // Determine file size by seeking to end and asking for the current position to avoid reallocation during reads.
    in.seekg(0, std::ios::end);
    std::streampos size = in.tellg();
    if (size < 0) {
        throw std::runtime_error("FileHandler::readFile: failed to stat '" + filename + "'");
    }
    in.seekg(0, std::ios::beg);

    // Allocate the exact number of bytes and then read them in one call.
    std::vector<char> data(static_cast<size_t>(size));
    if (!data.empty()) {
        in.read(data.data(), static_cast<std::streamsize>(data.size()));
        if (!in) {
            // If the stream enters a failed state, the read did not complete.
            throw std::runtime_error("FileHandler::readFile: failed to read '" + filename + "'");
        }
    }
    return data;
}

void FileHandler::writeFile(const std::string& filename, const std::vector<char>& data) {
    // Open in binary mode and truncate so the output exactly matches `data`.
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("FileHandler::writeFile: failed to open '" + filename + "'");
    }

    if (!data.empty()) {
        // Write all bytes in a single operation (binary-safe).
        out.write(data.data(), static_cast<std::streamsize>(data.size()));
        if (!out) {
            throw std::runtime_error("FileHandler::writeFile: failed to write '" + filename + "'");
        }
    }

    // Explicit flush so callers that immediately re-open the file see all bytes.
    out.flush();
    if (!out) {
        throw std::runtime_error("FileHandler::writeFile: failed to flush '" + filename + "'");
    }
}


