#ifndef VIRUS_ENCODER_DETECTOR_ENCODER_HPP
#define VIRUS_ENCODER_DETECTOR_ENCODER_HPP
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>

namespace fs = std::filesystem;

class Encoder {
    static const char KEY = 57;

public:
    // Encode files byte by byte using XOR encrypting
    static int encodeFile(const fs::path &filePath);

    int start_encoder(int argc, char **argv);
};


#endif //VIRUS_ENCODER_DETECTOR_ENCODER_HPP
