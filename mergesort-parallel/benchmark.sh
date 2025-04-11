#!/bin/bash
#SBATCH --job-name=mergesort_benchmark
#SBATCH --partition=Centaurus
#SBATCH --output=benchmark_output.txt
#SBATCH --error=benchmark_error.txt
#SBATCH --time=00:20:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16

module load gcc

make clean
make

echo "n,time_seq,time_par" > results.csv

for n in 1000 10000 100000 1000000 10000000 50000000
do
    echo "Running sequential for n=$n"
    t_seq=$(./mergesort_seq $n 2>&1 >/dev/null)
    echo "Running parallel for n=$n"
    t_par=$(./mergesort_parallel $n 2>&1 >/dev/null)
    echo "$n,$t_seq,$t_par" >> results.csv
done
