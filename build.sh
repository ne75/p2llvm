#!/bin/bash

#
# display help
#
help()
{
   # Display Help
   echo "Build and optionally install P2LLVM system: toolchain, libc, and libp2"
   echo
   echo "Syntax: build.sh [dir] [options]"
   echo "dir:           Folder to install to, optionally"
   echo "-h:            display this message"
   echo
   echo "options:"
   echo "--skip_llvm:    skip LLVM toolchain build"
   echo "--skip_libc:    skip C standard library build"
   echo "--skip_libp2:   skip P2 standard library build"
}

build_llvm=true
build_libc=true
build_libp2=true

# get options
install_dir_base=$1

while [ -n "$2" ]; do # while loop starts
    case "$2" in
        -h) # display Help
            help
            exit;;
        --skip_llvm)
            echo "Skip LLVM"
            build_llvm=false;;
        --skip_libc)
            echo "Skip libc"
            build_libc=false;;
        --skip_libp2)
            echo "Skip libp2"
            build_libp2=false;;
    esac
    shift
done

set -e

#
# build and optionally install llvm toolchain
#
if $build_llvm; then
    echo
    echo "========================"
    echo "==== Building llvm ====="
    echo "========================"
    cd llvm-project
    mkdir -p build_release
    cd build_release

    if [ $# -ne 0 ]; then
        cmake -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="lld;clang" -DCMAKE_INSTALL_PREFIX=$install_dir_base -DCMAKE_BUILD_TYPE=Release -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=P2 -DLLVM_TARGETS_TO_BUILD="" ../llvm

        make install -j8
        cp ../../libp2/p2.ld $install_dir_base/
    else
        cmake -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="lld;clang" -DCMAKE_BUILD_TYPE=Release -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=P2 -DLLVM_TARGETS_TO_BUILD="" ../llvm

        make -j8
    fi
    cd ../..
fi

#
# build and optionally install libc
#
if $build_libc; then
    echo
    echo "========================"
    echo "==== Building libc ====="
    echo "========================"
    cd libc
    if [ $# -ne 0 ]; then
        make clean
        make -j8 install DEST=$install_dir_base/libc
    else
        make clean
        make -j8
    fi
    cd ..
fi

#
# build libp2
#

if $build_libp2; then
    echo
    echo "========================="
    echo "==== Building libp2 ====="
    echo "========================="
    cd libp2
    make clean
    make -j8
    cd ..

    #
    # optionally install libp2
    #
    if [ $# -ne 0 ]; then
        echo "Installing to" $install_dir_base "..."
        install_dir=$install_dir_base/libp2
        mkdir -p $install_dir/lib
        cp libp2/build/libp2.a $install_dir/lib/libp2.a
        cp -r libp2/include $install_dir/
    fi
fi

echo "Done"
