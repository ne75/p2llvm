import subprocess
from colorama import Fore
import tempfile
import os
from pathlib import Path
import re
from pprint import pprint

load_cmd = '/opt/p2llvm/bin/loadp2'
load_args = ['-ZERO', '-l', '0', '-v', '-FIFO', '4096']

def load(port, app, baud, verbose=False, retries=3):
        
    result = False

    while retries:
        args = load_args.copy()
        args.append('-p')
        args.append(port)
        args[2] = str(baud)

        args.append('{}'.format(app))

        p = subprocess.Popen([load_cmd] + args, stdout=subprocess.PIPE)

        if verbose:
            print(" ".join([load_cmd] + args))
        
        output = p.communicate()[0].decode('ascii')

        if p.returncode != 0:
            print(Fore.RED + output + Fore.RESET)
            print(Fore.YELLOW + "Load failed, retrying...\r" + Fore.RESET)
            retries -= 1;
            result = False;
        else:
            result = True;
            break

    if not result:
        print(Fore.RED + "Failed to load app\r" + Fore.RESET)
        if verbose:
            print("Load command was: " + str([load_cmd] + args))

    return result

def gen_bin(elf_file, outdir=None):
    '''
    generate a binary file for loading from the given elf file.
    if outdir is given, save the binary there. otherwise it will be placed in 
    a temporary directory
    
    returns the path to the generated binary
    '''
    if not outdir:
        outdir = tempfile.gettempdir()

    outfile = os.path.join(outdir, Path(elf_file).stem)

    p = subprocess.Popen([
        '/opt/p2llvm/bin/llvm-objcopy',
        '-O',
        'binary',
        elf_file,
        outfile
    ], stdout=subprocess.PIPE)

    p.wait()

    if (p.returncode != 0):
        return None

    return outfile
    

def get_objdump_data(elf_file):
    '''
    return a dict of sections in the elf file. each section is keyed with the section header (as given by objdump), and the value is a 
    dict containing a cleaned up version of the address, instruction encoding, and parsed instruction 

    example:
    {
        ".cog<__start>": {
            "section_addr": 0
            "0": ["f6 03 a1 f8", "mov r0, ptra"]
        }
    }

    '''

    p = subprocess.Popen([
        '/opt/p2llvm/bin/llvm-objdump',
        '-d',
        elf_file,
    ], stdout=subprocess.PIPE)

    out, _ = p.communicate()

    lines = out.decode('ascii').splitlines()

    current_section = ''
    current_subsection = ''
    current_section_key = ''
    current_section_addr = 0

    SECTION_PATERN = r"^Disassembly of section (.*?):$"
    SUBSECTION_PATTERN = r"^([a-f0-9]+) <(.*?)>:$"
    INST_PATTERN = r"^\s+([a-f0-9]+):\s+(([a-f0-9]+\s)+)\s+(.*?)$"

    output_dict = {}

    for l in lines:
        if re.search(SECTION_PATERN, l): # check for the start of a code section
            current_section = re.search(SECTION_PATERN, l).group(1)

        elif re.search(SUBSECTION_PATTERN, l): # check for the start of a function section
            current_section_addr = int(re.search(SUBSECTION_PATTERN, l).group(1), 16)
            current_subsection = re.search(SUBSECTION_PATTERN, l).group(2)

        elif re.search(INST_PATTERN, l): # check for an individual instruction 
            inst_match = re.search(INST_PATTERN, l)
            current_section_key = "{}<{}>".format(current_section, current_subsection)

            if current_section_key not in output_dict:
                output_dict[current_section_key] = {}
                output_dict[current_section_key]["section_addr"] = current_section_addr

            inst_encoding = inst_match.group(2).split(' ')
            inst_encoding = [s for s in inst_encoding if s]
            inst_encoding = inst_encoding[::-1]

            output_dict[current_section_key][int(inst_match.group(1), 16)] = [' '.join(inst_encoding), inst_match.group(4).strip()]

    return output_dict