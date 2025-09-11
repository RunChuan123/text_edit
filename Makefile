CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra  -pedantic -g -O0
CPPFLAGS := -DDEBUG

TARGET := main
SRC := main.cpp basic_func.cpp text_editor.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ 
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)

