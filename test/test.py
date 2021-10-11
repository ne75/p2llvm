import argparse
import yaml
import os
import signal
import subprocess
from colorama import Fore
import re
import serial
import time

# TODO: make directorys more robust 
no_opt_build_dir = 'build/no_opt'
opt_build_dir = 'build/opt'

load_cmd = '/opt/p2llvm/bin/loadp2'
load_args = ['-ZERO', '-l', '230400, ''-SINGLE', '-FIFO', '4096', '-v']

START_OF_TEST = '$'
END_OF_TEST = '~'

class TestCase:
    def __init__(self, name):
        self.name = name
        self.ref_file = os.path.join('ref', name + '.yaml')

        self.ref = yaml.load(open(self.ref_file, 'r'))
        self.expected_output = None if self.ref['expected_output'] == '' else self.ref['expected_output']

        self.compile_success = False

    def compile_case(self):
        p = subprocess.Popen(['make', self.name], cwd=os.path.join(no_opt_build_dir))
        p.wait()

        if (p.returncode != 0):
            print(Fore.RED + self.name + ": Failed to compile non-optimized test" + Fore.RESET)
            return False

        p = subprocess.Popen(['make', self.name], cwd=os.path.join(opt_build_dir))
        p.wait()

        if (p.returncode != 0):
            print(Fore.RED + self.name + ": Failed to compile non-optimized test" + Fore.RESET)
            return False

        self.compile_success = True
        return True

    def run(self, port):
        if (not self.compile_success):
            return False

        ser = serial.Serial()
        ser.baudrate = 230400
        ser.port = port
        ser.dtr = False
        ser.open()

        print(Fore.GREEN + self.name + ": Running non-optimized test" + Fore.RESET)
        if not self.load(port, os.path.join(no_opt_build_dir, 'src', self.name)):
            return False

        non_opt_res = True
        opt_res = True

        if (self.expected_output):
            non_opt_res = self.compare(self.get_output(ser))
            if non_opt_res:
                print(Fore.GREEN + self.name + ": Non-optimized test passed" + Fore.RESET)
            else:
                print(Fore.RED + self.name + ": Non-optimized test failed" + Fore.RESET)
                return False

        print(Fore.GREEN + self.name + ": Running optimized test" + Fore.RESET)
        if not self.load(port, os.path.join(opt_build_dir, 'src', self.name)):
            return False

        if (self.expected_output):
            opt_res = self.compare(self.get_output(ser))
            if non_opt_res:
                print(Fore.GREEN + self.name + ": Non-optimized test passed" + Fore.RESET)
            else:
                print(Fore.RED + self.name + ": Non-optimized test failed" + Fore.RESET)
                return False

        return True

    def load(self, port, app):
        args_base = load_args
        args_base.append('-p')
        args_base.append(port)

        args = args_base + [app]
        p = subprocess.Popen([load_cmd] + args, stdout=subprocess.PIPE)
        
        output = p.communicate()[0].decode('ascii')

        if p.returncode != 0:
            print(Fore.RED + "Failed to load test case\r" + Fore.RESET)
            print(Fore.RED + output + Fore.RESET)
            print("Load command was: " + str([load_cmd] + args))
            return False

        return True

    def get_output(self, ser):
        done = False
        results = ''
        while not done:
            l = ser.readline().decode('ascii').strip() + '\n'
            results += l

            if (l == '~\n'):
                done = True

        return results.strip()

    def compare(self, output):
        r = re.search(r'\$((.|\n)*?)\~', output).group(0)[1:-1].strip() # validate our output

        if r and r == self.expected_output:
            return True
        else:
            print("Expected:\n" + self.expected_output)
            print("Got:\n" + r)

        return False

def prepare():
    '''
    prepare the test cases for compilation. 

    returns the available test cases
    '''
    os.makedirs(no_opt_build_dir, exist_ok=True)
    os.makedirs(opt_build_dir, exist_ok=True)

    print(Fore.GREEN + "=== Generating non-optimized test cases ===" + Fore.RESET)
    p = subprocess.Popen(['cmake', '../../'], cwd=no_opt_build_dir)
    p.wait()

    if p.returncode != 0:
        print(Fore.RED + "CMake configuration failed for non-optimized cases" + Fore.RESET)

    print(Fore.GREEN + "=== Generating optimized test cases ===" + Fore.RESET)
    p = subprocess.Popen(['cmake', '../../'], cwd=opt_build_dir)
    p.wait()

    if p.returncode != 0:
        print(Fore.RED + "CMake configuration failed for optimized cases" + Fore.RESET)

    p = subprocess.Popen(['cmake', '--build', '.', '--target', 'help'], cwd=no_opt_build_dir, stdout=subprocess.PIPE) # both lists will be the same
    stdout, _ = p.communicate();

    tests = []
    for m in re.findall(r'\.\.\. t\d+', stdout.decode('ascii')):
        tests.append(re.search(r't\d+', m).group(0))

    return tests

def natural_sort(l): 
    convert = lambda text: int(text) if text.isdigit() else text.lower()
    alphanum_key = lambda key: [convert(c) for c in re.split('([0-9]+)', key)]
    return sorted(l, key=alphanum_key)

def main():
    parser = argparse.ArgumentParser(description='P2 LLVM Tests')
    parser.add_argument('--port', type=str, required=True)
    parser.add_argument('--tests', type=str, nargs='+')

    args = parser.parse_args()
    port = args.port
    test_names = natural_sort(prepare())

    # assemble the list of tests to run
    tests_to_run = []

    if (args.tests):
        for t in args.tests:
            if t in test_names:
                tests_to_run.append(TestCase(t))
    else:
        for t in test_names:
            tests_to_run.append(TestCase(t))

    for t in tests_to_run:
        t.compile_case()

    for t in tests_to_run:
        if (t.run(port)):
            print(Fore.LIGHTGREEN_EX + t.name + ": PASS!" + Fore.RESET)
        else:
            print(Fore.LIGHTRED_EX + t.name + ": FAIL!" + Fore.RESET)

if __name__ == "__main__":
    main()