#!/bin/bash
set -eo pipefail

NAME="$1"
DCD_PREFIX="${PREFIX}/${SUBPREFIX}"
mkdir -p "${PREFIX}/bin/"
mkdir -p "${DCD_PREFIX}/bin"
mkdir -p "${DCD_PREFIX}/licenses/dcd"
mkdir -p "${DCD_PREFIX}/etc/dcd/contracts"

# install binaries 
cp -R "${BUILD_DIR}"/bin/* ${DCD_PREFIX}/bin

# install licenses
cp -R "${BUILD_DIR}"/licenses/dcd/* ${DCD_PREFIX}/licenses

# install bios and boot contracts
cp -R "${BUILD_DIR}"/contracts/contracts/dcd.bios/dcd.bios.* ${DCD_PREFIX}/etc/dcd/contracts
cp -R "${BUILD_DIR}"/contracts/contracts/dcd.boot/dcd.boot.* ${DCD_PREFIX}/etc/dcd/contracts

for f in $(ls "${BUILD_DIR}/bin/"); do
    bn=$(basename "$f")
    ln -sf ../"${SUBPREFIX}/bin/$bn" "${PREFIX}/bin/$bn"
done
echo "Compressing '$NAME.tar.gz'..."
tar -cvzf "$NAME.tar.gz" ./"${PREFIX}"/*
rm -r "${PREFIX}"
