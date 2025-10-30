import os
import sys

from utils import compile_target, create_sbatch_script

ENERGIES = [1 / i for i in range(6, 11)]
N_CROSS = 5000
GRID_SIZE = 1000
N_THREADS = 10
THRESHOLD = 0.05

DSYS = "HH"
CC = "cc"
if len(sys.argv) > 1:
    CC = sys.argv[1]
TARGET = "grid_rte"

if DSYS == "HH":
    TARGET_FOLDER = "simulations/henon_heiles"
elif DSYS == "SP":
    TARGET_FOLDER = "simulations/spring_pendulum"
else:
    TAGET_FOLDER = None
    sys.exit()
compile_target(TARGET, cc=CC, dsys=DSYS)

TIME = "24:00:00"
SBATCH_SCRIPT = create_sbatch_script(TARGET, TARGET_FOLDER, TIME, clusters=N_THREADS)

for e in ENERGIES:
    CMD = (
        f"sbatch {SBATCH_SCRIPT} {e:.16f} {N_CROSS} {GRID_SIZE} {THRESHOLD} {N_THREADS}"
    )
    print(CMD)
    os.system(CMD)
