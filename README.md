# Propeller 2 LLVM Libraries
Contains Propeller 2 Library, C Standard Library, examples, and documentation for p2llvm

# Getting Started
(eventually I'll make a docker image that has all dependnecies set up so building is a simple one step process)

1. Initialize the llvm submodule in this repo.
1. Create a build folder in p2llvm: ```mkdir build & cd build```
1. Configure the project for building. This gives the basic configuration and creates Makefiles. See (insert link) for details on this command
``` cmake -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="lld;clang" -DCMAKE_INSTALL_PREFIX=<install dir> -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=P2 -DLLVM_TARGETS_TO_BUILD="" ../llvm ```
Make `install dir` something like `/opt/p2llvm`
1. One configured, run ```make -j4``` (replace 4 with how many cores you want to use). Make a sandwich, initial build can take around 30 minutes or so on a 4-core machine
1. Run ```make install```, if desired.
