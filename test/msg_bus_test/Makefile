# msg_bus_test Makefile

TARGET = ../_build/msg_bus_test

INCS = -I../../util/
SRCS = $(wildcard *.cpp)

CFLAGS = -Wall -g -std=c++11
LFLAGS =

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(INCS) $(SRCS) $(CFLAGS) $(LFLAGS)

.PHONY: clean
clean:
	rm $(TARGET)
