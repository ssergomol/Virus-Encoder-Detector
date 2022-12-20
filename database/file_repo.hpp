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

    // Remove files from database which
    // were modified by process "pid"
    void removeOutOfList(int pid);

    // Decode files encoded by process "pid"
    void recoverFiles(int pid);

    bool contains(const std::string& path) const;
};

#endif
