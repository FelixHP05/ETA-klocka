#!/usr/bin/env bash
# Mostly AI GENERATED 


set -euo pipefail
PRINT_PREFIX="\033[1;35m[build.sh]\033[0m"
LABEL="RPI-RP2"
MNT="/mnt/rp2"
UF2="build/main.uf2"
DEV="/dev/disk/by-label/$LABEL"

# Build
printf "\n\n$PRINT_PREFIX Generating build files...\n"
cmake -B build -G Ninja

printf "\n\n$PRINT_PREFIX Building...\n"
ninja -C build


# Wait for device to appear
printf "\n\n$PRINT_PREFIX Waiting for device with label "\""$LABEL"\"..."\n"
while [[ ! -e "$DEV" ]]; do
    sleep 0.2
done


# Mount
sudo mkdir -p "$MNT"
if mountpoint -q "$MNT"; then
    printf "$PRINT_PREFIX Already mounted at $MNT\n"
else
    printf "$PRINT_PREFIX Mounting...\n"
    sudo mount "$DEV" "$MNT"
fi


# flash
printf "$PRINT_PREFIX Copying UF2...\n"
sudo cp "$UF2" "$MNT/"
sync # flush filesystem buffer


# unmount
printf "$PRINT_PREFIX Unmounting...\n"
sudo umount "$MNT"
printf "$PRINT_PREFIX Done.\n"