import argparse
import subprocess
import sys
from pathlib import Path

def generate_shaders(source_folder : str, output_folder : str, dxc_exec : str) -> None:
    folder = Path(source_folder)
    for file in folder.rglob("*.hlsl"):
        if file.is_file():
            file_type = Path(file).stem
            file_target = ''
            if len(file_type) <= 2:
                print("Skipping file {} with an invalid format. "
                      "The last two characters should indicate the ".format(file))
                return 
            else:
                if file_type[-2:] == "PS":
                    file_target = 'ps_6_6'
                if file_type[-2:] == "CS":
                    file_target = 'cs_6_6'
                if file_type[-2:] == "VS":
                    file_target = 'vs_6_6'
                if file_type[-2:] == "MS":
                    file_target = 'ms_6_6'
                if file_type[-2:] == "AS":
                    file_target = 'as_6_6'

            print("Generating shader {}".format(file))
            command = [dxc_exec, '-T', file_target, '-E', 'main', '-Fo', f'{output_folder}/{Path(file).stem}.cso', file]
            result = subprocess.run(command, capture_output=True, text=True)
            if result.stderr:
                print(result.stderr)
            if result.stdout:
                print(result.stdout)
            if result.returncode != 0:
                sys.exit(result.returncode)

def main():
    parser = argparse.ArgumentParser(description='Generate shaders')
    parser.add_argument('-dxc_path', help='Path to DXC executable')
    parser.add_argument('-source_folder', help='Source folder (where the hlsl files are located)')
    parser.add_argument('-output_folder', help='Output folder (where the shaders are going to be generated)')
    args = parser.parse_args()
    print("Source folder: {}".format(args.source_folder))
    print("Output folder: {}".format(args.output_folder))
    print("DXC executable: {}".format(args.dxc_path))

    Path(args.output_folder).mkdir(parents=True, exist_ok=True)
    generate_shaders(args.source_folder, args.output_folder, args.dxc_path)

main()