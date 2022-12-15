#ifndef VIRUS_ENCODER_DETECTOR_FILE_REPO_HPP
#define VIRUS_ENCODER_DETECTOR_FILE_REPO_HPP
#include "models/file.hpp"

class Storage;

class FileRepo {
private:
    Storage* store;

public:
    FileRepo(Storage* storage) : store(storage) {}
    void insertFile(File file);
};

#endif
