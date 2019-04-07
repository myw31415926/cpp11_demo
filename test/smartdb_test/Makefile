# smartdb_test Makefile

TARGET = ../_build/smartdb_test

INCS = -I../../util/ -I../../lib/sqlite3/include -I../../lib/rapidjson/include
SRCS = $(wildcard *.cpp)

CFLAGS = -Wall -g -std=c++11
LFLAGS = -L../../lib/sqlite3/lib -lsqlite3

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(INCS) $(SRCS) $(CFLAGS) $(LFLAGS)

.PHONY: clean
clean:
	rm $(TARGET)
