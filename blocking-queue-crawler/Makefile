CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -Irapidjson/include
LIBS = -lcurl

all: crawler

crawler: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o crawler $(LIBS)

clean:
	rm -f crawler *.o output/*.txt
