# range_test Makefile

TARGET = ../_build/result_of_test

INCS =
SRCS = $(wildcard *.cpp)

CFLAGS = -Wall -g -std=c++11
LFLAGS =

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(INCS) $(SRCS) $(CFLAGS) $(LFLAGS)

.PHONY: clean
clean:
	rm $(TARGET)
