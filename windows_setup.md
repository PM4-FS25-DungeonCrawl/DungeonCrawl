# Setup WSL

Install Ubuntu via Microsoft Store

open Ubuntu

in the Terminal, run:

sudo apt install build-essential

sudo apt install manpages-dev

sudo apt install cmake

sudo apt install gdb

sudo apt install gcc

sudo apt install meson

sudo apt update

sudo apt upgrade

# Setup Notcurses

## Ubuntu

 sudo apt install libnotcurses-dev libnotcurses3

 if that doesn't work:

 apt-get install build-essential cmake doctest-dev libavdevice-dev libdeflate-dev libgpm-dev libncurses-dev libqrcodegen-dev libswscale-dev libunistring-dev pandoc pkg-config

## Windows

Install msys2 from https://www.msys2.org/

open msys2 mingw edition, probably under "C:\tools\msys64\mingw64.exe"

run:
pacman -S mingw-w64-x86_64-notcurses

pacman -S mingw-w64-x86_64-python mingw-w64-x86_64-meson

restart the shell.

The path conversion between windows and msys is:

Example project location:

C:\User\Username\ProjectRoot

The path in msys would be:

/c/User/Username/ProjectRoot

compile the project from within the msys2 mingw shell using normal mason commands.

## OSX

run brew install notcurses.

restart the shell.


# CLion configuration for WSL

in the CLion settings

under "File | Settings | Build, Execution, Deployment | Toolchains"

click on the +, click WSL


Toolset: Ubuntu

CMake: WSL CMake

Build Tool: leave empty

C Compiler: /bin/gcc

C++ Compiler: /bin/g++

Debugger: WSL GDB


click Apply

if there are already other Toolchains, move WSL to the top of the list


in the Run window at the bottom, under … | Modify run configuration

enable "Emulate terminal in the output console"


# Before running

navigate into project directory in WSL, then run:

mkdir buildDir

meson setup builddir



