# Makefile
CXX = g++
NVCC = nvcc
CXXFLAGS = -O3
NVFLAGS = -O3 -arch=sm_61

all: nbody nbody_gpu

nbody: nbody.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

nbody_gpu: nbody_gpu.cu
	$(NVCC) $(NVFLAGS) -o $@ $<

clean:
	rm -f nbody nbody_gpu
