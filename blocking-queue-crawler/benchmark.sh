#!/bin/bash
#SBATCH --job-name=bfs-bqueue
#SBATCH --output=out.txt
#SBATCH --error=err.txt
#SBATCH --partition=Centaurus
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --time=00:10:00

module load gcc/11.2.0
module load cmake
module load curl

make clean && make

./crawler "Tom Hanks" 4