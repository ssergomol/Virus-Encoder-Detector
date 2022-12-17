#include "db.hpp"
#include "file_repo.hpp"
#include "white_list_repo.hpp"
#include "black_list_repo.hpp"
#include <stdio.h>
#include <sqlite3.h>
#include <fstream>


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

void Storage::connect() {
    sqlite3 *db;
    int rc = sqlite3_open("/etc/Virus-Encoder-Detector/modified_files.db", &db);
//    int rc = sqlite3_open("/", &db);
    if (rc) {
        fprintf(stderr, "Can't open database connection: %s\n", sqlite3_errmsg(db));
        return;
    } else {
        printf("Database connection opened successfully\n");
        this->db = db;
    }
}

void Storage::close() {
    if (sqlite3_close(db) == SQLITE_OK) {
        printf("Database connection closed");
    } else {
        std::cerr << sqlite3_errmsg(db) << std::endl;
        fprintf(stderr, "Can't close database");
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

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Operation done successfully\n");
        }
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

WhiteListRepo* Storage::BlackList() {
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

};
