#include <iostream>
#include "file_repo.hpp"
#include "models/file.hpp"
#include <sqlite3.h>
#include "db.hpp"

void FileRepo::insertFile(File file) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "INSERT INTO files(path, content, size)"
                                                " VALUES(?, ?, ?)", -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "prepare failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_bind_text(stmt, 1, reinterpret_cast<char *>(&file.getFileName()[0]),
                           file.getFileName().size(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_bind_blob(stmt, 2, static_cast<void *>(file.getContent().data()),
                           file.getContent().size(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_bind_int(stmt, 3, file.getSize());
    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
    }
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char* sql = "SELECT * from files;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}