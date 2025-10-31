import subprocess
import sys
from pathlib import Path

from utils import compile_target

TOTAL_TIME = 5000
GRID_SIZE = 500
THRESHOLD = 0.05
N_THREADS = 8

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


script_dir = Path(__file__).resolve().parent
project_root = script_dir.parents[1]
exe_path = (project_root / TARGET_FOLDER).resolve()
exe = exe_path / f"{TARGET}.x"
cmd = [str(exe), str(TOTAL_TIME), str(GRID_SIZE), str(THRESHOLD), str(N_THREADS)]
print(" ".join(cmd))
subprocess.run(cmd, check=True)
