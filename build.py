import argparse
import logging
import multiprocessing
import os
import subprocess
import sys

DEBUG_BUILD_DIR = "llvm-project/build_debug"
RELEASE_BUILD_DIR = "llvm-project/build_release"
LIBP2_DIR = "libp2"
LIBP2PP_DIR = "libp2++"
LIBC_DIR = "libc"

CPU_COUNT = multiprocessing.cpu_count()

build_dir = ""

def logged_run(*a, **kw):
    logging.debug(f"RUNNING: {a}, {kw}")
    if 'cwd' in kw and kw['cwd'] is not None:
        logging.debug(f'CWD: {kw["cwd"]}')
    if len(a) == 1 and isinstance(a[0], list):
        logging.debug(f'CMD: {" ".join(a[0])}')
    return subprocess.run(*a, **kw)


def copy(src, dst, cwd=None, recurse=False):
    cmd = ['cp']
    if (recurse):
        cmd += ['-r']

    cmd += [src, dst]

    logged_run(cmd, cwd=cwd, check=True)

    return True

def build_llvm(args):
    # create the build directory
    os.makedirs(build_dir, exist_ok=True)

    # setup cmake command
    cmake_cmd = [
        'cmake',
        '-G',
        'Unix Makefiles',
        '-DCMAKE_OSX_ARCHITECTURES=arm64',
        '-DLLVM_INSTALL_UTILS=true'
        '-DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++',
        '-DLLVM_ENABLE_PROJECTS=lld;clang',
        '-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=P2',
        '-DLLVM_TARGETS_TO_BUILD=',
        f'-DCMAKE_BUILD_TYPE={"Debug" if args.debug else "Release"}',
        ]

    if args.install:
        cmake_cmd.append(f"-DCMAKE_INSTALL_PREFIX={args.install}")

    cmake_cmd.append("../llvm")

    # run cmake
    if args.configure:
        logged_run(cmake_cmd, cwd=build_dir, check=True)

    # build LLVM, optionally installing it
    if args.install:
        logged_run(['make', 'install', f'-j{CPU_COUNT}'], cwd=build_dir, check=True)
    else:
        logged_run(['make', f'-j{CPU_COUNT}'], cwd=build_dir, check=True)

    # install the linker script to either the install destination or the build directory
    if (install_dest):
        if not copy('../../libp2/p2.ld', install_dest, cwd=build_dir):
            return False
        if not copy('../../libp2/p2_debug.ld', install_dest, cwd=build_dir):
            return False
    else:
        if not copy('../../libp2/p2.ld', '.', cwd=build_dir):
            return False
        if not copy('../../libp2/p2_debug.ld', '.', cwd=build_dir):
            return False

    return True

def build_libp2(install_dest, llvm, clean=False, configure=True):
    build_dir = os.path.join(LIBP2_DIR, 'build')
    os.makedirs(build_dir, exist_ok=True)

    # build libp2

    if configure:
        cmake_cmd = ['cmake', '-Dllvm=' + str(os.path.join(install_dest, 'bin')), '../']

        logged_run(cmake_cmd, cwd=build_dir, check=True)

    if clean:
        logged_run(['make', 'clean'], cwd=build_dir, check=True)

    llvm_cmd = ['make', f'LLVM={llvm}', f'-j{CPU_COUNT}']
    logged_run(llvm_cmd, cwd=build_dir, check=True)

    # install libp2
    install_dir = os.path.join(install_dest, "libp2")
    os.makedirs(os.path.join(install_dir, 'lib'), exist_ok=True)

    if not copy(os.path.join(build_dir, 'lib', 'libp2.a'), os.path.join(install_dir, 'lib', 'libp2.a')):
        return False

    if not copy(os.path.join(build_dir, 'lib', 'p2db', 'libp2db.a'), os.path.join(install_dir, 'lib', 'libp2db.a')):
        return False

    if not copy(os.path.join(LIBP2_DIR, 'include'), install_dir, recurse=True):
        return False

    return True

