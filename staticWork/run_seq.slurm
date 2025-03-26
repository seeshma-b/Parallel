#!/bin/bash
#SBATCH --job-name=seq_bfs
#SBATCH --output=logs/seq_output.txt
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=1
#SBATCH --partition=Centaurus

./level_client "Tom Hanks" 3
