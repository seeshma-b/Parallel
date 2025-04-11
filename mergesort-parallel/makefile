CXX = g++
CXXFLAGS = -std=c++11 -O3 -pthread

all: mergesort_seq mergesort_parallel

mergesort_seq: mergesort_seq_nocopy.cpp
	$(CXX) $(CXXFLAGS) -o mergesort_seq mergesort_seq_nocopy.cpp

mergesort_parallel: mergesort_parallel.cpp
	$(CXX) $(CXXFLAGS) -o mergesort_parallel mergesort_parallel.cpp

clean:
	rm -f mergesort_seq mergesort_parallel *.o
