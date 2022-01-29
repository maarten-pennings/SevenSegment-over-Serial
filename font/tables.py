#!/usr/bin/python3


# tables.py - Script creating a fonttable for a 7-segment display
# 2022 jan 29  v4  Maarten Pennings  renamed mimic7s to lookalike7s
# 2022 jan 04  v3  Maarten Pennings  Added mimic7s
# 2022 jan 04  v2  Maarten Pennings  Added cross references to duplicates
# 2022 jan 03  v1  Maarten Pennings  Created
version = "v4"


import os
import io
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw


#                          <-div_7s_hlen->
#                        ^ ###############  
#          div_7s_hwidth | ## segment a ##   
#                        v ###############  
#                        ^                  
#            div_7s_ysep |                   
#       div_7s_vwidth    v                      div_7s_vwidth
#              <-----><--->               <---><-----> 
#            ^ ####### div_7s_xsep div_7s_xsep ####### 
#            | #######                         ####### 
#            | #######                         ####### 
#            | #######                         ####### 
# div_7s_vlen| ## f ##                         ## b ## 
#            | #######                         ####### 
#            | #######                         ####### 
#            | #######                         ####### 
#            v #######                         ####### 
#                        ^                  
#            div_7s_ysep |                   
#                        v
#                        ^ ###############  
#          div_7s_hwidth | ## segment g ##   
#                        v ###############  
#                        ^                  
#            div_7s_ysep |                   
#                        v
#              #######                         ####### 
#              #######                         ####### 
#              #######                         ####### 
#              #######                         ####### 
# div_7s_vlen  ## e ##                         ## c ## 
#              #######                         ####### 
#              #######                         ####### 
#              #######                         ####### 
#              #######                         ####### 
#                        ^                  
#            div_7s_ysep |                     div_7s_xsep  div_7s_vwidth
#                        v                            <---><----->
#                        ^ ###############                 #######      
#          div_7s_hwidth | ## segment d ##                 ## p ##
#                        v ###############                 #######

div_7s_hlen        = 16
div_7s_hwidth      = 5
div_7s_vlen        = 16
div_7s_vwidth      = 5
div_7s_xsep        = 1
div_7s_ysep        = 1
                   
#div_7s_bgcol       = "black"      #8
#div_7s_bgcol       = "dimgray"    #7
#div_7s_bgcol       = "gray"       #6
#div_7s_bgcol       = "darkgray"   #5
#div_7s_bgcol       = "silver"     #4
#div_7s_bgcol       = "lightgray"  #3
div_7s_bgcol       = "gainsboro"  #2
#div_7s_bgcol       = "whitesmoke" #1
#div_7s_bgcol       = "white"      #0
div_7s_fgcol       = "blue"
div_7s_fgerror     = "red"
div_7s_fgfree      = "green"
                   
div_grid_a_hlen    = 56  # width of a cell in the ascii table
div_grid_a_vlen    = 110 # height of a cell in the ascii table
div_grid_s_hlen    = 75  # width of a cell in the patterns table
div_grid_s_vlen    = 98 # height of a cell in the patterns table
div_grid_hsep      = 1   # (horizontal) border thickness in both tables
div_grid_vsep      = 1   # (vertical) border thickness in both tables
div_grid_title     = 28  # height for title at bottom in both tables
                   
div_grid_xcount    = 16
div_grid_yfirst    = 2
div_grid_ylast     = 7
                   
div_mainfont_name  ="consolab.ttf"
div_mainfont_size  = 20

div_mediumfont_name="consolab.ttf"
div_mediumfont_size= 14

div_smallfont_name ="consolab.ttf"
div_smallfont_size = 9

div_grid_linecol   = "black"
div_grid_bgcol     = "white"
div_hitext_color   = "black"
div_lotext_color   = "silver"
                   

