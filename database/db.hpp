#ifndef VIRUS_ENCODER_DETECTOR_DB_HPP
#define VIRUS_ENCODER_DETECTOR_DB_HPP
#include <sqlite3.h>
#include <iostream>

class FileRepo;

class Storage {
    sqlite3 *db{};
    FileRepo *fileRepo{};

public:
    sqlite3* getDB() const;
    void connect();
    void close();
    void initDB(const std::string& initFileName);
    FileRepo* File();
    static int callback(void *, int argc, char **argv, char **azColName);
    ~Storage();
};
#endif