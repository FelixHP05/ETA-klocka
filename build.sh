#!/usr/bin/env bash
# Mostly AI GENERATED 


set -euo pipefail
LABEL="RPI-RP2"
MNT="/mnt/rp2"
UF2="build/main.uf2"
DEV="/dev/disk/by-label/$LABEL"

# Build
echo "Building..."
cmake -B build -G Ninja
ninja -C build
echo ""
echo ""


# Wait for device to appear
echo "Waiting for device with label "\""$LABEL"\"...
while [[ ! -e "$DEV" ]]; do
    sleep 0.2
done


# Mount
sudo mkdir -p "$MNT"
if mountpoint -q "$MNT"; then
    echo "Already mounted at $MNT"
else
    echo "Mounting..."
    sudo mount "$DEV" "$MNT"
fi


# flash
echo "Copying UF2..."
sudo cp "$UF2" "$MNT/"
sync # flush filesystem buffer


# unmount
echo "Unmounting..."
sudo umount "$MNT"
echo "Done."