font_unique7s = [
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, # padding for 0x0_
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, # padding for 0x1_
  # pgfedcba
  0b0000_0000, # 20 spc
  0b0010_1000, # 21 !
  0b0010_0010, # 22 "
  0b0110_0011, # 23 #
  0b0100_1001, # 24 $
  0b0010_0100, # 25 %
  0b0111_1110, # 26 &
  0b0000_0010, # 27 '
  0b0010_1001, # 28 (
  0b0000_1011, # 29 )
  0b0000_0001, # 2A *
  0b0100_0010, # 2B +
  0b0000_1100, # 2C ,
  0b0100_0000, # 2D -
  0b0001_0000, # 2E .
  0b0101_0010, # 2F /
  # pgfe_dcba
  0b0011_1111, # 30 0
  0b0000_0110, # 31 1
  0b0101_1011, # 32 2
  0b0100_1111, # 33 3
  0b0110_0110, # 34 4
  0b0110_1101, # 35 5
  0b0111_1101, # 36 6
  0b0000_0111, # 37 7
  0b0111_1111, # 38 8
  0b0110_1111, # 39 9
  0b0000_1001, # 3A :
  0b0000_1010, # 3B ;
  0b0010_0001, # 3C <
  0b0100_1000, # 3D =
  0b0000_0011, # 3E >
  0b0100_1011, # 3F ?
  # pgfe_dcba
  0b0011_1011, # 40 @
  0b0111_0111, # 41 A
  0b0111_1100, # 42 B
  0b0011_1001, # 43 C
  0b0001_1111, # 44 D
  0b0111_1001, # 45 E
  0b0111_0001, # 46 F
  0b0011_1101, # 47 G
  0b0111_0110, # 48 H
  0b0011_0000, # 49 I
  0b0001_1110, # 4A J
  0b0111_0101, # 4B K
  0b0011_1100, # 4C L
  0b0101_0101, # 4D M
  0b0011_0111, # 4E N
  0b0010_1111, # 4F O
  # pgfe_dcba
  0b0111_0011, # 50 P
  0b0110_1011, # 51 Q
  0b0011_0011, # 52 R
  0b0010_1101, # 53 S
  0b0010_1011, # 54 T
  0b0011_1110, # 55 U
  0b0111_0010, # 56 V
  0b0110_1010, # 57 W
  0b0011_0110, # 58 X
  0b0110_1110, # 59 Y
  0b0001_1011, # 5A Z
  0b0011_0001, # 5B [
  0b0110_0100, # 5C \
  0b0000_1110, # 5D ]
  0b0010_0011, # 5E ^
  0b0000_1000, # 5F _
  # pgfe_dcba
  0b0010_0000, # 60 `
  0b0101_1111, # 61 a
  0b0110_1100, # 62 b
  0b0101_1000, # 63 c
  0b0101_1110, # 64 d
  0b0111_1011, # 65 e
  0b0101_0001, # 66 f
  0b0110_0111, # 67 g
  0b0111_0100, # 68 h
  0b0000_0101, # 69 i
  0b0000_1101, # 6A j
  0b0111_1010, # 6B k
  0b0011_1000, # 6C l
  0b0001_0101, # 6D m
  0b0101_0100, # 6E n
  0b0101_1100, # 6F o
  # pgfe_dcba
  0b0101_0011, # 70 p
  0b0110_0101, # 71 q
  0b0101_0000, # 72 r
  0b0010_0101, # 73 s
  0b0111_1000, # 74 t
  0b0001_1100, # 75 u
  0b0011_0010, # 76 v
  0b0010_1010, # 77 w
  0b0001_0100, # 78 x
  0b0010_1110, # 79 y
  0b0001_0011, # 7A z
  0b0100_0110, # 7B {
  0b0000_0100, # 7C |
  0b0111_0000, # 7D }
  0b0100_0001, # 7E ~
  0b0101_1101  # 7F del
]


