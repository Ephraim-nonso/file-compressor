#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>

/**
 * @brief Simple binary file I/O helper.
 *
 * Encapsulates safe read/write operations used by tests and higher-level components.
 */
class FileHandler {
public:
    /**
     * @brief Read entire file into memory (binary-safe).
     * @throws std::runtime_error on failure.
     */
    static std::vector<char> readFile(const std::string& filename);

    /**
     * @brief Write all bytes to file (binary-safe).
     * @throws std::runtime_error on failure.
     */
    static void writeFile(const std::string& filename, const std::vector<char>& data);
};

#endif


