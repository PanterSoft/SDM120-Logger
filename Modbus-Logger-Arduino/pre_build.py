from SCons.Script import Import
import shutil
import os

def before_build(source, target, env):
    print("Running pre-build script...")

    # Define source and destination paths
    source_file = "SDM_Config_User.h"  # Adjust this path
    # Find the SDM package path
    sdm_package_path = os.path.join(env['PROJECT_DIR'], '.pio', 'libdeps', 'esp32dev', 'SDM')
    destination_folder = os.path.join(sdm_package_path, 'src')  # Adjust this path if necessary

    # Ensure the destination folder exists
    os.makedirs(destination_folder, exist_ok=True)

    # Copy the file
    shutil.copy(source_file, destination_folder)
    print(f"Copied {source_file} to {destination_folder}")

# Register the build script
Import("env")
env.AddPreAction("build", before_build)