#!/usr/bin/python3

import sys
import argparse
from textwrap import dedent
from pathlib import Path
from typing import List, Tuple, TextIO
from PIL import Image

Image_t = Image.Image
RGBA = Tuple[int, int, int, int]

def ceilDiv(x: int, y: int) -> int: return -(-x // y);
def floorDiv(x: int, y: int) -> int: return ( x // y);

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
pngFile = Path(args.input).resolve()
cFile = Path(args.output).resolve()

# intermediate storage (byte arrays)
greenData: List[int]
alphaData: List[int]
size: Tuple[int, int]

# TODO: read options
enableGreen = True
enableAlpha = True

# read png
with Image.open(pngFile).convert("RGBA") as img:
    img: Image_t
    rowLength: int = ceilDiv(img.width, 8) # bytes per row

    greenData = [0] * (img.height * rowLength)
    alphaData = [0] * (img.height * rowLength)
    size = (rowLength, img.height)

    for y in range(img.height):
        for xByte in range(rowLength):                
            for xPixel in range(8):
                
                x = 8*xByte + xPixel
                r,g,b,a = img.getpixel((8*xByte + xPixel, y)) \
                    if x < img.width \
                    else (0,0,0,0)
                
                greenData[ y*rowLength + xByte ] |= (g >> 7) << (7-xPixel)
                alphaData[ y*rowLength + xByte ] |= (a >> 7) << (7-xPixel)



# write c
with open(cFile, "w") as cWriter:
    
    cWriter.write("#include <stdint.h>\n")
    cWriter.write("#include \"image.h\"\n")

    if enableGreen:
        cWriter.write("\n\nstatic uint8_t green[] = {\n")
        for greenByte in greenData:
            cWriter.write(f"    0x{greenByte:02x},\n")
        cWriter.write("};\n")

    if enableAlpha: 
        cWriter.write("\n\nstatic uint8_t alpha[] = {\n")
        for alphaByte in alphaData:
            cWriter.write(f"    0x{alphaByte:02x},\n")
        cWriter.write("};\n")

    cWriter.write(f"\n\nImage {pngFile.stem} = ")
    cWriter.write(f"Image({"green" if enableGreen else "NULL"}, {"alpha" if enableAlpha else "NULL"}, {size[0]}, {size[1]});\n")

