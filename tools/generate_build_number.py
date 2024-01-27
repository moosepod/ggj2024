# This file is managed by pdantler.
# Any changes you make to it will be overwritten next time pdantler is run.

import argparse
import datetime

def generate_version_file():
    parser = argparse.ArgumentParser()
    parser.add_argument("version_path", help="Path to version file")
    args = parser.parse_args()

    build_number = datetime.datetime.now().isoformat()
    with open(args.version_path, "w") as f:
        f.write(f'#define BUILD_NUMBER "{build_number}"\n')

if __name__ == "__main__":
    generate_version_file()