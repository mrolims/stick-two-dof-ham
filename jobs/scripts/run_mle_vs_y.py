import subprocess
import sys
from pathlib import Path

from utils import compile_target

ENERGIES = [1 / 8]
TOTAL_TIME = 1e5
NUM_Y = int(1e4)
PY = 0.0
N_THREADS = 10

DSYS = "HH"
CC = "cc"
if len(sys.argv) > 1:
    CC = sys.argv[1]
TARGET = "mle_vs_y"

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

for e in ENERGIES:
    exe = exe_path / f"{TARGET}.x"
    cmd = [str(exe), f"{e:.16f}", str(TOTAL_TIME), str(NUM_Y), str(PY), str(N_THREADS)]
    print(" ".join(cmd))
    subprocess.run(cmd, check=True)
