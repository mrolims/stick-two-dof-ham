import subprocess


def create_sbatch_script(
    target,
    target_folder,
    time,
    clusters=1,
    mail_type="END,FAIL",
    output_path="../../logs/slurm/%x_%j.out",
):

    script = f"""#!bin/bash

#SBATCH -t {time} -c {clusters}
#SBATCH --mail-user=rolim.sales@unesp.br --mail-type={mail_type}
#SBATCH --output={output_path}

export INPUT="{target_folder}/{target}.x"
export OUTPUT="*.dat"

job-nanny ./"$INPUT" "$@"
"""

    return script


def compile_target(target, cc="cc", dsys="HH"):
    """Remove the existing executable and recompile a single target."""
    project_root = "../../"
    exe = f"{target}.x"

    # Remove the old executable if it exists
    cmd = ["rm", "-f", exe]
    print(" ".join(cmd))
    subprocess.run(cmd, cwd=project_root, check=False)

    # Recompile the target
    subprocess.run(
        ["make", f"CC={cc}", f"DSYS={dsys}", target],
        cwd=project_root,
        check=True,
    )
