#!/usr/bin/env bash
# Mostly AI GENERATED 


set -euo pipefail
LABEL="RPI-RP2"
MNT="/mnt/rp2"
UF2="build/main.uf2"
DEV="/dev/disk/by-label/$LABEL"

# Build
echo ""
echo "[build.sh] Writing build rules..."
cmake -B build -G Ninja

echo ""
echo "[build.sh] Building..."
ninja -C build


# Wait for device to appear
echo ""
echo "[build.sh] Waiting for device with label "\""$LABEL"\"...
while [[ ! -e "$DEV" ]]; do
    sleep 0.2
done


# Mount
sudo mkdir -p "$MNT"
if mountpoint -q "$MNT"; then
    echo "[build.sh] Already mounted at $MNT"
else
    echo "[build.sh] Mounting..."
    sudo mount "$DEV" "$MNT"
fi


# flash
echo "[build.sh] Copying UF2..."
sudo cp "$UF2" "$MNT/"
sync # flush filesystem buffer


# unmount
echo "[build.sh] Unmounting..."
sudo umount "$MNT"
echo "[build.sh] Done."