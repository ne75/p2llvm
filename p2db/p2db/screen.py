from codecs import decode
from blessed import Terminal
from io import StringIO 
import readline
import re

from . import p2tools
from .debug_cmd import P2DBPrompt

class Window():

    # see https://en.wikipedia.org/wiki/Box-drawing_character#Unix,_CP/M,_BBS
    VLINE = 'x'
    HLINE = 'q'
    BRCORNER = 'j'
    BLCORNER = 'm'
    URCORNER = 'k'
    ULCORNER = 'l'

    def __init__(self, term, x, y, w, h, title='') -> None:
        self.term = term
        self.x = int(x)
        self.y = int(y)
        self.w = int(w)
        self.h = int(h)

        self.box_char_active = False
        self.title = title
        if (self.title):
            self.title_render = " " + title + " "
        else:
            self.title_render = ""

    def _box_char_start(self) -> None:
        self.box_char_active = True
        print(u'\x1b(0', end='')

    def _box_char_end(self) -> None:
        self.box_char_active = False
        print(u'\x1b(B', end='')
        
    def _draw_border(self) -> None:
        self._box_char_start()
        with self.term.location(self.x, self.y):
            print(self.ULCORNER, end='');

            self._box_char_end()
            print(self.title_render, end='')
            self._box_char_start()

            top_border = self.HLINE*(self.w-len(self.title_render)-2)
            print(top_border, end='')

            print(self.URCORNER, end='');

            for i in range(1, self.h-1):
                print(self.term.move_xy(self.x, self.y + i), end='')
                print(self.VLINE, end='')
                print(self.term.move_xy(self.x + self.w - 1, self.y + i), end='')
                print(self.VLINE, end='')

            print(self.term.move_xy(self.x, self.y + self.h - 1), end='')
            print(self.BLCORNER, end='');

            top_border = self.HLINE*(self.w-2)
            print(top_border, end='')

            print(self.BRCORNER, end='');


        self._box_char_end()

    def clear(self) -> None:
        with self.term.location(self.x+1, self.y+1):
            for i in range(1, self.h-1):
                print(' '*(self.w-2))
                print(self.term.move_x(self.x+1), end='')

    def resize(self, w, h) -> None:
        self.w = w
        self.h = h

    def move(self, x, y) -> None:
        self.x = x
        self.y = y

    def render(self) -> None:
        self._draw_border()

class CommandWindow(Window):
    def __init__(self, term, x, y, w, h) -> None:
        super().__init__(term, x, y, w, h)

    def render(self) -> None:
        super().render()

    def update(self) -> None:
        self.render()

        with self.term.location(self.x+1, self.y+1):
            print(self.term.normal_cursor, end='');
            cmd = input("(p2db) ")
            print(self.term.hide_cursor, end='');

        with self.term.location(self.x+1, self.y+1):
            print(' '*(self.w - 2))

        return cmd

class LogWindow(Window):
    def __init__(self, strout, term, x, y, w, h) -> None:
        super().__init__(term, x, y, w, h)

        self.strout = strout
        self.lines = []

    def render(self) -> None:
        self.clear()

        with self.term.location(self.x+2, self.y+1):
            for i in range(len(self.lines)):
                print(self.lines[i])
                print(self.term.move_x(self.x+2), end='')

        super().render()

    def update(self) -> None:
        self.lines = self.strout.getvalue().splitlines()

        if len(self.lines) > self.h-2:
            self.lines = self.lines[-(self.h-2):]

        self.render()

