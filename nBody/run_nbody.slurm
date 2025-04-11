#!/bin/bash
#SBATCH --job-name=nbody_benchmark
#SBATCH --output=nbody_output.log
#SBATCH --error=nbody_error.log
#SBATCH --time=01:00:00  # 1 hour max runtime
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --partition=Centaurus
#SBATCH --mail-type=ALL
#SBATCH --mail-user=sbabu4@uncc.edu

# Load necessary modules
module load gcc

# Run the benchmarks
./nbody 3 200 5000000 1000 > benchmark_solar.log
./nbody 100 1 10000 100 > benchmark_100.log
./nbody 1000 1 10000 100 > benchmark_1000.log

echo "Benchmark jobs completed.pls work pls"
