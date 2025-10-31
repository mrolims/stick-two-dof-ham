import subprocess
import sys
from pathlib import Path

from utils import compile_target

ENERGY = 1.0 / 8.0
TOTAL_TIME = 1e8
FINITE_TIMES = [100, 250, 500, 750, 1000]
THRESHOLD = 0.05

DSYS = "HH"
CC = "cc"
if len(sys.argv) > 1:
    CC = sys.argv[1]
TARGET = "finite_time_mle"

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
for finite_time in FINITE_TIMES:
    cmd = [
        str(exe),
        f"{ENERGY:.16f}",
        str(TOTAL_TIME),
        str(finite_time),
    ]
    print(" ".join(cmd))
    subprocess.run(cmd, check=True)
