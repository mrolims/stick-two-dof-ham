import subprocess
from pathlib import Path


def create_sbatch_script(
    target,
    target_folder,
    time,
    clusters=1,
    mail_type="END,FAIL",
    output_path="logs/slurm/%x_%j.out",
):
    """
    Create and write an sbatch script that runs a compiled simulation,
    using absolute paths safe for HPC environments.
    """

    # Project root = current working directory when script runs
    project_root = Path.cwd().resolve()
    sbatch_dir = project_root / "jobs" / "sbatch"
    sbatch_dir.mkdir(parents=True, exist_ok=True)

    # Resolve absolute paths for executable and data
    exe_path = (project_root / target_folder / f"{target}.x").resolve()
    data_dir = (project_root / "data").resolve()

    # Destination .sh file
    sbatch_path = sbatch_dir / f"{target}.sh"

    # Compose script text
    script = f"""#!/bin/bash

#SBATCH -t {time} -c {clusters}
#SBATCH --mail-user=rolim.sales@unesp.br --mail-type={mail_type}
#SBATCH --output={project_root}/{output_path}

export INPUT="{exe_path}"
export OUTPUT="{data_dir}/*.dat"

job-nanny "$INPUT" "$@"
"""

    # Write and make executable
    sbatch_path.write_text(script)

    print(f"✅ Created sbatch script: {sbatch_path}")
    print(f"   INPUT: {exe_path}")
    print(f"   OUTPUT: {data_dir}")

    return sbatch_path


def compile_target(target, cc="cc", dsys="HH"):
    """Remove the existing executable and recompile a single target."""
    # Path of this script → go two levels up to reach project root
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parents[1]  # two levels up: jobs/scripts → project root

    exe = project_root / f"{target}.x"

    # Remove the old executable if it exists
    exe.unlink(missing_ok=True)

    # Run make at the project root
    subprocess.run(
        ["make", f"CC={cc}", f"DSYS={dsys}", target],
        cwd=project_root,
        check=True,
    )
