CXX=g++
CXXFLAGS=-std=c++17 -O2 -lcurl -pthread -I$(HOME)/rapidjson/include

all: sequential parallel

sequential: level_client.cpp
	$(CXX) $(CXXFLAGS) -o level_client level_client.cpp

parallel: parallel_level_client.cpp
	$(CXX) $(CXXFLAGS) -o parallel_level_client parallel_level_client.cpp

clean:
	rm -f level_client parallel_level_client *.o
