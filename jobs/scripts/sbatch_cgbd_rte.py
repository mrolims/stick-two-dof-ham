import os
import sys

from utils import compile_target, create_sbatch_script

N_CROSSES = [5000, 10000]
GRID_SIZE = 1000
THRESHOLD = 0.05
N_THREADS = 15

DSYS = "HH"
CC = "cc"
if len(sys.argv) > 1:
    CC = sys.argv[1]
TARGET = "cgbd_rte"

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

for n_cross in N_CROSSES:
    CMD = f"sbatch {SBATCH_SCRIPT} {n_cross} {GRID_SIZE}  {THRESHOLD} {N_THREADS}"
    print(CMD)
    os.system(CMD)