font_lookalike7s = [
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, # padding for 0x0_
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, # padding for 0x1_
  # pgfedcba
  0b0000_0000, # 20 spc
  0b0010_1000, # 21 !
  0b0010_0010, # 22 "
  0b0110_0011, # 23 #
  0b0100_1001, # 24 $
  0b0010_0100, # 25 %
  0b0111_1110, # 26 &
  0b0000_0010, # 27 '
  0b0011_1001, # 28 (
  0b0000_1111, # 29 )
  0b0000_0001, # 2A *
  0b0100_0010, # 2B +
  0b0000_1100, # 2C ,
  0b0100_0000, # 2D -
  0b0001_0000, # 2E .
  0b0101_0010, # 2F /
  # pgfe_dcba
  0b0011_1111, # 30 0
  0b0000_0110, # 31 1
  0b0101_1011, # 32 2
  0b0100_1111, # 33 3
  0b0110_0110, # 34 4
  0b0110_1101, # 35 5
  0b0111_1101, # 36 6
  0b0000_0111, # 37 7
  0b0111_1111, # 38 8
  0b0110_1111, # 39 9
  0b0000_1001, # 3A :
  0b0000_1010, # 3B ;
  0b0101_1000, # 3C <
  0b0100_1000, # 3D =
  0b0100_1100, # 3E >
  0b0100_1011, # 3F ?
  # pgfe_dcba
  0b0011_1011, # 40 @
  0b0111_0111, # 41 A
  0b0111_1100, # 42 B
  0b0011_1001, # 43 C
  0b0101_1110, # 44 D
  0b0111_1001, # 45 E
  0b0111_0001, # 46 F
  0b0011_1101, # 47 G
  0b0111_0110, # 48 H
  0b0011_0000, # 49 I
  0b0001_1110, # 4A J
  0b0111_0101, # 4B K
  0b0011_1100, # 4C L
  0b0101_0101, # 4D M
  0b0011_0111, # 4E N
  0b0011_1111, # 4F O
  # pgfe_dcba
  0b0111_0011, # 50 P
  0b0110_1011, # 51 Q
  0b0011_0011, # 52 R
  0b0110_1101, # 53 S
  0b0111_1000, # 54 T
  0b0011_1110, # 55 U
  0b0111_0010, # 56 V
  0b0110_1010, # 57 W
  0b0011_0110, # 58 X
  0b0110_1110, # 59 Y
  0b0101_1011, # 5A Z
  0b0011_1001, # 5B [
  0b0110_0100, # 5C \
  0b0000_1111, # 5D ]
  0b0010_0011, # 5E ^
  0b0000_1000, # 5F _
  # pgfe_dcba
  0b0010_0000, # 60 `
  0b0101_1111, # 61 a
  0b0111_1100, # 62 b
  0b0101_1000, # 63 c
  0b0101_1110, # 64 d
  0b0111_1011, # 65 e
  0b0111_0001, # 66 f
  0b0110_1111, # 67 g
  0b0111_0100, # 68 h
  0b0000_0101, # 69 i
  0b0000_1101, # 6A j
  0b0111_0101, # 6B k
  0b0011_1000, # 6C l
  0b0101_0101, # 6D m
  0b0101_0100, # 6E n
  0b0101_1100, # 6F o
  # pgfe_dcba
  0b0111_0011, # 70 p
  0b0110_0111, # 71 q
  0b0101_0000, # 72 r
  0b0110_1101, # 73 s
  0b0111_1000, # 74 t
  0b0001_1100, # 75 u
  0b0111_0010, # 76 v
  0b0110_1010, # 77 w
  0b0001_0100, # 78 x
  0b0010_1110, # 79 y
  0b0101_1011, # 7A z
  0b0100_0110, # 7B {
  0b0000_0110, # 7C |
  0b0111_0000, # 7D }
  0b0100_0001, # 7E ~
  0b0101_1101  # 7F del
]


grid_ycount = div_grid_ylast + 1 - div_grid_yfirst
width_7s  = div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep + div_7s_vwidth  #   + div_7s_xsep + div_7s_vwidth
height_7s = div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth
assert width_7s  <= div_grid_a_hlen
assert height_7s <= div_grid_a_vlen


segment_a = 0b00000001
segment_b = 0b00000010
segment_c = 0b00000100
segment_d = 0b00001000
segment_e = 0b00010000
segment_f = 0b00100000
segment_g = 0b01000000
segment_p = 0b10000000


# Converts integer `val` to an 2 char long hexadecimal string 
def hex2(val) :
  return ("00"+hex(val)[2:])[-2:]


# Converts integer `val` to an 8 char long binary string (with space in middle)
def bin8(val) :
  s = ("00000000"+bin(val)[2:])[-8:]
  return s[0:4]+" "+s[4:]


# Convert integer ascii value to character (but makes some unprintable ones readable)
def chrr(ascii) :
  if   ascii==0x20 : return "□"
  elif ascii==0x7F : return "←"
  return chr(ascii)


