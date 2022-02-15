import subprocess
from colorama import Fore
import tempfile
import os
from pathlib import Path
import re

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
            0: ["f6 03 a1 f8", "mov r0, ptra"]
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

            i_str_fragments = [s.strip() for s in inst_match.group(4).split('\t') if s]

            if len(i_str_fragments) == 1:
                i_str_fragments = ['', i_str_fragments[0], '']
            elif len(i_str_fragments) == 2:
                if i_str_fragments[-1] == 'wc' or i_str_fragments[-1] == 'wz' or i_str_fragments[-1] == 'wcz':
                    # we don't have a condition, add a blank to the front
                    i_str_fragments.insert(0, '')
                else:
                    # we don't have a effect, add a blank to the back
                    i_str_fragments.append('')

            elif len(i_str_fragments) == 3:
                pass # don't need to do anything
            else:
                print("Unknown instruction format: " + str(i_str_fragments))

            i_str = '{: >14} {: <18}{}'.format(i_str_fragments[0], i_str_fragments[1], i_str_fragments[2])

            output_dict[current_section_key][int(inst_match.group(1), 16)] = (
                ' '.join(inst_encoding), 
                i_str
            )

    return output_dict

def get_inst(data, pc):
    '''
    get the instruction at the given pc in data

    if no such instruction exists, return None
    '''

    for s in data:
        for i in data[s]:
            if isinstance(i, int) and i == pc:
                return data[s][i]

    return None

def get_section(data, addr) -> str:
    '''
    get the section name for a given address
    '''

    for s in data:
        if data[s]["section_addr"] == addr:
            return s

    return ""