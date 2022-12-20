CREATE TABLE modified_files(
    path TEXT PRIMARY KEY,
    pid INTEGER
);

CREATE TABLE black_list(
    path TEXT PRIMARY KEY
);

CREATE TABLE white_list(
    path TEXT PRIMARY KEY
);