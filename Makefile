CXX = g++
cc = gcc

BUILT_LIB = libs 
LIB_PATH = include
LIB_SQL = -lpthread -ldl -I$(LIB_PATH)
LIB = -lpthread -ldl -std=c++20 -lyaml-cpp -DSQLITE_DEFAULT_FILE_PERMISSIONS=600 -I$(LIB_PATH)
BIN = sqlite exe
TARGET = test

all : $(BIN)

sqlite : $(LIB_PATH)/sqlite3/sqlite3.c $(LIB_PATH)/sqlite3/shell.c
	$(cc) -o $@ $^ $(LIB_SQL)

exe : main.cpp database/db.cpp database/file_repo.cpp database/models/file.cpp database/white_list_repo.cpp database/black_list_repo.cpp detector/detector.cpp encoder/encoder.cpp sqlite3.o loguru.o 
	$(CXX) -o $@ $^ $(LIB)

sqlite3.o : $(LIB_PATH)/sqlite3/sqlite3.c
	$(cc) -o $@ -c $^

loguru.o : $(LIB_PATH)/loguru/loguru.cpp
	$(CXX) -o $@ -c $^
 
run:
	mkdir $(TARGET)-mount && cd $(TARGET)-mount
	sudo mount --bind $(TARGET) $(TARGET)-mount
	sudo ./exe $(TARGET)-mount 

clean: 
	sudo umount $(TARGET)-mount
	rm -rf sqlite3.o loguru.o $(BIN) $(TARGET)-mount detector.db

.PHONY: all
