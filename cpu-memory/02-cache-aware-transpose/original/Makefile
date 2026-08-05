CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall

TARGET = transpose
SRC = transpose.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)