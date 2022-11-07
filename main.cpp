#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

const std::string KEY{"cypher"};


int main(int argc, char **argv) {
    std::string targetPath;

    // Check if there is a valid argument number
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [target path]" << std::endl;
        return 1;
    }

    targetPath = argv[1];
    for (const auto &dirEntry: fs::recursive_directory_iterator(targetPath)) {
        if (dirEntry.is_regular_file()) {
            // XOR the file content 
        }
        std::cout << dirEntry.path() << std::endl;
    }

    return 0;
}