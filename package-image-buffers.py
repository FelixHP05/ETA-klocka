

import argparse
from pathlib import Path


parser = argparse.ArgumentParser()
parser.add_argument("-i", "--input")
parser.add_argument("-o", "--output")
args = parser.parse_args()


xbmFile = Path(args.input)
cFile = Path(args.input)


with open(cFile, "w") as cWriter:
    cWriter.write(
f"""
#include "${ xbmFile.relative_to()}"
#include "../src/image.h"

Image ${ } = Image(_bits, NULL, _width, _height);
"""    
    )
