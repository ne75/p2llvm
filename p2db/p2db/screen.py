from codecs import decode
from blessed import Terminal
from io import StringIO 
import time

from debug_cmd import P2DBPrompt

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
            cmd = input("(p2db) ")

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
        self.lines.extend(self.strout.getvalue().splitlines())
        self.strout.truncate(0)
        self.strout.seek(0)

        if len(self.lines) > self.h-2:
            self.lines = self.lines[-(self.h-2):]

        self.render()

class StatusWindow(Window):
    def __init__(self, term, x, y, w, h) -> None:
        super().__init__(term, x, y, w, h, "Status")

        self.lines = []
        self.porta_str = 0
        self.portb_str = 0

        self.render_empty = False

    def render(self) -> None:
        self.clear()

        if self.render_empty:
            with self.term.location(self.x+5, self.y+2):
                print(self.term.magenta + "No connection to cog" + self.term.normal)
        else:
            with self.term.location(self.x+2, self.y+2):
                for i in range(len(self.lines)):
                    print(self.lines[i])
                    print(self.term.move_x(self.x+2), end='')

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

        super().render()

    def update(self, status, dira, dirb, ina, inb) -> None:
        if status == None:
            self.render_empty = True
            self.render()
            return

        self.render_empty = False

        self.lines = []
        stat_dict = vars(status)

        for k in stat_dict:
            if k == 'pc':
                self.lines.append("{}: {:#02x}".format(k, stat_dict[k]))
            elif k.startswith('_'):
                pass
            else:
                self.lines.append("{}: {}".format(k, stat_dict[k]))

        if len(self.lines) > self.h-2:
            self.lines = self.lines[-(self.h-2):]

        # the pin status  should be drawn as: 
        #     0   1   2   3 ... 31  32
        #     X   L   H ...
        # x is not defined (for whatever reason)
        # L/H for low high state, red if output, blue if input
        # 

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

        self.render()

class InstructionWindow(Window):
    def __init__(self, obj_data, term, x, y, w, h) -> None:
        super().__init__(term, x, y, w, h, title="Source")

        self.obj_data = obj_data
        self.inst_to_render = []
        self.section_to_render = ''
        self.pc = 0

        self.render_empty = False

    def render(self) -> None:
        self.clear()

        if self.render_empty:
            with self.term.location(self.x+5, self.y+2):
                print(self.term.magenta + "---" + self.term.normal)
        else:
            with self.term.location(self.x+5, self.y+2):
                print(self.section_to_render)

            with self.term.location(self.x+10, self.y+5):
                for i in range(len(self.inst_to_render)): 
                    if self.pc == self.inst_to_render[i][0]:
                        print(self.term.bold_cyan, end='')

                    print("{:x}: {}\t\t{}".format(self.inst_to_render[i][0], self.inst_to_render[i][1], self.inst_to_render[i][2]))  
                    print(self.term.normal, end='')
                    print(self.term.move_x(self.x+10), end='')

        super().render()

    def update(self, pc):
        if pc == None:
            self.render_empty = True
            self.render()
            return

        self.render_empty = False

        self.inst_to_render = []
        self.pc = pc

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
        self.cmd.init(ser)

        if (STAT_WINDOW_W < 1):
            STAT_WINDOW_W = int(STAT_WINDOW_W*self.term.width)

        self.cmd_window = CommandWindow(self.term, 0, self.term.height-CMD_WINDOW_H, self.term.width, CMD_WINDOW_H)
        self.log_window = LogWindow(self.strout, self.term, 0, self.term.height-CMD_WINDOW_H - LOG_WINDOW_H, self.term.width, LOG_WINDOW_H)
        self.stat_window = StatusWindow(self.term, 0, 0, STAT_WINDOW_W, self.term.height - CMD_WINDOW_H - LOG_WINDOW_H)
        self.inst_window = InstructionWindow(obj_data, self.term, STAT_WINDOW_W, 0, self.term.width-STAT_WINDOW_W, self.term.height - CMD_WINDOW_H - LOG_WINDOW_H)
        self.done = False

    def run(self):
        
        self.cmd.preloop()

        with self.term.fullscreen():
            while not self.done:
                stat = self.cmd.get_status()
                self.stat_window.update(stat, self.cmd.dira, self.cmd.dirb, self.cmd.ina, self.cmd.inb)
                self.log_window.update()

                if stat == None:
                    self.inst_window.update(None)
                else:
                    self.inst_window.update(self.cmd.get_status().pc)

                cmd_str = self.cmd_window.update()
                if self.cmd.onecmd(cmd_str):
                    break