#include "db.hpp"
#include "file_repo.hpp"
#include "models/file.hpp"
#include <iostream>

int main(){
    Storage db = Storage();
    db.connect();
    db.initDB("init_db.sql");
    std::vector<char> content{'1', '2', '3'};
    File file("/home/ok", content, 3);
    db.File()->insertFile(file);
    db.close();

    return 0;
}