# Draws a 7-segment at position `(x,y)` on `draw` using color `div_7s_bgcol` for inactive segments and color `fgcolor` for active segments.
# A segment a,b,c,d,e,f,g,p is active if the corresponding bit is set in `pattern`.
def draw_7s(draw,x,y,pattern,fgcolor) :
  # draw.rectangle([x,y,x+width_7s,y+height_7s],fgcolor)
  # draw segment 'a'
  x0 = x + div_7s_vwidth + div_7s_xsep
  y0 = y
  draw.rectangle([x0,y0,x0+div_7s_hlen-1,y0+div_7s_hwidth-1],fgcolor if pattern & segment_a else div_7s_bgcol)
  # draw segment 'b'
  x0 = x + div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if pattern & segment_b else div_7s_bgcol)
  # draw segment 'c'
  x0 = x + div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if pattern & segment_c else div_7s_bgcol)
  # draw segment 'd'
  x0 = x + div_7s_vwidth + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_hlen-1,y0+div_7s_hwidth-1],fgcolor if pattern & segment_d else div_7s_bgcol)
  # draw segment 'e'
  x0 = x 
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if pattern & segment_e else div_7s_bgcol)
  # draw segment 'f'
  x0 = x 
  y0 = y + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if pattern & segment_f else div_7s_bgcol)
  # draw segment 'g'
  x0 = x + div_7s_vwidth + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_hlen-1,y0+div_7s_hwidth-1],fgcolor if pattern & segment_g else div_7s_bgcol)
  # draw segment 'P'
  x0 = x + div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep + div_7s_vwidth + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_hwidth-1],fgcolor if pattern & segment_p else div_7s_bgcol)
  