def build_libp2pp(install_dest, llvm, clean=False, configure=True):
    build_dir = os.path.join(LIBP2PP_DIR, 'build')
    os.makedirs(build_dir, exist_ok=True)

    # build libp2++
    if configure:
        cmake_cmd = ['cmake', '-Dllvm=' + str(os.path.join(install_dest, 'bin')), '../']
        logged_run(cmake_cmd, cwd=build_dir, check=True)

    if clean:
        logged_run(['make', 'clean'], cwd=build_dir, check=True)

    logged_run(['make', f'llvm={llvm}', f'-j{CPU_COUNT}'], cwd=build_dir, check=True)

    # install libp2
    install_dir = os.path.join(install_dest, "libp2")
    os.makedirs(os.path.join(install_dir, 'lib'), exist_ok=True)

    # if not copy(os.path.join(build_dir, 'lib', 'libp2++.a'), os.path.join(install_dir, 'lib', 'libp2++.a')):
    #     return False

    if not copy(os.path.join(LIBP2PP_DIR, 'include'), install_dir, recurse=True):
        return False

    return True

def build_libc(install_dest, llvm, clean=False, configure=True):
    build_dir = os.path.join(LIBC_DIR, 'build')
    os.makedirs(build_dir, exist_ok=True)

    # build libc
    if configure:
        cmake_cmd = ['cmake', '-Dllvm=' + str(os.path.join(install_dest, 'bin')), '../']

        logged_run(cmake_cmd, cwd=build_dir, check=True)

    if clean:
        logged_run(['make', 'clean'], cwd=build_dir, check=True)

    logged_run(['make', f'LLVM={llvm}', f'-j{CPU_COUNT}'], cwd=build_dir, check=True)
    # install libc
    install_dir = os.path.join(install_dest, "libc")
    os.makedirs(os.path.join(install_dir, 'lib'), exist_ok=True)

    if not copy(os.path.join(build_dir, 'libc.a'), os.path.join(install_dir, 'lib', 'libc.a')):
        return False

    if not copy(os.path.join(LIBC_DIR, 'include'), install_dir, recurse=True):
        return False

    return True


def parse_args():
    parser = argparse.ArgumentParser(description='P2 LLVM Build Script')
    parser.add_argument('--configure', nargs='?', const=True, default=False)
    parser.add_argument('--skip_llvm', nargs='?', const=True, default=False)
    parser.add_argument('--skip_libc', nargs='?', const=True, default=False)
    parser.add_argument('--skip_libp2', nargs='?', const=True, default=False)
    parser.add_argument('--clean', nargs='?', const=True, default=False)
    parser.add_argument('--debug', nargs='?', const=True, default=False)
    parser.add_argument('--install', type=str, required=False)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()


def main():
    global build_dir

    args = parse_args()
    logging.basicConfig(
        stream=sys.stderr,
        level=logging.DEBUG if args.verbose else logging.INFO
    )
    configure = args.configure
    skip_llvm = args.skip_llvm
    skip_libc = args.skip_libc
    skip_libp2 = args.skip_libp2
    install_dest = args.install
    debug = args.debug
    clean = args.clean

    # set up build directory
    if (debug):
        build_dir = DEBUG_BUILD_DIR
    else:
        build_dir = RELEASE_BUILD_DIR

    if install_dest:
        llvm = os.path.abspath(install_dest)
    else:
        llvm = os.path.abspath(build_dir)

    # build LLVM
    if not skip_llvm:
        if not build_llvm(args):
            return

    # build libp2
    if not skip_libp2:
        if (install_dest):
            r = build_libp2(install_dest, llvm, clean, configure)
            r = build_libp2pp(install_dest, llvm, clean, configure)
        else:
            r = build_libp2(build_dir, llvm, clean, configure)
            r = build_libp2pp(build_dir, llvm, clean, configure)

        if not r:
            return

    # build libc
    if not skip_libc:
        if (install_dest):
            build_libc(install_dest, llvm, clean, configure)
        else:
            build_libc(build_dir, llvm, clean, configure)



if __name__ == "__main__":
    main();
