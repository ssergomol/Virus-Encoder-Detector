#include "black_list_repo.hpp"
#include "db.hpp"
#include <loguru/loguru.hpp>

#include <iostream>
#include <sqlite3/sqlite3.h>
#include <vector>

void BlackListRepo::addExe(const std::string &path) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "INSERT INTO black_list(path)"
                                                " VALUES(?)", -1, &stmt, nullptr);

    CHECK_F(rc == SQLITE_OK, "Prepare failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_bind_text(stmt, 1, const_cast<char *>(&path[0]), path.size(), SQLITE_STATIC);

    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *sql = "SELECT * FROM black_list;";
    sqlite3_exec(store->getDB(), sql, store->callback, 0, 0);
}

bool BlackListRepo::contains(const std::string& path) const {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(store->getDB(), "SELECT * FROM black_list WHERE path = ?"
            , -1, &stmt, nullptr);

    CHECK_F(rc == SQLITE_OK, "Prepare failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_bind_text(stmt, 1, const_cast<char *>(&path[0]), path.size(), SQLITE_STATIC);
    CHECK_F(rc == SQLITE_OK, "Bind failed: %s\n", sqlite3_errmsg(store->getDB()));

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // No rows returned
    if (rc == SQLITE_DONE) {
        return false;
    }

    return true;
}
