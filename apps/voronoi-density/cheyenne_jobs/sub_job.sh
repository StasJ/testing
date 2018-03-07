#!/bin/bash
#PBS -A NVST0001
#PBS -N OpenMP_job
#PBS -j oe
#PBS -m abe
#PBS -M shaomeng@ucar.edu
#PBS -q economy
#PBS -l walltime=12:00:00
### Request 10 CPUS for 10 threads
#PBS -l select=1:ncpus=36:ompthreads=36

export TMPDIR=/glade/scratch/$USER/temp
mkdir -p $TMPDIR

### Run OpenMP program with thread pinning using omplace
omplace ./voro2density.sh
