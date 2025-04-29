#!/bin/bash
#SBATCH --job-name=nbody_gpu
#SBATCH --partition=GPU
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:1
#SBATCH --output=logs/gpu_output.txt

module load cuda/12.4

./nbody_gpu 100000 0.01 10 5 128
