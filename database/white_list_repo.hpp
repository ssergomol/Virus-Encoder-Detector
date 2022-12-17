#ifndef VIRUS_ENCODER_DETECTOR_WHITE_LIST_REPO_HPP
#define VIRUS_ENCODER_DETECTOR_WHITE_LIST_REPO_HPP
#include <iostream>
#include <vector>

class Storage;

class WhiteListRepo {
private:
    Storage* store;

public:
    WhiteListRepo(Storage* storage) : store(storage) {}
    void addExe(const std::string& path);
    bool contains(const std::string& path) const;
};


#endif //VIRUS_ENCODER_DETECTOR_WHITE_LIST_REPO_HPP
