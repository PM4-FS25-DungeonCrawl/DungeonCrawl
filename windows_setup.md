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
