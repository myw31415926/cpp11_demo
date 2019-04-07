# sqlite3_test Makefile

TARGET = ../_build/sqlite3_test

INCS = -I../../util/  -I../../lib/sqlite3/include
SRCS = $(wildcard *.cpp)

CFLAGS = -Wall -g
LFLAGS = -L../../lib/sqlite3/lib -lsqlite3 -lpthread -ldl

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(INCS) $(SRCS) $(CFLAGS) $(LFLAGS)

.PHONY: clean
clean:
	rm $(TARGET)
