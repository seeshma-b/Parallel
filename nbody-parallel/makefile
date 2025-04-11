CXX = g++
CXXFLAGS = -O2 -std=c++11 -fopenmp
TARGET = nbody
SRC = nbody.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
