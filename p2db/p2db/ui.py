from prompt_toolkit import Application
from prompt_toolkit.layout.containers import VSplit, HSplit, Window
from prompt_toolkit.layout.layout import Layout
from prompt_toolkit.layout import FormattedTextControl, WindowAlign
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.widgets import Frame, TextArea, Box
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.data_structures import Point
from prompt_toolkit.formatted_text import ANSI
from prompt_toolkit.layout.screen import Char

from colorama import Fore, Style
import threading
import logging
import time
import re

from . import p2tools
from . import p2db_server

log = logging.getLogger('main')
Char.display_mappings['\t'] = '\t'

class UI:
    kb = KeyBindings()

    help_text = '''
    p2db
    ----

        help                : Print this dialog
        step [Ctrl+S]       : Step by one instruction. Call instructions are stepped over. Modifier instructions (augd/s, setq) will be skipped.
        stepin [Ctrl+T]     : Step into a function call
        stepout [Ctrl+O]    : Step out of the current function call
        break <addr>        : Set breakpoint at 'addr' and continue. 'addr' should be in hex
        getreg <reg>        : Get the value in 'reg'. 'reg' can be an address or register name. Address should be in hex
        getbyte <addr>      : Get the byte at hub address 'addr'. Address should be in hex
        getlong <addr>      : Get the long at hub address 'addr'. Address should be in hex
        pins                : Update pin status data
        cog <n>             : Set the active cog to n
        cogaddr <addr>      : Set the cog execution address (for native cogs)
        continue            : (unimplemented) Continue execution. Cog will be disconnected until it interrupts itself
        reset               : (unimplemented) Reload the current program
        quit [Ctrl+Q]       : Quit
    '''

    pc_cursor_string = Fore.CYAN + "     ---> " + Fore.RESET
    pc_cursor_size = 10

    instance = None
    
    def __init__(self, server: p2db_server.P2DBServer, objdata):
        assert(not UI.instance)

        UI.instance = self

        self.server = server
        self.obj_data = objdata
        self.app = Application(full_screen=True)
        self.current_func = ''
        self.dirty = True
        self.render_lock = threading.Lock()

        # dict of commands and handler function for each
        self.commands = {
            "step": self.on_step,
            "stepin": self.on_stepin,
            "stepout": self.on_stepout,
            "break": self.on_break,
            "getreg": self.on_getreg,
            "getbyte": self.on_getbyte,
            "getlong": self.on_getlong,
            "continue": self.on_continue,
            "pins": self.on_pins,
            "cog": self.on_cog,
            "cogaddr": self.on_cogaddr,
            "reset": self.on_reset,
            "quit": self.on_quit,
            "help": self.on_help
        }

        # log stuff
        def log_cursor_pos():
            y = self.log.text.value.count('\n')
            return Point(0, y)

        self.log = FormattedTextControl(ANSI(""), get_cursor_position=log_cursor_pos)
        self.log_area = Window(self.log)

        # prompt stuff
        cmd_completer = WordCompleter(list(self.commands.keys()))
        self.prompt = TextArea(
            height=1,
            prompt="p2db > ",
            multiline=False,
            wrap_lines=False,
            complete_while_typing=True,
            completer=cmd_completer,
            accept_handler = self.accept,
            focus_on_click=True,
        )

        # status window stuff
        self.status = FormattedTextControl(ANSI(''))
        self.connection = FormattedTextControl(ANSI(''))
        self.pins = FormattedTextControl(ANSI(''))
        status_split = HSplit([
            VSplit([
                Window(self.status),
                Box(Window(self.connection, align=WindowAlign.RIGHT), 3, padding_top=0)
            ]),
            Frame(Box(Window(self.pins, width=95, height=5), padding=3, padding_bottom=0, padding_top=1), "Pins")
        ])

        # instruction window stuff
        def inst_cursor_pos():
            y = max(0, min(self.pc_line, self.instructions.text.value.count('\n')))
            return Point(0, y)

        self.pc_line = 0
        self.instructions = FormattedTextControl(ANSI(''), focusable=True, get_cursor_position=inst_cursor_pos)
        self.function_header = FormattedTextControl(ANSI(''))
        instruction_split = HSplit([
            Box(Window(self.function_header, height=1), 1, padding_top=0),
            Box(Window(self.instructions, height=40), 1)
        ])

        # Frames for each section
        self.cog_status_window = Frame(Box(status_split, 1), "Status")
        self.instruction_window = Frame(Box(instruction_split, 1), "Source")
        self.log_window = Frame(Box(self.log_area, padding=1, padding_bottom=0))
        self.prompt_window = Frame(self.prompt)

        body = VSplit([
            self.cog_status_window,
            self.instruction_window,
        ])

        root_container = HSplit([
            body,
            self.log_window,
            self.prompt_window
        ])

        layout = Layout(root_container, self.prompt)

        self.app = Application(layout=layout, key_bindings=self.kb, full_screen=True, before_render=self.prerender, after_render=self.postrender)
        self.app.layout.focus(self.prompt_window)

    @kb.add('c-c')
    @kb.add('c-q')
    def exit_(event):
        event.app.exit()

    def on_help(self, args):
        ui_instance = UI.instance

        ui_instance.update_log(ui_instance.help_text + "\n")

    @kb.add('c-s')
    def on_step(self, args=[]):
        ui_instance = UI.instance

        r = ui_instance.server.step()

        if r:
            ui_instance.update_log(r + "\n", Fore.RED)

    @kb.add('c-t')
    def on_stepin(self, args=[]):
        ui_instance = UI.instance

        r = ui_instance.server.stepin()

        if r:
            ui_instance.update_log(r + "\n", Fore.RED)

    @kb.add('c-o')
    def on_stepout(self, args=[]):
        ui_instance = UI.instance

        r = ui_instance.server.stepout()

        if r:
            ui_instance.update_log(r + "\n", Fore.RED)

    def on_break(self, args):
        ui_instance = UI.instance

        r = ui_instance.server.breakpoint(args[0])

        if r:
            ui_instance.update_log(r + "\n", Fore.RED)

    def on_getreg(self, args):
        ui_instance = UI.instance

        if len(args) != 1:
            ui_instance.update_log("Error: expected 1 argument\n", Fore.RED)
            return

        r = ui_instance.server.get_reg(args[0])

        if (r[0]):
            ui_instance.update_log(r[0], Fore.RED)
        else:
            try:
                addr = int(args[0], 16)
                ui_instance.update_log("reg {:#02x} -> {:#02x}".format(addr, r[1]) + "\n")
            except ValueError:
                ui_instance.update_log("{} -> {:#02x}".format(args[0], r[1]) + "\n")

    def on_getbyte(self, args):
        ui_instance = UI.instance

        if len(args) != 1:
            ui_instance.update_log("Error: expected 1 argument\n", Fore.RED)
            return

        r = ui_instance.server.get_byte(args[0])

        if (r[0]):
            ui_instance.update_log(r[0] + "\n", Fore.RED)
        else:
            ui_instance.update_log("byte @ {:#02x} -> {:#02x}".format(int(args[0], 16), r[1]) + "\n")

    def on_getlong(self, args):
        ui_instance = UI.instance

        if len(args) != 1:
            ui_instance.update_log("Error: expected 1 argument\n", Fore.RED)
            return

        r = ui_instance.server.get_long(args[0])

        if (r[0]):
            ui_instance.update_log(r[0] + "\n", Fore.RED)
        else:
            ui_instance.update_log("long @ {:#02x} -> {:#02x}".format(int(args[0], 16), r[1]) + "\n")

    def on_cog(self, args):
        ui_instance = UI.instance

        if len(args) != 1:
            ui_instance.update_log("Error: expected 1 argument\n", Fore.RED)
            return

        try:
            cog_num = int(args[0])
        except ValueError:
            ui_instance.update_log("Error: expected numeric argument\n", Fore.RED)
            return

        ui_instance.server.set_cog(cog_num)

    def on_cogaddr(self, args):
        ui_instance = UI.instance

        if len(args) != 1:
            ui_instance.update_log("Error: expected 1 argument\n", Fore.RED)
            return

        try:
            addr = int(args[0], 16)
        except ValueError:
            ui_instance.update_log("Error: expected numeric argument\n", Fore.RED)
            return

        ui_instance.server.cog_states[ui_instance.server.current_cog].status.set_cog_addr(addr)


    @kb.add('c-p')
    def on_pins(self, args=[]):
        ui_instance = UI.instance

        ui_instance.server.update_pins()

    def on_continue(self, args=[]):
        ui_instance = UI.instance

        ui_instance.server.continue_exec()

    def on_reset(self, args):
        ui_instance = UI.instance

        ui_instance.update_log('reset unimplemented\n')

    def on_quit(self, args):
        ui_instance = UI.instance

        ui_instance.app.exit()

    def accept(self, buff):
        cmd = self.prompt.text
        args = cmd.split(' ')

        if args[0] in self.commands:
            self.commands[args[0]](args[1:])
        else:
            self.update_log("Unknown command: " + args[0] + "\n", Fore.RED)

        self.dirty = True

    @kb.add('c-i')
    def shift_focus(e):
        e.app.layout.focus_next()

    def update_log(self, new_text, color=""):
        self.log.text = ANSI(self.log.text.value + color + new_text + Fore.RESET)

    def get_section_str(self, sec, ptr):
        '''
        return a atring for all instructions in a given section. place the cursor string at PTR
        '''

        data_str = ''
        section_addr = sec['section_addr']
        for i in range(section_addr, section_addr + 4*(len(sec) - 1), 4):
            inst = " {:x}: {}        {}\n".format(i, sec[i][0], sec[i][1])

            if 'call' in sec[i][1]:
                if 'calla' in sec[i][1]:
                    pat = r'^(.*?) #\\([0-9]+)(.*?)' # pattern to get the address of a call instruction
                    r = re.search(pat, sec[i][1])
                    call_addr = int(r.group(2)) if r else 0
                    call_dest = p2tools.get_section(self.obj_data, call_addr)

                    if call_addr != 0:
                        # if call address is 0x200-0x400, convert it to where the LUT function is stored in HUB ram
                        if call_addr >= 0x200 and call_addr < 0x400:
                            call_addr = 4*(call_addr - 0x200) + 0x200
                        call_dest = p2tools.get_section(self.obj_data, call_addr)

                else:
                    call_dest = ''

                inst = " {:x}: {: <20}{}{}\t\t{}{}\n".format(i, 
                                                            sec[i][0], 
                                                            Fore.LIGHTGREEN_EX, 
                                                            sec[i][1], 
                                                            call_dest,
                                                            Fore.RESET)

            elif 'jmp' in sec[i][1] or 'tj' in sec[i][1] or 'dj' in sec[i][1]:

                inst = " {:x}: {: <20}{}{}{}\n".format(i, 
                                                        sec[i][0], 
                                                        Fore.CYAN, 
                                                        sec[i][1], 
                                                        Fore.RESET)
            else:
                inst = " {:x}: {: <20}{}\n".format(i, sec[i][0], sec[i][1])

            if i == ptr:
                data_str += Style.BRIGHT + self.pc_cursor_string + inst + Style.RESET_ALL
                self.pc_line = int((i - section_addr)/4)
            else:
                data_str += ' '*self.pc_cursor_size + inst + Style.RESET_ALL

        return data_str

    def prerender(self, app):
        self.render_lock.acquire()

    def postrender(self, app):
        self.render_lock.release()

    def data_updater(self):
        do_redraw = False
        while(1):
            if (self.server.stat_dirty or self.dirty):
                self.render_lock.acquire()
                self.server.stat_dirty = False
                do_redraw = True

                stat = self.server.get_status()

                if (stat):
                    # draw the status dictionary
                    stat_dict = vars(self.server.get_status())
                    stat_lines = []

                    for k in stat_dict:
                        if k.startswith('_'):
                            pass
                        elif k == 'pc':
                            stat_lines.append("{: >30} : {: <#8x}".format(k, stat_dict[k]))
                        else:
                            stat_lines.append("{: >30} : {!s: <8}".format(k, stat_dict[k]))
                    
                    stat_text = '\n'.join(stat_lines)
                    self.status.text = stat_text

                    # draw cog connections status's
                    conn_str = ''
                    for i in range(8):
                        fmt = ''
                        marker = ''
                        if self.server.cog_states[i].get_state() == p2db_server.CogState.IDLE:
                            fmt = Fore.GREEN
                        elif self.server.cog_states[i].get_state() == p2db_server.CogState.EXECUTING:
                            fmt = Fore.YELLOW
                        else:
                            fmt = Fore.RED

                        if i == self.server.current_cog:
                            fmt += Style.BRIGHT
                            marker = '*'

                        conn_str += fmt + '{: >10}'.format('{} Cog {}\n'.format(marker, i)) + Style.RESET_ALL + Fore.RESET

                    self.connection.text = ANSI(conn_str)

                    # draw the pin states
                    porta_str = ''
                    portb_str = ''
                    for i in range(32):
                        bit = ''
                        if (self.server.dira >> i) & 1:
                            color = Fore.RED + Style.BRIGHT
                        else:
                            color = Fore.LIGHTBLUE_EX + Style.BRIGHT
                        
                        if (self.server.ina >> i) & 1:
                            bit = 'H'
                        else:
                            bit = 'L'

                        if not self.server.have_pin_data:
                            bit = 'X'
                            color = Fore.LIGHTBLACK_EX
                        
                        porta_str += color + "{0: <3}".format(bit)

                        if (self.server.dirb >> i) & 1:
                            color = Fore.RED + Style.BRIGHT
                        else:
                            color = Fore.LIGHTBLUE_EX + Style.BRIGHT
                        
                        if (self.server.inb >> i) & 1:
                            bit = 'H'
                        else:
                            bit = 'L'

                        if not self.server.have_pin_data:
                            bit = 'X'
                            color = Fore.LIGHTBLACK_EX
                        
                        portb_str += color + "{0: <3}".format(bit)

                    pin_str = porta_str + '\n\n\n' + portb_str + Fore.RESET + Style.RESET_ALL
                    self.pins.text = ANSI(pin_str) 
                
                    # update the dissassembly window
                    # get the function the current PC is in
                    pc = stat.get_mem_pc()
                    cog_mode = stat.exec_mode == "cogex"

                    func_name = ''
                    for sec in self.obj_data:
                        if pc in self.obj_data[sec]:
                            section = self.obj_data[sec]
                            func_name = sec

                    if cog_mode and stat.exec_mode != 'lutex' and stat._cog_exec_base_addr == -1:
                        self.function_header.text = ANSI(Fore.YELLOW + "Cog Execution Mode. Set base address with 'cogaddr' to see disassembly" + Fore.RESET)
                        self.instructions.text = ANSI("")
                    else:
                        s = self.get_section_str(section, pc)
                        self.instructions.text = ANSI(s)
                        self.function_header.text = ANSI(func_name)

                else:
                    self.status.text = "*** No connection to cog"
                    self.function_header.text = ANSI("*** No connection to cog")

            # get the log data
            while not self.server.log_queue.empty():
                c = self.server.log_queue.get()
                if c != '\r':
                    self.update_log(c, Fore.LIGHTGREEN_EX)

            if do_redraw:
                self.render_lock.release()
                self.app.invalidate()
                do_redraw = False

            time.sleep(0.02)

    def run(self):  
        t = threading.Thread(target=self.data_updater, daemon=True)
        t.start()
        self.app.run();