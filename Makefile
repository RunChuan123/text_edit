CXX := clang++
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -g -O0
CPPFLAGS := -DDEBUG

TARGET := main
SRC := main.cpp debug_logger.hpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ 
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)
