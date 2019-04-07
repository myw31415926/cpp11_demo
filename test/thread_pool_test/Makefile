# thread_pool_test Makefile

TARGET = ../_build/thread_pool_test

INCS = -I../../util/
SRCS = $(wildcard *.cpp)

CFLAGS = -Wall -g -std=c++11
LFLAGS = -pthread

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(INCS) $(SRCS) $(CFLAGS) $(LFLAGS)

.PHONY: clean
clean:
	rm $(TARGET)
