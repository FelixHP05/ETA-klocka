#!/usr/bin/python3

import sys
import argparse
from textwrap import dedent
from pathlib import Path

#parse input
parser = argparse.ArgumentParser()
parser.add_argument("-i", "--input")
parser.add_argument("-o", "--output")
args = parser.parse_args()

#validate input (kindof)
if (not args.input):
    print("missing argument --input\n")
    sys.exit(1)

if (not args.output):
    print("missing argument --output\n")
    sys.exit(1)

# paths
xbmFile = Path(args.input).resolve()
cFile = Path(args.output).resolve()
xbmIncludeDir = Path(__file__).resolve().parent / "images"


# write file
with open(cFile, "w") as cWriter:
    cWriter.write(dedent(
        f"""
        #include "{xbmFile.relative_to( xbmIncludeDir)}"
        #include "image.h"

        Image {xbmFile.stem} = Image((uint8_t*)_bits, NULL, _width, _height);
        """
    ))
