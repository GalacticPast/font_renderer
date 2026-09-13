#!/bin/bash
#!/bin/bash
set -e

SRC="src"
BIN="bin"
CC="clang"
ASSEMBLY="slug"
EXTENSION=""

INCLUDES=""
# If raylib, uncomment the following line:
# LINKER_FLAGS="-L./external/raylib/lib/windows/ -lraylib -lkernel32 -lgdi32 -luser32 -ladvapi32 -ltdh -lwinmm -lm"
LINKER_FLAGS="-DDPLATFORM_LINUX -lGL -lEGL"

COMPILER_FLAGS="-Wall -Werror -Wextra -g -O0 -Wno-system-headers -Wno-unused-but-set-variable -Wno-unused-variable -Wno-varargs -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-function -fsanitize=undefined"

linux_platform="$XDG_SESSION_TYPE"

if [ "$linux_platform" = "wayland" ]; then
    echo "Bulding for wayland" 
    LINKER_FLAGS="$LINKER_FLAGS -DDPLATFORM_LINUX_WAYLAND -lwayland-client -lxkbcommon -lwayland-egl"
    
    if [[ ! -d "src/platform/wayland" ]]; then
        echo "Bulding scaffold" 
        mkdir -p src/platform/wayland
        cd src/platform/wayland || exit 1

        # XDG Shell Protocol Generation
        wayland-scanner private-code < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > xdg-shell-protocol.c
        error=$?
        if [ $error -ne 0 ]; then
            echo "Error generating xdg-shell-protocol.c: $error"
            exit $error
        fi

        wayland-scanner client-header < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > xdg-shell-client-protocol.h
        error=$?
        if [ $error -ne 0 ]; then
            echo "Error generating xdg-shell-client-protocol.h: $error"
            exit $error
        fi

        wayland-scanner private-code < /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml > xdg-decoration-unstable-v1-protocol.c
        error=$?
        if [ $error -ne 0 ]; then
            echo "Error generating xdg-decoration-unstable-v1-protocol.c: $error"
            exit $error
        fi

        wayland-scanner client-header < /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml > xdg-decoration-unstable-v1.h
        error=$?
        if [ $error -ne 0 ]; then
            echo "Error generating xdg-decoration-unstable-v1.h: $error"
            exit $error
        fi

        cd ../../../
    fi
elif [ "$linux_platform" = "x11" ]; then
    LINKER_FLAGS="$LINKER_FLAGS -DDPLATFORM_LINUX_X11"
fi

mkdir -p "$BIN"

if [ -z "$ASSEMBLY" ]; then
    echo "[ERROR] Please name the executable. Variable ASSEMBLY is empty."
    exit 1
fi

# Gather all .c files recursively
SRC_FILES=$(find "$SRC" -type f -name '*.c')

echo "Building ${ASSEMBLY}${EXTENSION}..."
$CC $SRC_FILES $COMPILER_FLAGS -o "$BIN/$ASSEMBLY$EXTENSION" $INCLUDES $LINKER_FLAGS
