#!/bin/bash
##SBATCH --partition=compute    ### Partition
#SBATCH --job-name=PA5_G        ### Job Name
#SBATCH --time=03:00:00         ### WallTime
#SBATCH --nodes=1               ### Number of Nodes
#SBATCH --tasks-per-node=1      ### Number of tasks

rm gpu_32.csv # clear previous results

echo "imagesize,convtime,magtime,supptime,sorttime,edgetime,hysttime,d2htime,hd2time" >> gpu_32.csv
for i in 256 512 1024 2048 4096 8192 10240
do
    for((j=0;j<30;j++)) do
	    srun ./canny ~/lennas/Lenna_org_$i.pgm 0.6 >> gpu_32.csv
    done
done
