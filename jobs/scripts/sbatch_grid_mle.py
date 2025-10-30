import os
import sys

from utils import compile_target, create_sbatch_script

ENERGIES = [1 / 8]
TOTAL_TIME = 1e4
GRID_SIZE = 540
N_THREADS = 10

DSYS = "HH"
CC = "cc"
TARGET = "grid_mle"

if DSYS == "HH":
    TARGET_FOLDER = "../../simulations/henon_heiles"
elif DSYS == "SP":
    TARGET_FOLDER = "../../simulations/spring_pendulum"
else:
    TAGET_FOLDER = None
    sys.exit()
compile_target(TARGET, cc=CC, dsys=DSYS)

SBATCH_SCRIPT = f"../sbatch/{TARGET}.sh"
TIME = "24:00:00"
SCRIPT_TEXT = create_sbatch_script(TARGET, TARGET_FOLDER, TIME, clusters=N_THREADS)
with open(f"{SBATCH_SCRIPT}", "w") as f:
    f.write(SCRIPT_TEXT)

for e in ENERGIES:
    CMD = f"sbatch {SBATCH_SCRIPT} {e:.16f} {TOTAL_TIME} {GRID_SIZE} {N_THREADS}"
    print(CMD)
    os.system(CMD)