class StatusWindow(Window):
    def __init__(self, term, x, y, w, h) -> None:
        super().__init__(term, x, y, w, h, "Status")

        self.stat_lines = []
        self.conn_lines = []
        self.porta_str = 0
        self.portb_str = 0

        self.render_empty = False

    def render(self) -> None:
        self.clear()

        if self.render_empty:
            with self.term.location(self.x+5, self.y+2):
                print(self.term.orange + self.term.bold + "No connection to cog" + self.term.normal)
        else:
            # draw status lines
            with self.term.location(self.x+2, self.y+2):
                for i in range(len(self.stat_lines)):
                    print(self.stat_lines[i])
                    print(self.term.move_x(self.x+2), end='')
            
            # draw pin status
            with self.term.location(self.x+3, self.y + self.h - 6):
                for i in range(32):
                    print("{0: <3}".format(i), end="")

                print(self.term.move_xy(self.x+3, self.y + self.h - 5), end='')
                print(self.porta_str)
                print(self.term.move_xy(self.x+3, self.y + self.h - 4), end='')
                print(self.portb_str)

                print(self.term.move_xy(self.x+3, self.y + self.h - 3), end='')
                for i in range(32):
                    print("{0: <3}".format(i+32), end="")

            # draw cog status
            with self.term.location(self.x+self.w-13, self.y+2):
                for i in range(len(self.conn_lines)):
                    print(self.conn_lines[i])
                    print(self.term.move_x(self.x+self.w-13), end='')

        super().render()

    def update(self, status, conn, current_cog, dira, dirb, ina, inb) -> None:
        # create status table
        if status == None:
            self.render_empty = True
            self.render()
            return

        self.render_empty = False

        self.stat_lines = []
        stat_dict = vars(status)

        for k in stat_dict:
            if k.startswith('_'):
                pass
            elif k == 'pc':
                self.stat_lines.append("{: <22}{: >#8x}".format(k, stat_dict[k]))
            else:
                self.stat_lines.append("{: <22}{!s: >8}".format(k, stat_dict[k]))

        # create pin table
        self.porta_str = ''
        self.portb_str = ''
        for i in range(32):
            bit = ''
            if (dira >> i) & 1:
                color = self.term.red
            else:
                color = self.term.cyan
            
            if (ina >> i) & 1:
                bit = 'H'
            else:
                bit = 'L'
            
            self.porta_str += color + "{0: <3}".format(bit) + self.term.normal

            if (dirb >> i) & 1:
                color = self.term.red
            else:
                color = self.term.cyan
            
            if (inb >> i) & 1:
                bit = 'H'
            else:
                bit = 'L'
            
            self.portb_str += color + "{0: <3}".format(bit) + self.term.normal

        # create connection table
        self.conn_lines = []

        for i in range(8):
            fmt = ''
            marker = ''
            if conn[i]:
                fmt = self.term.green
            else:
                fmt = self.term.red

            if i == current_cog:
                fmt += self.term.bold
                marker = '*'

            self.conn_lines.append(fmt + '{: >10}'.format('{} Cog {}'.format(marker, i)) + self.term.normal)


        self.render()

