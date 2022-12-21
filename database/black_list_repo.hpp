#ifndef VIRUS_ENCODER_DETECTOR_BLACK_LIST_REPO_HPP
#define VIRUS_ENCODER_DETECTOR_BLACK_LIST_REPO_HPP
#include <iostream>
#include <vector>

class Storage;

class BlackListRepo {
private:
    Storage* store;

public:
    BlackListRepo(Storage* storage) : store(storage) {}
    void addExe(const std::string& path);
    bool contains(const std::string&) const;
};

#endif //VIRUS_ENCODER_DETECTOR_BLACK_LIST_REPO_HPP
