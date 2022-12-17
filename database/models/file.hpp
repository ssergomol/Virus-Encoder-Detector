#ifndef VIRUS_ENCODER_DETECTOR_FILE_HPP
#define VIRUS_ENCODER_DETECTOR_FILE_HPP

#include <vector>
#include <iostream>

class File {
    std::string fileName;
    std::vector<char> content;
    unsigned int size;
    unsigned int pid;

public:
    File(std::string name, std::vector<char> cont,
         unsigned int length, unsigned int processPID)
            : fileName(name), content(cont),
            size(length), pid(processPID) {}

    File(std::string);

    std::string getFileName() const;

    std::vector<char> getContent() const;

    unsigned int getSize() const;

    unsigned int getPID() const;
};

#endif
