#!/bin/bash

echo "Starting BFS benchmark runs..." | tee benchmark_results.log

for depth in 2 3 4 5; do
    echo "--------------------------------------" | tee -a benchmark_results.log
    echo "Running BFS with depth $depth..." | tee -a benchmark_results.log
    /usr/bin/time -v ./crawler "Tom Hanks" $depth 2>&1 | tee benchmark_depth_${depth}.log
    perf stat ./crawler "Tom Hanks" $depth 2>&1 | tee perf_depth_${depth}.log
    echo "--------------------------------------" | tee -a benchmark_results.log
done

echo "Benchmarking complete." | tee -a benchmark_results.log
