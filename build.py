import argparse
import os
import subprocess

DEBUG_BUILD_DIR = "llvm-project/build_debug"
RELEASE_BUILD_DIR = "llvm-project/build_release"
LIBP2_DIR = "libp2"
LIBC_DIR = "libc"

build_dir = ""

def build_llvm(configure=True, debug=False, install_dest=None):
    '''
    if install_dest is None, then don't run install
    '''

    # create the build directory
    os.makedirs(build_dir, exist_ok=True)

    # setup cmake command
    cmake_cmd = [   'cmake', 
                    '-G', 
                    'Unix Makefiles',
                    '-DLLVM_ENABLE_PROJECTS=lld;clang',
                    '-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=P2',
                    '-DLLVM_TARGETS_TO_BUILD='
                    ]

    if debug:
        cmake_cmd.append("-DCMAKE_BUILD_TYPE=" + "Debug")
    else:
        cmake_cmd.append("-DCMAKE_BUILD_TYPE=" + "Release")

    if install_dest:
        cmake_cmd.append("-DCMAKE_INSTALL_PREFIX=" + install_dest)

    cmake_cmd.append("../llvm")

    # run cmake
    if (configure):
        p = subprocess.Popen(cmake_cmd, cwd=build_dir)
        p.wait()

        if p.returncode != 0:
            return False

    # build LLVM, optionally installing it
    if (install_dest):
        p = subprocess.Popen(['make', 'install', '-j8'], cwd=build_dir)
    else:
        p = subprocess.Popen(['make', '-j8'], cwd=build_dir)

    p.wait()

    if p.returncode != 0:
        return False

    # install the linker script to either the install destination or the build directory
    if (install_dest):
        p = subprocess.Popen(['cp', '../../libp2/p2.ld', install_dest], cwd=build_dir)
    else:
        p = subprocess.Popen(['cp', '../../libp2/p2.ld', '.'], cwd=build_dir)

    p.wait()

    if p.returncode != 0:
        return False

    return True

def build_libp2(install_dest, llvm, clean=False, configure=True):
    build_dir = os.path.join(LIBP2_DIR, 'build')
    os.makedirs(build_dir, exist_ok=True)

    # build libp2

    if configure:
        cmake_cmd = ['cmake', '../']

        p = subprocess.Popen(cmake_cmd, cwd=build_dir)
        p.wait()

        if p.returncode != 0:
            return False
        

    if clean:
        p = subprocess.Popen(['make', 'clean'], cwd=build_dir)
        p.wait()
        if p.returncode != 0:
            return False

    p = subprocess.Popen(['make', 'LLVM=' + llvm], cwd=build_dir)
    p.wait()
    if p.returncode != 0:
        return False

    # install libp2
    install_dir = os.path.join(install_dest, "libp2")
    os.makedirs(os.path.join(install_dir, 'lib'), exist_ok=True)

    p = subprocess.Popen(['cp', os.path.join(build_dir, 'lib', 'libp2.a'), os.path.join(install_dir, 'lib', 'libp2.a')])
    p.wait()
    if p.returncode != 0:
        return False

    p = subprocess.Popen(['cp', os.path.join(build_dir, 'lib', 'p2db', 'libp2db.a'), os.path.join(install_dir, 'lib', 'libp2db.a')])
    p.wait()
    if p.returncode != 0:
        return False

    p = subprocess.Popen(['cp', '-r', os.path.join(LIBP2_DIR, 'include'), install_dir])
    p.wait()
    if p.returncode != 0:
        return False

    return True

def build_libc(install_dest, llvm, clean=False):
    # build and install libc
    install_dir = os.path.join('..', install_dest, 'libc')

    if clean:
        p = subprocess.Popen(['make', 'clean'], cwd=LIBC_DIR)
        p.wait()
        if p.returncode != 0:
            return False

    p = subprocess.Popen(['make', 'LLVM=' + llvm, 'install', 'DEST=' + str(install_dir)], cwd=LIBC_DIR)
    p.wait()
    if p.returncode != 0:
        return False

    return True

def main():
    global build_dir

    parser = argparse.ArgumentParser(description='P2 LLVM Build Script')
    parser.add_argument('--configure', nargs='?', const=True, default=False)
    parser.add_argument('--skip_llvm', nargs='?', const=True, default=False)
    parser.add_argument('--skip_libc', nargs='?', const=True, default=False)
    parser.add_argument('--skip_libp2', nargs='?', const=True, default=False)
    parser.add_argument('--clean', nargs='?', const=True, default=False)
    parser.add_argument('--debug', nargs='?', const=True, default=False)
    parser.add_argument('--install', type=str, required=False)
    

    args = parser.parse_args()

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
        if not build_llvm(configure, debug, install_dest):
            return

    # build libp2
    if not skip_libp2:
        if (install_dest):
            r = build_libp2(install_dest, llvm, clean, configure)
        else:
            r = build_libp2(build_dir, llvm, clean, configure)

        if not r:
            return

    # build libc
    if not skip_libc:
        if (install_dest):
            r = build_libc(install_dest, llvm, clean)
        else:
            r = build_libc(build_dir, llvm, clean)

        if not r:
            return


if __name__ == "__main__":
    main();