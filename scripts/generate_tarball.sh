#!/bin/bash
set -eo pipefail

NAME="$1"
EOS_PREFIX="${PREFIX}/${SUBPREFIX}"
mkdir -p "${PREFIX}/bin/"
mkdir -p "${EOS_PREFIX}/bin"
mkdir -p "${EOS_PREFIX}/licenses/dcd"
mkdir -p "${EOS_PREFIX}/etc/dcd/contracts"

# install binaries 
cp -R "${BUILD_DIR}"/bin/* ${EOS_PREFIX}/bin

# install licenses
cp -R "${BUILD_DIR}"/licenses/dcd/* ${EOS_PREFIX}/licenses

# install bios and boot contracts
cp -R "${BUILD_DIR}"/contracts/contracts/dcd.bios/dcd.bios.* ${EOS_PREFIX}/etc/dcd/contracts
cp -R "${BUILD_DIR}"/contracts/contracts/dcd.boot/dcd.boot.* ${EOS_PREFIX}/etc/dcd/contracts

for f in $(ls "${BUILD_DIR}/bin/"); do
    bn=$(basename "$f")
    ln -sf ../"${SUBPREFIX}/bin/$bn" "${PREFIX}/bin/$bn"
done
echo "Compressing '$NAME.tar.gz'..."
tar -cvzf "$NAME.tar.gz" ./"${PREFIX}"/*
rm -r "${PREFIX}"
