#ifndef VIRUS_ENCODER_DETECTOR_FILE_HPP
#define VIRUS_ENCODER_DETECTOR_FILE_HPP

#include <vector>
#include <iostream>

class File {
    std::string fileName;
//    std::vector<char> content;
//    unsigned int size;
    unsigned int pid;

public:
    File(std::string name, unsigned int processPID)
            : fileName(name), pid(processPID) {}

//    File(std::string path, int pid);

    std::string getFileName() const;

//    std::vector<char> getContent() const;

//    unsigned int getSize() const;

    unsigned int getPID() const;
};

#endif
