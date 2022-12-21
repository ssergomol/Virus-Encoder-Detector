CXX = g++
cc = gcc

BUILT_LIB = libs 
LIB_PATH = include
LIB_SQL = -lpthread -ldl -I$(LIB_PATH)
LIB = -lpthread -ldl -std=c++20 -DSQLITE_DEFAULT_FILE_PERMISSIONS=600 -I$(LIB_PATH)
BIN = sqlite exe
TARGET = test
YAML = libs/yaml-cpp

all : $(BIN)

sqlite : $(LIB_PATH)/sqlite3/sqlite3.c $(LIB_PATH)/sqlite3/shell.c
	$(cc) -o $@ $^ $(LIB_SQL)

exe : main.cpp database/db.cpp database/file_repo.cpp database/models/file.cpp database/white_list_repo.cpp database/black_list_repo.cpp detector/detector.cpp encoder/encoder.cpp sqlite3.o loguru.o $(YAML)/binary.cpp $(YAML)/convert.cpp $(YAML)/depthguard.cpp $(YAML)/directives.cpp $(YAML)/emit.cpp $(YAML)/emitfromevents.cpp $(YAML)/emitter.cpp $(YAML)/emitterstate.cpp $(YAML)/emitterutils.cpp $(YAML)/exceptions.cpp $(YAML)/exp.cpp $(YAML)/memory.cpp $(YAML)/nodebuilder.cpp $(YAML)/node.cpp $(YAML)/node_data.cpp $(YAML)/nodeevents.cpp $(YAML)/null.cpp $(YAML)/ostream_wrapper.cpp $(YAML)/parse.cpp $(YAML)/parser.cpp $(YAML)/regex_yaml.cpp $(YAML)/scanner.cpp $(YAML)/scanscalar.cpp $(YAML)/scantag.cpp $(YAML)/scantoken.cpp $(YAML)/simplekey.cpp $(YAML)/singledocparser.cpp $(YAML)/stream.cpp $(YAML)/tag.cpp 
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
