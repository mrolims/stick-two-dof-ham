#!/bin/bash

#SBATCH -t 24:00:00 -c 15
#SBATCH --mail-user=rolim.sales@unesp.br --mail-type=END,FAIL
#SBATCH --output=/Users/mrolims/Library/Mobile Documents/com~apple~CloudDocs/Research/Projects/Active/stick-two-dof-ham/logs/slurm/%x_%j.out

export INPUT="/Users/mrolims/Library/Mobile Documents/com~apple~CloudDocs/Research/Projects/Active/stick-two-dof-ham/simulations/henon_heiles/cgbd_rte.x"
export OUTPUT="/Users/mrolims/Library/Mobile Documents/com~apple~CloudDocs/Research/Projects/Active/stick-two-dof-ham/data/*.dat"

job-nanny "$INPUT" "$@"
