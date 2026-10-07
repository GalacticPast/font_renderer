@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0"

set SRC=src
set BIN=bin
set CC=clang
set ASSEMBLY=slug
set EXTENSION=.exe

set INCLUDES=-Ivendor/glad -Ivendor
rem If raylib, uncomment the following line:
rem set LINKER_FLAGS=-L./external/raylib/lib/windows/ -lraylib -lkernel32 -lgdi32 -luser32 -ladvapi32 -ltdh -lwinmm -lm
set DEFINES=-DDB_PLATFORM_WINDOWS
set LINKER_FLAGS=-lopengl32 -lgdi32 -luser32

rem Trap sanitizer failures without linking the incompatible Windows UBSan runtime.
set COMPILER_FLAGS=-Wall -Werror -Wextra -g -O0 -Wno-system-headers -Wno-unused-but-set-variable -Wno-unused-variable -Wno-varargs -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-function -fsanitize=undefined -fsanitize-trap=undefined

if not exist "%BIN%" mkdir "%BIN%"

if "%ASSEMBLY%"=="" (
    echo [ERROR] Please name the executable. Variable ASSEMBLY is empty.
    exit /b 1
)

rem 
set "SRC_FILES="

for /r "%SRC%" %%F in (*.c) do (
    set "SRC_FILES=!SRC_FILES! "%%F""
)

set "SRC_FILES=!SRC_FILES! "vendor\glad\glad.c""

echo Building %ASSEMBLY%%EXTENSION%...
%CC% !SRC_FILES! %COMPILER_FLAGS% -o "%BIN%\%ASSEMBLY%%EXTENSION%" %INCLUDES% %LINKER_FLAGS%
exit /b %errorlevel%
