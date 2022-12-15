#include "file.hpp"
#include <fstream>

File::File(std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        this->fileName = path;
        this->content = buffer;
        this->size = size;
    }
}

std::string File::getFileName() const {
    return fileName;
}

std::vector<char> File::getContent() const {
    return content;
}

unsigned int File::getSize() const {
    return size;
}
