@echo off
setlocal EnableDelayedExpansion

set SRC=src
set BIN=bin
set CC=clang
set ASSEMBLY=slug
set EXTENSION=.exe

set INCLUDES=
rem If raylib, uncomment the following line:
rem set LINKER_FLAGS=-L./external/raylib/lib/windows/ -lraylib -lkernel32 -lgdi32 -luser32 -ladvapi32 -ltdh -lwinmm -lm
set LINKER_FLAGS=

set COMPILER_FLAGS=-Wall -Werror -Wextra -g -O0 -Wno-system-headers -Wno-unused-but-set-variable -Wno-unused-variable -Wno-varargs -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-function -fsanitize=undefined

if not exist "%BIN%" mkdir "%BIN%"

if "%ASSEMBLY%"=="" (
    echo [ERROR] Please name the executable. Variable ASSEMBLY is empty.
    exit /b 1
)

rem Gather all .c files recursively
set SRC_FILES=
for /R "%SRC%" %%F in (*.c) do (
    set "SRC_FILES=!SRC_FILES! "%%F""
)

echo Building %ASSEMBLY%%EXTENSION%...
%CC% !SRC_FILES! %COMPILER_FLAGS% -o "%BIN%\%ASSEMBLY%%EXTENSION%" %INCLUDES% %LINKER_FLAGS%
