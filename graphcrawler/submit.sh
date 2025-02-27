#!/bin/bash
#SBATCH --job-name=graph_crawler
#SBATCH --output=output_%j.txt  
#SBATCH --error=error_%j.txt    
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:10:00          
#SBATCH --mem=4G                 
#SBATCH --partition=Centaurus        

# load required modules
module load gcc curl rapidjson

# run BFS crawler with depth 3
./crawler "Tom%20Hanks" 3
