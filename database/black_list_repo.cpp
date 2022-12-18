#include "black_list_repo.hpp"
#include "db.hpp"

#include <iostream>
#include <sqlite3.h>
#include <vector>

void BlackListRepo::addExe(const std::string &path) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "INSERT INTO black_list(path)"
                                                " VALUES(?)", -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "prepare failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return;
    }

    rc = sqlite3_bind_int(stmt, 1, reinterpret_cast<char *>(&path[0]), path.size(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
    }
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *sql = "SELECT * FROM black_list;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}

bool BlackListRepo::contains(const std::string& path) const {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "SELECT * FROM black_list WHERE path = ?"
            , -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "prepare failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, reinterpret_cast<char *>(&path[0]), path.size(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "bind failed: " << sqlite3_errmsg(store->getDB()) << std::endl;
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // No rows returned
    if (rc == SQLITE_DONE) {
        return false;
    }

    return true;
}
