#include "db.hpp"
#include "file_repo.hpp"
#include "white_list_repo.hpp"
#include "black_list_repo.hpp"
#include <stdio.h>
#include <sqlite3.h>
#include <fstream>
#include <loguru.hpp>


int Storage::callback(void *, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}


sqlite3 *Storage::getDB() const {
    return db;
}

void Storage::connect(const std::string& url) {
    sqlite3 *db;
//    int rc = sqlite3_open_v2(url.c_str(), &db,  SQLITE_OPEN_FULLMUTEX, nullptr);
    LOG_F(INFO, "Before open");
    int rc = sqlite3_open(url.c_str(), &db);
    LOG_F(INFO, "After open");
    if (rc != SQLITE_OK) {
        LOG_F(FATAL, "Can't open database connection: %s", sqlite3_errmsg(db));
        return;
    } else {
        LOG_F(INFO, "Database connection opened successfully");
        this->db = db;
    }
}

void Storage::close() {
    if (db == nullptr) {
        return;
    }

    if (sqlite3_close(db) == SQLITE_OK) {
        LOG_F(INFO, "Database connection closed");
    } else {
        LOG_F(FATAL, "Couldn't close database connection: %s", sqlite3_errmsg(db));
    }
}

void Storage::initDB(const std::string &initFileName) {
    char *zErrMsg = 0;
    std::ifstream file(initFileName, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        std::string init(buffer.begin(), buffer.end());
//            const char* sql = "SELECT * from files;";
//            const char* insert = "INSERT INTO files(path, content, size) VALUES (\"/home/ssergomol/\", '54455354', 25);";
        int rc = sqlite3_exec(this->db, init.c_str(),
                              this->callback, nullptr, &zErrMsg);

        CHECK_F(rc == SQLITE_OK, "SQL error: %s\n", zErrMsg);
    }
}

FileRepo* Storage::File() {
    if (fileRepo != nullptr) {
        return fileRepo;
    }

    this->fileRepo = new FileRepo(this);
    return fileRepo;
}

WhiteListRepo* Storage::WhiteList() {
    if (whiteListRepo != nullptr) {
        return whiteListRepo;
    }

    this->whiteListRepo = new WhiteListRepo(this);
    return this->whiteListRepo;
}

BlackListRepo* Storage::BlackList() {
    if (blackListRepo != nullptr) {
        return blackListRepo;
    }

    this->blackListRepo = new BlackListRepo(this);
    return this->blackListRepo;
}


Storage::~Storage() {
    if (fileRepo != nullptr) {
        delete(fileRepo);
    }

    if (blackListRepo != nullptr) {
        delete(blackListRepo);
    }

    if (whiteListRepo != nullptr) {
        delete(whiteListRepo);
    }

    close();
};
