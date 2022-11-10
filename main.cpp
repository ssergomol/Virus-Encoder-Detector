#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <bitset>

namespace fs = std::filesystem;

const char KEY = 57;

int encodeFile(const fs::path &filePath) {
    std::error_code errorCode;
    std::uintmax_t fileSize = fs::file_size(filePath, errorCode);
    FILE *fp;

    std::vector<char> buffer(fileSize);

    fp = fopen(filePath.c_str(), "r");
    if (!fp) {
        std::cerr << "File opening in read failed\n";
        return errno;
    }

    // Copy file content to buffer
    size_t bytesRead = fread(&buffer[0], 1, fileSize, fp);
    if (bytesRead != fileSize) {
        std::cerr << "File " << filePath << " size is " << fileSize << ", but has been read " << bytesRead << "\n";
        return 1;
    }
    fclose(fp);

    // Encode content in buffer
    for (size_t i = 0; i < fileSize; i++) {
        buffer[i] ^= KEY;
    }

    fp = fopen(filePath.c_str(), "w");
    if (!fp) {
        std::cerr << "File opening on write failed\n";
        return errno;
    }

    // Write encoded content back to file
    size_t bytesWritten = fwrite(&buffer[0], 1, fileSize, fp);
    if (bytesWritten != fileSize) {
        std::cerr << "File " << filePath << " size is " << fileSize << ", but has been written " << bytesWritten
                  << "\n";
        return 1;
    }
    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    std::string targetPath;

    // Check if there is a valid argument number
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [target path]" << std::endl;
        exit(EXIT_FAILURE);
    }

    targetPath = argv[1];
    for (const auto &dirEntry: fs::recursive_directory_iterator(targetPath)) {
        if (!dirEntry.is_directory()) {
            encodeFile(dirEntry.path());
        }
        std::cout << dirEntry.path() << std::endl;
    }

    return 0;
}