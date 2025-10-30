#!/bin/bash

#SBATCH -t 24:00:00 -c 10
#SBATCH --mail-user=rolim.sales@unesp.br --mail-type=END,FAIL
#SBATCH --output=/home/rolim/Research/stick-two-dof-ham/logs/slurm/%x_%j.out

export INPUT="/home/rolim/Research/stick-two-dof-ham/simulations/henon_heiles/grid_mle.x"
export OUTPUT="/home/rolim/Research/stick-two-dof-ham/data/*.dat"

job-nanny "$INPUT" "$@"