class InstructionWindow(Window):
    def __init__(self, obj_data, term, x, y, w, h) -> None:
        super().__init__(term, x, y, w, h, title="Source")

        self.obj_data = obj_data
        self.inst_to_render = []
        self.section_to_render = ''
        self.exec_mode_str = ''
        self.pc = 0

        self.render_empty = False
        self.cog_mode = False

    def render(self) -> None:
        self.clear()

        if self.render_empty:
            with self.term.location(self.x+5, self.y+2):
                print(self.term.orange + self.term.bold + "---" + self.term.normal)
        elif self.exec_mode_str:
            with self.term.location(self.x+5, self.y+2):
                print(self.exec_mode_str)
        else:
            with self.term.location(self.x+5, self.y+2):
                print(self.section_to_render)

            with self.term.location(self.x+10, self.y+5):
                for i in range(len(self.inst_to_render)): 
                    # draw calls in green
                    if 'call' in self.inst_to_render[i][2]:
                        print(self.term.lightgreen, end='')

                    if self.pc == self.inst_to_render[i][0]:
                        print(self.term.bold_cyan, end='')
                        print(self.term.move_left(5), end='')
                        print("---> ", end='')

                    print("{:x}: {}\t\t{}".format(self.inst_to_render[i][0], self.inst_to_render[i][1], self.inst_to_render[i][2]), end='')  

                    if 'calla' in self.inst_to_render[i][2]:    # only draw the address for calla since those correspond to an explicit function that has a section
                        pat = r'^(.*?) #([0-9]+)(.*?)' # pattern to get the address of a call instruction
                        with open("p2db.log", 'a') as f:
                            f.write("call instruction: {}".format(self.inst_to_render[i][2]) + '\n')

                        r = re.search(pat, self.inst_to_render[i][2])

                        call_addr = r.group(2) if r else 0

                        if call_addr == 0:
                            call_dest_name = '<indirect function call>'
                        else:
                            call_dest_name = p2tools.get_section(self.obj_data, int(call_addr))

                        print("\t" + call_dest_name)

                    print(self.term.normal, end='')
                    print(self.term.move_xy(self.x+10, self.y+5+i), end='')

        super().render()

    def update(self, status):
        if status == None:
            self.render_empty = True
            self.render()
            return

        pc = status.get_mem_pc()
        exec_mode = status.exec_mode

        self.render_empty = False

        self.inst_to_render = []
        self.pc = pc
        self.cog_mode = exec_mode == "cogex"
        self.exec_mode_str = ''

        if self.cog_mode:
            if status._cog_exec_base_addr == -1:
                self.exec_mode_str = "Cog Execution Mode"

        for sec in self.obj_data:
            if pc in self.obj_data[sec]:
                self.section_to_render = sec
                
        sec = self.section_to_render

        section_start = self.obj_data[sec]["section_addr"]
        section_end = self.obj_data[sec]["section_addr"] + 4*(len(self.obj_data[sec]) - 1)

        inst_start = max(section_start, pc - 4*int((self.h/2) - 5))
        inst_end = min(section_end, pc + 4*int((self.h/2) - 2))

        for i in range(inst_start, inst_end, 4):
            self.inst_to_render.append([i] + self.obj_data[sec][i])

        self.render()


CMD_WINDOW_H = 3
LOG_WINDOW_H = 20
STAT_WINDOW_W = 104

class Screen():

    def __init__(self, ser, obj_data) -> None:
        global CMD_WINDOW_H
        global LOG_WINDOW_H
        global STAT_WINDOW_W
        self.term = Terminal()

        self.strout = StringIO()
        self.cmd = P2DBPrompt(stdout=self.strout)
        self.cmd.init(ser, obj_data)

        if (STAT_WINDOW_W < 1):
            STAT_WINDOW_W = int(STAT_WINDOW_W*self.term.width)

        self.cmd_window = CommandWindow(self.term, 0, self.term.height-CMD_WINDOW_H, self.term.width, CMD_WINDOW_H)
        self.log_window = LogWindow(self.strout, self.term, 0, self.term.height-CMD_WINDOW_H - LOG_WINDOW_H, self.term.width, LOG_WINDOW_H)
        self.stat_window = StatusWindow(self.term, 0, 0, STAT_WINDOW_W, self.term.height - CMD_WINDOW_H - LOG_WINDOW_H)
        self.inst_window = InstructionWindow(obj_data, self.term, STAT_WINDOW_W, 0, self.term.width-STAT_WINDOW_W, self.term.height - CMD_WINDOW_H - LOG_WINDOW_H)
        self.done = False

    def run(self):
        
        self.cmd.preloop()

        with self.term.fullscreen(), self.term.hidden_cursor():
            while not self.done:
                stat = self.cmd.get_status()
                self.stat_window.update(stat, self.cmd.active, self.cmd.current_cog, self.cmd.dira, self.cmd.dirb, self.cmd.ina, self.cmd.inb)
                self.log_window.update()

                self.inst_window.update(stat)

                cmd_str = self.cmd_window.update()
                if self.cmd.onecmd(cmd_str):
                    break