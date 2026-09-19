#!/usr/bin/env bash

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

SRC="src"
BIN="bin"
ASSEMBLY="slug"

CC="${CC:-clang}"

# ------------------------------------------------------------
# Helpers
# ------------------------------------------------------------

error()
{
    echo
    echo "[ERROR] $1"
    echo
}

command_exists()
{
    command -v "$1" >/dev/null 2>&1
}

# ------------------------------------------------------------
# Check basic build tools
# ------------------------------------------------------------

if ! command_exists "$CC"; then
    error "Compiler '$CC' was not found."

    echo "Install a C compiler first."
    echo
    echo "Ubuntu/Debian:"
    echo "  sudo apt install clang"
    echo
    echo "Fedora:"
    echo "  sudo dnf install clang"
    echo
    echo "Arch:"
    echo "  sudo pacman -S clang"

    exit 1
fi

if ! command_exists pkg-config; then
    error "pkg-config was not found."

    echo "Ubuntu/Debian:"
    echo "  sudo apt install pkg-config"
    echo
    echo "Fedora:"
    echo "  sudo dnf install pkgconf-pkg-config"
    echo
    echo "Arch:"
    echo "  sudo pacman -S pkgconf"

    exit 1
fi

# ------------------------------------------------------------
# Determine Linux window system
# ------------------------------------------------------------

PLATFORM="${1:-${XDG_SESSION_TYPE:-}}"

case "$PLATFORM" in

    wayland)
        echo "Platform: Wayland"

        REQUIRED_LIBS=(
            egl
            gl
            wayland-client
            wayland-egl
            xkbcommon
        )

        PLATFORM_DEFINE="-DDPLATFORM_LINUX_WAYLAND"
        ;;

    x11)
        echo "Platform: X11"

        REQUIRED_LIBS=(
            egl
            gl
            x11
            x11-xcb
            xcb
        )

        PLATFORM_DEFINE="-DDPLATFORM_LINUX_X11"
        ;;

    *)
        error "Could not determine Linux window system."

        echo "Run:"
        echo "  ./build.sh wayland"
        echo
        echo "or:"
        echo "  ./build.sh x11"

        exit 1
        ;;
esac

# ------------------------------------------------------------
# Check development libraries
# ------------------------------------------------------------

MISSING_LIBS=()

for lib in "${REQUIRED_LIBS[@]}"; do
    if ! pkg-config --exists "$lib"; then
        MISSING_LIBS+=("$lib")
    fi
done

if [ ${#MISSING_LIBS[@]} -ne 0 ]; then

    error "Missing development libraries."

    echo "Missing pkg-config modules:"

    for lib in "${MISSING_LIBS[@]}"; do
        echo "  - $lib"
    done

    echo

    if [ "$PLATFORM" = "wayland" ]; then

        echo "Ubuntu/Debian:"
        echo "  sudo apt install \\"
        echo "      libwayland-dev \\"
        echo "      libxkbcommon-dev \\"
        echo "      libegl1-mesa-dev \\"
        echo "      libgl1-mesa-dev"

        echo
        echo "Fedora:"
        echo "  sudo dnf install \\"
        echo "      wayland-devel \\"
        echo "      libxkbcommon-devel \\"
        echo "      mesa-libEGL-devel \\"
        echo "      mesa-libGL-devel"

        echo
        echo "Arch:"
        echo "  sudo pacman -S \\"
        echo "      wayland \\"
        echo "      libxkbcommon \\"
        echo "      libglvnd"

    else

        echo "Ubuntu/Debian:"
        echo "  sudo apt install \\"
        echo "      libx11-dev \\"
        echo "      libx11-xcb-dev \\"
        echo "      libxcb1-dev \\"
        echo "      libegl1-mesa-dev \\"
        echo "      libgl1-mesa-dev"

        echo
        echo "Fedora:"
        echo "  sudo dnf install \\"
        echo "      libX11-devel \\"
        echo "      libX11-xcb \\"
        echo "      libxcb-devel \\"
        echo "      mesa-libEGL-devel \\"
        echo "      mesa-libGL-devel"

        echo
        echo "Arch:"
        echo "  sudo pacman -S \\"
        echo "      libx11 \\"
        echo "      libxcb \\"
        echo "      libglvnd"

    fi

    echo
    exit 1
fi

echo "All required development libraries found."

# ------------------------------------------------------------
# Get compiler/linker flags from pkg-config
# ------------------------------------------------------------

PKG_CFLAGS="$(pkg-config --cflags "${REQUIRED_LIBS[@]}")"
PKG_LIBS="$(pkg-config --libs "${REQUIRED_LIBS[@]}")"

# ------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------
COMPILER_FLAGS=(
    -Wall
    -Werror
    -Wextra

    -g
    -O0

    -Wno-system-headers
    -Wno-unused-but-set-variable
    -Wno-unused-variable
    -Wno-varargs
    -Wno-unused-private-field
    -Wno-unused-parameter
    -Wno-unused-function

    -fsanitize=undefined

    -DDPLATFORM_LINUX
    "$PLATFORM_DEFINE"
)

INCLUDES=(
    -I"$ROOT/vendor/glad"
    -I"$ROOT/vendor"
)

# ------------------------------------------------------------
# Source files
# ------------------------------------------------------------

mapfile -t SRC_FILES < <(
    find "$SRC" -type f -name '*.c'
)

SRC_FILES+=(
    "$ROOT/vendor/glad/glad.c"
)

# ------------------------------------------------------------
# Build
# ------------------------------------------------------------

mkdir -p "$BIN"

echo
echo "Building $ASSEMBLY..."

"$CC" \
    "${SRC_FILES[@]}" \
    "${COMPILER_FLAGS[@]}" \
    "${INCLUDES[@]}" \
    $PKG_CFLAGS \
    -o "$BIN/$ASSEMBLY" \
    $PKG_LIBS

echo
echo "Built: $BIN/$ASSEMBLY"
