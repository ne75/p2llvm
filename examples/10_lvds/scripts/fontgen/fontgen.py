#
# FontGen
# Author: Jared Sanson (jared@jared.geek.nz)
#
# Requires Python Imaging Library (PIL)
#
# To add a new font, create a new FONT dictionary and tweak the parameters
# until the output .png looks correct.
# Monospaced fonts work best, but some variable-width ones work well too.
#
# Once the png file looks good, you can simply include the .h file in your
# project and use it. (how you use it is up to you)
#

from PIL import Image, ImageFont, ImageDraw
import os.path

FONT = {'fname': r'Menlo.ttc', 'idx': 1, 'size': 28, 'yoff':-2, 'w': 16, 'h': 32}

#TODO: Support variable-width character fonts

FONT_FILE = FONT['fname']
FONT_SIZE = FONT['size']
FONT_Y_OFFSET = FONT.get('yoff', 0)

CHAR_WIDTH = FONT.get('w', 5)
CHAR_HEIGHT = FONT.get('h', 8)

FONT_BEGIN = ' '
FONT_END = '~'
#FONTSTR = ''.join(chr(x).upper() for x in range(ord(FONT_BEGIN), ord(FONT_END)+1))
FONTSTR = ''.join(chr(x) for x in range(ord(FONT_BEGIN), ord(FONT_END)+1))

OUTPUT_NAME = os.path.splitext(FONT_FILE)[0] + '_font'
OUTPUT_PNG = OUTPUT_NAME + '.png'
OUTPUT_H = OUTPUT_NAME + '.h'

GLYPH_WIDTH = CHAR_WIDTH+4

WIDTH = GLYPH_WIDTH * len(FONTSTR)
HEIGHT = CHAR_HEIGHT

img = Image.new("RGBA", (WIDTH, HEIGHT), (255,255,255))
#fnt = ImageFont.load_default()
fnt = ImageFont.truetype(FONT_FILE, FONT_SIZE, FONT.get('idx'))

drw = ImageDraw.Draw(img)
#drw.fontmode = 1

for i in range(len(FONTSTR)):
    drw.text((i*GLYPH_WIDTH,FONT_Y_OFFSET), FONTSTR[i], (0,0,0), font=fnt)

img.save(OUTPUT_PNG)

#### Convert to C-header format
f = open(OUTPUT_H, 'w')
num_chars = len(FONTSTR)
f.write('const uint32_t Menlo_font[%d][%d] = {\n' % (num_chars, CHAR_HEIGHT))

chars = []
for i in range(num_chars):
    ints = []
    # for j in range(CHAR_WIDTH):
    #     x = i*GLYPH_WIDTH + j
    #     val = 0
    #     for y in range(CHAR_HEIGHT):
    #         rgb = img.getpixel((x,y))
    #         val = (val >> 1) | (0x80 if rgb[0] == 0 else 0)

    #     ints.append('0x%.8x' % (val))

    for j in range(CHAR_HEIGHT):
        val = 0
        for x in range(CHAR_WIDTH):
            rgb = img.getpixel((x+i*GLYPH_WIDTH, j))
            val = (val >> 1) | (0x8000 if rgb[0] < 100 else 0)

        ints.append('0x%.4x' % (val))

    c = FONTSTR[i]
    if c == '\\': c = '"\\"' # bugfix
    f.write('\t{%s}, // %s\n' % (','.join(ints), c))


f.write('};\n\n')

f.close()
