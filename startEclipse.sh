#!/usr/bin/bash

# -----------------------------------------------------
# 1. Receive arguments from CMD launcher
# -----------------------------------------------------
WIN_TOOLCHAIN_DIR="$1"
WIN_ECLIPSE_EXE="$2"

echo "DEBUG: Raw argument 1 = '$1'"
echo "DEBUG: Raw argument 2 = '$2'"
echo "DEBUG: WIN_TOOLCHAIN_DIR = '$WIN_TOOLCHAIN_DIR'"
echo "DEBUG: WIN_ECLIPSE_EXE   = '$WIN_ECLIPSE_EXE'"

# -----------------------------------------------------
# 2. Convert Windows paths → MSYS2 paths
# -----------------------------------------------------
TOOLCHAIN_DIR="$(cygpath -u "$WIN_TOOLCHAIN_DIR")"
ECLIPSE_EXE="$(cygpath -u "$WIN_ECLIPSE_EXE")"

echo "TOOLCHAIN_DIR (MSYS2): $TOOLCHAIN_DIR"
echo "ECLIPSE_EXE (MSYS2):   $ECLIPSE_EXE"

# -----------------------------------------------------
# 3. Validate toolchain path
# -----------------------------------------------------
if [ ! -x "$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-gcc" ]; then
    echo "ERROR: GCC not found in toolchain:"
    echo "  $TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-gcc"
    exit 1
fi

# -----------------------------------------------------
# 4. Validate eclipse path
# -----------------------------------------------------
if [ ! -x "$ECLIPSE_EXE" ]; then
    echo "ERROR: Eclipse executable not found:"
    echo "  $ECLIPSE_EXE"
    exit 1
fi

# -----------------------------------------------------
# 5. Add MSYS2 and Toolchain binaries to PATH
# -----------------------------------------------------
export PATH="/usr/bin:$TOOLCHAIN_DIR/bin:$PATH"
export TOOLCHAIN_DIR
export SHELL=/usr/bin/bash

export CC="$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-gcc"
export CXX="$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-g++"
export AR="$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-ar"
export LD="$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-ld"
export STRIP="$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-strip"

# -----------------------------------------------------
# 6. Test toolchain
# -----------------------------------------------------
"$TOOLCHAIN_DIR/bin/arm-none-linux-gnueabihf-gcc" --version

# -----------------------------------------------------
# 7. Launch Eclipse
# -----------------------------------------------------
"$ECLIPSE_EXE" -data "$HOME/eclipse-workspace" &