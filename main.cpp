#include <iostream>
#include "database/db.hpp"
#include "database/models/file.hpp"
#include "database/file_repo.hpp"


int main() {
    Storage database = Storage();
    database.connect();
    database.initDB("database/init_db.sql");
    std::vector<char> content{'5', '4', '4', '5'};
    File file("/home/ssergomol/", content, 4);
    database.File()->insertFile(file);
    database.close();
}