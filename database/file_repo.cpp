#include <iostream>
#include <filesystem>
#include "file_repo.hpp"
#include "models/file.hpp"
#include <sqlite3.h>
#include "db.hpp"
#include "../encoder/encoder.hpp"
#include <loguru.hpp>
#include <cstring>


void FileRepo::insertFile(File file) {
    sqlite3_stmt *stmt;
    const char* query = "INSERT INTO modified_files(path, pid) VALUES(?1, ?2)";
    int rc = sqlite3_prepare_v2(store->getDB(), query, strlen(query),
                                &stmt, nullptr);

    CHECK_F(rc == SQLITE_OK, "Prepare failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_bind_text(stmt, 1, file.getFileName().c_str(),
                           file.getFileName().size(), SQLITE_TRANSIENT);
    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));
//
//    rc = sqlite3_bind_blob(stmt, 2, static_cast<void *>(file.getContent().data()),
//                           file.getContent().size(), SQLITE_STATIC);
//    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));
//
//    rc = sqlite3_bind_int(stmt, 3, file.getSize());
//    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_bind_int(stmt, 2, file.getPID());
    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *sql = "SELECT * from modified_files;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}

void FileRepo::removeOutOfList(int pid) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "DELETE FROM modified_files WHERE pid = ?", -1, &stmt, nullptr);

    CHECK_F(rc == SQLITE_OK, "Prepare failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_bind_int(stmt, 1, pid);

    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    const char *sql = "SELECT * from modified_files;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}

void FileRepo::recoverFiles(int pid) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "SELECT path FROM modified_files WHERE pid = ?",
                                -1, &stmt, nullptr);

    CHECK_F(rc == SQLITE_OK, "Prepare failed: %s\n", sqlite3_errmsg(store->getDB()));
    rc = sqlite3_bind_int(stmt, 1, pid);

    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_step(stmt);

    while (rc == SQLITE_ROW) {
        const char* pathString = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const std::filesystem::path path = pathString;

        // decode encoded file
        Encoder::encodeFile(path);
        printf("Path decoded: '%s'\n", pathString);
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
}

bool FileRepo::contains(const std::string& path) const {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "SELECT * FROM modified_files WHERE path = ?"
            , -1, &stmt, nullptr);

    CHECK_F(rc == SQLITE_OK, "Prepare failed: %s\n", sqlite3_errmsg(store->getDB()));

    if (rc == SQLITE_OK) {
        LOG_F(INFO, "OK: %d", rc);
    }

    rc = sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), SQLITE_TRANSIENT);
    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));

    if (rc == SQLITE_OK) {
        LOG_F(INFO, "OK: %d", rc);
    }

//    rc = sqlite3_step(stmt);
//    sqlite3_finalize(stmt);
//
//    // IF no rows returned
//    if (rc == SQLITE_DONE) {
//        return false;
//    }

    return true;
}