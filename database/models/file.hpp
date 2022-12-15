#ifndef VIRUS_ENCODER_DETECTOR_FILE_HPP
#define VIRUS_ENCODER_DETECTOR_FILE_HPP

#include <vector>
#include <iostream>

class File {
    std::string fileName;
    std::vector<char> content;
    unsigned int size;

public:
    File(std::string name, std::vector<char> cont, unsigned int length)
            : fileName(name), content(cont), size(length) {}

    File(std::string);

    std::string getFileName() const;

    std::vector<char> getContent() const;

    unsigned int getSize() const;
};

#endif
