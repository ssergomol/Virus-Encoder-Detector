#ifndef VIRUS_ENCODER_DETECTOR_DB_HPP
#define VIRUS_ENCODER_DETECTOR_DB_HPP
#include <sqlite3.h>
#include <iostream>

class FileRepo;
class WhiteListRepo;
class BlackListRepo;

class Storage {
    sqlite3 *db;
    FileRepo *fileRepo;
    WhiteListRepo *whiteListRepo;
    BlackListRepo *blackListRepo;


public:
    sqlite3* getDB() const;
    void connect(const std::string& url);
    void close();
    void initDB(const std::string& initFileName);
    FileRepo* File();
    BlackListRepo* BlackList();
    WhiteListRepo* WhiteList();
    static int callback(void *, int argc, char **argv, char **azColName);
    ~Storage();
};
#endif
