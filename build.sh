#!/usr/bin/env bash
# AI GENERATED 


set -euo pipefail

LABEL="RPI-RP2"
MNT="/mnt/rp2"
UF2="build/main.uf2"

# Build
echo "Building..."
cmake -B build -G Ninja
ninja -C build

# Find device
echo "Looking for device with label: $LABEL"
DEV="$(readlink -f "/dev/disk/by-label/$LABEL" || true)"
if [[ -z "${DEV}" || ! -e "${DEV}" ]]; then
    echo "Device not found: $LABEL"
    exit 1
fi
echo "Found device: $DEV"


# Mount
sudo mkdir -p "$MNT"
if mountpoint -q "$MNT"; then
    echo "Already mounted at $MNT"
else
    echo "Mounting..."
    sudo mount "$DEV" "$MNT"
fi

echo "Copying UF2..."
sudo cp "$UF2" "$MNT/"
sync # flush filesystem buffer

echo "Unmounting..."
sudo umount "$MNT"

echo "Done."