def table_ascii(font7s,font7sname) :
  # Compute size for image to generate
  width  = div_grid_xcount*div_grid_a_hlen + (div_grid_xcount+1)*div_grid_hsep
  height = grid_ycount*div_grid_a_vlen + (grid_ycount+1)*div_grid_vsep       + div_grid_title + div_grid_vsep
  # Create image of computed size
  image = Image.new("RGBA", (width,height), div_grid_linecol ) 
  draw = ImageDraw.Draw(image)
  mainfont = ImageFont.truetype(div_mainfont_name, div_mainfont_size)
  smallfont = ImageFont.truetype(div_smallfont_name, div_smallfont_size)
  # Create the grid
  for yy in range(div_grid_yfirst,div_grid_ylast+1) :
    y0 = (yy-div_grid_yfirst)*(div_grid_a_vlen+div_grid_vsep) + div_grid_vsep
    y1 = y0+div_grid_a_vlen-1
    for xx in range(div_grid_xcount) :
      y = y0+1 # vertical cursor in cell
      ascii = yy*div_grid_xcount+xx
      # Draw the grid cell
      x0 = xx*(div_grid_a_hlen+div_grid_hsep) + div_grid_hsep
      x1 = x0+div_grid_a_hlen-1
      draw.rectangle([x0,y0,x1,y1],fill=div_grid_bgcol)
      # Draw the hex ASCII value
      label = f"{ascii:02X}"
      sizex,sizey = draw.textsize( label, font=mainfont)
      draw.text( (x0+4,y), label, div_lotext_color, font=mainfont)
      # Draw the ASCII character
      label = chrr(ascii)
      sizex,sizey= draw.textsize( label, font=mainfont)
      draw.text( (x1-sizex-4,y), label, div_hitext_color, font=mainfont)
      y += div_mainfont_size
      # Draw the duplicate hex numbers: (hex numbers of) chars with same pattern
      pattern = font7s[ascii]
      otherchars = [_ascii for _ascii,_pattern in enumerate(font7s) if _ascii!=ascii and _pattern==pattern and _ascii>=32]
      if len(otherchars)==0 : 
        label = "no"
        label2 = "duplicates"
        col = div_lotext_color
      else :
        label = ' '.join(map(hex2,otherchars))
        label2 = '  '.join(map(chrr,otherchars))
        col = div_7s_fgerror
      sizex,sizey= draw.textsize( label, font=smallfont)
      draw.text( (x0+(div_grid_a_hlen-sizex)//2,y), label, col, font=smallfont)
      y += div_smallfont_size+2
      # Draw the duplicate characters
      label = label2
      sizex,sizey= draw.textsize( label, font=smallfont)
      draw.text( (x0+(div_grid_a_hlen-sizex)//2,y), label, col, font=smallfont)
      y += div_smallfont_size+4
      # Draw the 7-segment
      draw_7s( draw, x0+(div_grid_a_hlen-width_7s)//2, y, pattern, div_7s_fgcol )
      # Draw the pattern
      label = bin8(pattern)
      sizex,sizey= draw.textsize( label, font=smallfont)
      draw.text( (x0+(div_grid_a_hlen-sizex)//2, y1-2-sizey), label, div_lotext_color, font=smallfont)
  # Draw title cell
  x0 = div_grid_hsep
  y0 = grid_ycount*(div_grid_a_vlen+div_grid_vsep) + div_grid_vsep
  draw.rectangle([x0,y0,x0+width-2*div_grid_hsep-1,y0+div_grid_title-1],fill=div_grid_bgcol)
  # Draw the title
  label = f"ASCII table for font '{font7sname}'"
  sizex,sizey= draw.textsize( label, font=mainfont)
  draw.text( ((width-sizex)/2,y0+(div_grid_title-sizey)/2+2), label, div_hitext_color, font=mainfont)
  return image


def table_pattern(font7s,font7sname) :
  # Compute size for image to generate
  width  = 16*div_grid_s_hlen + (16+1)*div_grid_hsep
  height = 8*div_grid_s_vlen + (8+1)*div_grid_vsep       + div_grid_title + div_grid_vsep
  # Create image of computed size
  image = Image.new("RGBA", (width,height), div_grid_linecol ) 
  draw = ImageDraw.Draw(image)
  mainfont = ImageFont.truetype(div_mainfont_name, div_mainfont_size)
  mediumfont = ImageFont.truetype(div_mediumfont_name, div_mediumfont_size)
  smallfont = ImageFont.truetype(div_smallfont_name, div_smallfont_size)
  # Create the grid
  for yy in range(8) :
    y0 = yy*(div_grid_s_vlen+div_grid_vsep) + div_grid_vsep
    y1 = y0+div_grid_s_vlen-1
    for xx in range(16) :
      y = y0+1 # vertical cursor in cell
      pattern = yy*div_grid_xcount+xx
      # Draw the grid cell
      x0 = xx*(div_grid_s_hlen+div_grid_hsep) + div_grid_hsep
      x1 = x0+div_grid_s_hlen-1
      draw.rectangle([x0,y0,x1,y1],fill=div_grid_bgcol)
      # Draw the pattern
      label = bin8(pattern)
      sizex,sizey= draw.textsize( label, font=mediumfont)
      draw.text( (x0+(div_grid_s_hlen-sizex)//2, y), label, div_lotext_color, font=mediumfont)
      y += div_mediumfont_size+2
      # Draw the duplicate hex numbers: (hex numbers of) chars with same pattern
      chars = [_ascii for _ascii,_pattern in enumerate(font7s) if _pattern==pattern and _ascii>=32]
      if len(chars)==0 : 
        label = "not"
        label2 = "used"
        col_label = div_lotext_color
        col_7s = div_7s_fgfree
      else :
        label = ' '.join(map(hex2,chars))
        label2 = '  '.join(map(chrr,chars))
        col_label = div_hitext_color if len(chars)==1 else div_7s_fgerror
        col_7s = div_7s_fgcol
      sizex,sizey= draw.textsize( label, font=smallfont)
      draw.text( (x0+(div_grid_s_hlen-sizex)//2,y), label, col_label, font=smallfont)
      y += div_smallfont_size+2
      # Draw the duplicate characters
      label = label2
      sizex,sizey= draw.textsize( label, font=smallfont)
      draw.text( (x0+(div_grid_s_hlen-sizex)//2,y), label, col_label, font=smallfont)
      y += div_smallfont_size+4
      # Draw the 7-segment
      draw_7s( draw, x0+(div_grid_s_hlen-width_7s)//2, y, pattern, col_7s )
  # Draw title cell
  x0 = div_grid_hsep
  y0 = 8*(div_grid_s_vlen+div_grid_vsep) + div_grid_vsep
  draw.rectangle([x0,y0,x0+width-2*div_grid_hsep-1,y0+div_grid_title-1],fill=div_grid_bgcol)
  # Draw the title
  label = f"Pattern table for font '{font7sname}'"
  sizex,sizey= draw.textsize( label, font=mainfont)
  draw.text( ((width-sizex)/2,y0+(div_grid_title-sizey)/2+2), label, div_hitext_color, font=mainfont)
  return image


def main(font7s,font7sname) :
  print( f"processing '{font7sname}'")
  image = table_ascii(font7s,font7sname)
  filename = f"{font7sname}_ascii.png"
  image.save(filename)
  print( f"  saving {filename}")
  image = table_pattern(font7s,font7sname)
  filename = f"{font7sname}_pattern.png"
  image.save(filename)
  print( f"  saving {filename}")


# The entry point for command line test
if __name__ == "__main__":
  print( f"tables.py {version}")
  main(font_unique7s,"unique7s")
  main(font_lookalike7s,"lookalike7s")
