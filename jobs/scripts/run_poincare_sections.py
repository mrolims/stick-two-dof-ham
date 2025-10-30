import subprocess
import sys
from pathlib import Path

from utils import compile_target

ENERGIES = [1 / i for i in range(6, 11)]
N_CROSS = 500
NUM_IC = 200

DSYS = "HH"
CC = "cc"
if len(sys.argv) > 1:
    CC = sys.argv[1]
TARGET = "poincare_section"

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
    cmd = [str(exe), f"{e:.16f}", str(N_CROSS), str(NUM_IC)]
    print(" ".join(cmd))
    subprocess.run(cmd, check=True)
