#include <iostream>
#include <filesystem>
#include "file_repo.hpp"
#include "models/file.hpp"
#include <sqlite3.h>
#include "db.hpp"
#include "../encoder/encoder.hpp"

void FileRepo::insertFile(File file) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "INSERT INTO files(path, content, size, pid)"
                                                " VALUES(?, ?, ?, ?)", -1, &stmt, nullptr);

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

    rc = sqlite3_bind_int(stmt, 4, file.getPID());
    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
    }
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *sql = "SELECT * from files;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}

void FileRepo::removeOutOfList(int pid) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "DELETE FROM files WHERE pid = ?", -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "prepare failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_bind_int(stmt, 1, pid);

    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    const char *sql = "SELECT * from files;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}

void FileRepo::recoverFiles(int pid) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "SELECT path FROM files WHERE pid = ?", -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "prepare failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_bind_int(stmt, 1, pid);

    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_step(stmt);

    while (rc == SQLITE_ROW) {
        auto pathString = sqlite3_column_text(stmt, 0);
        const std::filesystem::path path = pathString;

        // decode encoded file
        Encoder encoder;
        encoder.encodeFile(path);
        printf("Path decoded: '%s'\n", pathString);
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

}