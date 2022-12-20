#ifndef VIRUS_ENCODER_DETECTOR_FILE_HPP
#define VIRUS_ENCODER_DETECTOR_FILE_HPP

#include <vector>
#include <iostream>

class File {
    std::string fileName;
    std::vector<char> content{'0'};
    unsigned int size = 0;
    unsigned int pid;

public:
    File(std::string name, std::vector<char> cont,
         unsigned int length, unsigned int processPID)
            : fileName(name), content(cont),
            size(length), pid(processPID) {}

    File(std::string path, int pid);

    std::string getFileName() const;

    std::vector<char> getContent() const;

    unsigned int getSize() const;

    unsigned int getPID() const;
};

#endif
