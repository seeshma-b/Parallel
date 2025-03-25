CXX=g++
CXXFLAGS=-std=c++17 -O2 -I$(HOME)/rapidjson/include
LDFLAGS=-lcurl -pthread

all: sequential parallel

sequential: level_client.cpp
	$(CXX) $(CXXFLAGS) level_client.cpp -o level_client $(LDFLAGS)

parallel: parallel_level_client.cpp
	$(CXX) $(CXXFLAGS) parallel_level_client.cpp -o parallel_level_client $(LDFLAGS)

clean:
	rm -f level_client parallel_level_client *.o
