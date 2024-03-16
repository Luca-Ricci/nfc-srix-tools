#!/usr/bin/env bash

# Create build folder
mkdir build
cd build
TEMP_DIR=$(mktemp -d build.XXXXXXXX)
cd "${TEMP_DIR}"

# Build
cmake ../../
make

# Copy executables
mv srix-readTag ../
mv srix-readDump ../
mv srix-writeTag ../
mv srix-resetTag ../
mv srix-updateCdown ../

# Cleanup
cd ../
rm -fr "${TEMP_DIR}"
