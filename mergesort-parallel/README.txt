## Files
- `mergesort_seq_nocopy.cpp` — Sequential Merge Sort implementation
- `mergesort_parallel.cpp` — Parallel Merge Sort implementation using `std::thread`
- `Makefile` — Compiles both sequential and parallel versions
- `benchmark.sh` — SLURM batch script to compile and benchmark on Centaurus
- `benchmark_output.txt` — Output of the benchmarking job
- `benchmark_error.txt` — Error log of the benchmarking job (should be empty if successful)
- `results.csv` — CSV file containing time measurements for both versions

## compiling
module load gcc
make clean
make
