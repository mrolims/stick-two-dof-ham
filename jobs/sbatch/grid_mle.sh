#!bin/bash

#SBATCH -t 24:00:00 -c 10
#SBATCH --mail-user=rolim.sales@unesp.br --mail-type=END,FAIL
#SBATCH --output=../../logs/slurm/%x_%j.out

export INPUT="../../simulations/henon_heiles/grid_mle.x"
export OUTPUT="*.dat"

job-nanny ./"$INPUT" "$@"
