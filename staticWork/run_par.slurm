#!/bin/bash
#SBATCH --job-name=par_bfs
#SBATCH --output=logs/par_output.txt
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=8
#SBATCH --partition=Centaurus

./parallel_level_client "Tom Hanks" 3
