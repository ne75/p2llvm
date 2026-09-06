import os
import shlex
import sys

import lit.formats

config.name = 'P2 integration'
config.test_format = lit.formats.ShTest(execute_external=False)
config.suffixes = ['.c', '.cpp', '.ll', '.s', '.test']
config.excludes = ['tools', 'hardware', 'README.md', 'legacy-coverage.md']
config.test_source_root = os.path.dirname(__file__)
root = os.path.dirname(config.test_source_root)
build = os.path.abspath(lit_config.params.get('p2_build', os.path.join(root, 'build', 'phase0-llvm')))
config.test_exec_root = os.path.join(build, 'p2-tests')
bindir = os.path.join(build, 'bin')
config.environment['PATH'] = bindir + os.pathsep + os.environ.get('PATH', '')
config.substitutions.extend([
    ('%python', shlex.quote(sys.executable)),
    ('%tools', shlex.quote(os.path.join(config.test_source_root, 'tools'))),
    ('%root', shlex.quote(root)),
    ('%bin', shlex.quote(bindir)),
    ('%clang', shlex.quote(os.path.join(bindir, 'clang')) + ' --target=p2'),
    ('%llc', shlex.quote(os.path.join(bindir, 'llc')) + ' -march=p2 -verify-machineinstrs'),
    ('%mc', shlex.quote(os.path.join(bindir, 'llvm-mc')) + ' -triple=p2'),
])
