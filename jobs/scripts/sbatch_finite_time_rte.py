import os
import sys

from utils import compile_target, create_sbatch_script

ENERGIES = [1 / 7, 1 / 8, 1 / 9]
TOTAL_TIME = int(1e9)
FINITE_TIMES = [200, 400, 600, 800, 1000]
THRESHOLD = 0.05

DSYS = "HH"
CC = "cc"
if len(sys.argv) > 1:
    CC = sys.argv[1]
TARGET = "finite_time_rte"

if DSYS == "HH":
    TARGET_FOLDER = "simulations/henon_heiles"
elif DSYS == "SP":
    TARGET_FOLDER = "simulations/spring_pendulum"
else:
    TAGET_FOLDER = None
    sys.exit()
compile_target(TARGET, cc=CC, dsys=DSYS)

TIME = "2-00"
SBATCH_SCRIPT = create_sbatch_script(TARGET, TARGET_FOLDER, TIME, clusters=1)

for finite_time in FINITE_TIMES:
    for e in ENERGIES:
        CMD = f"sbatch {SBATCH_SCRIPT} {e:.16f} {TOTAL_TIME} {finite_time} {THRESHOLD}"
        print(CMD)
        os.system(CMD)
