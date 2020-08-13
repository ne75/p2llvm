# Propeller 2 LLVM Libraries
Contains Propeller 2 Library, C Standard Library, examples, and documentation for p2llvm

## Getting Started
(eventually I'll make a docker image that has all dependnecies set up so building is a simple one-step process)

0. Make sure you have the reuqired dependancies listed here https://llvm.org/docs/GettingStarted.html#software
1. Initialize the llvm-project submodule in this repo.
```
git submodule init
git submodule update
```
2. Create a build folder in llvm-project and change to it: 
```
mkdir build & cd build
```
3. Configure the project for building. This gives the basic configuration and creates Makefiles. See https://llvm.org/docs/GettingStarted.html for details on this command
``` 
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/opt/p2llvm -DLLVM_ENABLE_PROJECTS="lld;clang" -DCMAKE_BUILD_TYPE=Release -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=P2 -DLLVM_TARGETS_TO_BUILD="" ../llvm-project/llvm
```
Make `install dir` something like `/opt/p2llvm`

4. Once configured, run 
```
make -j8
``` 
(replace 8 with how many cores you want to use). Make a sandwich, initial build took around 30 minutes on my 4-core i7 Macbook.

5. Install, if desired 
```
make install
```

## Building Projects
Building C/C++ code with this compile suite works like any other. Compile each source file into a .o, link them all together with ld. Below are the specific details on this. Eventually, libp2, libc, and the linker script will all be automagically included by clang, that's on the todo list. Chances are you already have clang installed on your computer (expecially if on macOS), so be sure to set up you builds to use the above install directory and not your system clang.

1. Compile a source object. I use the following CFLAGS. The ciritcal ones are `--target=p2`, which tells our clang build what backend to use; `-fno-jump-tables` (required, see wiki in docs for why); and `-Os`, which will make code a lot more size and speed efficient. eventually, all necessary options will be automatically added by clang, including the library includes. 
```
CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2
<llvm binaries>/clang $(CFLAGS) -o hello.o -c hello.c
```
2. Link your objects into an executable ELF binary. 
```
LFLAGS = -Tp2.ld
<llvm binaries>/ld.lld $(LFLAGS) -o hello.elf hello.o
```
3. Load the resulting elf using loadp2
```
loadp2 -v -ZERO -b 230400 -t hello.elf
```

