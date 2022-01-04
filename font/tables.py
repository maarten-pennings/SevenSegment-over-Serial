#!/usr/bin/python3


# fonttable.py - Script creating a fonttable for a 7-segment display
# 2022 jan 03  v1  Maarten Pennings  created
version = "v1"


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

div_7s_hlen    = 20
div_7s_hwidth  = 5
div_7s_vlen    = 20
div_7s_vwidth  = 5
div_7s_xsep    = 1
div_7s_ysep    = 1

div_7s_bgcol   = "silver"
div_7s_fgcol   = "blue"
div_7s_fgerror = "red"
div_7s_fgfree  = "green"

div_grid_hlen  = 50
div_grid_vlen  = 90
div_grid_hsep  = 2
div_grid_vsep  = 2
div_grid_title = 28

div_grid_linecol = "silver"
div_grid_bgcol   = "white"

div_grid_xcount = 16
div_grid_yfirst = 2
div_grid_ylast  = 7

div_char_fontname="consolab.ttf"
div_char_fontsize=20
div_char_fontcolor= "black"

font_unique = [
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
  0b0010_0111, # 37 7
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
  0b0000_0110, # 7C |     # 0b0000_0100, # 7C |
  0b0111_0000, # 7D }
  0b0100_0001, # 7E ~
  0b0101_1101  # 7F del
]

# Prepend the path of this script to filename to make it an absolute path
def getPath(filename):
  folder = os.path.dirname(os.path.realpath(__file__))
  path = os.path.join(folder, filename)
  return path


grid_ycount = div_grid_ylast + 1 - div_grid_yfirst
width_7s  = div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep + div_7s_vwidth     + div_7s_xsep + div_7s_vwidth
height_7s = div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth
assert width_7s  <= div_grid_hlen
assert height_7s <= div_grid_vlen

segment_a = 0b00000001
segment_b = 0b00000010
segment_c = 0b00000100
segment_d = 0b00001000
segment_e = 0b00010000
segment_f = 0b00100000
segment_g = 0b01000000
segment_p = 0b10000000

def draw_7s(draw,x,y,segments,fgcolor) :
  # draw.rectangle([x,y,x+width_7s,y+height_7s],fgcolor)
  # draw segment 'a'
  x0 = x + div_7s_vwidth + div_7s_xsep
  y0 = y
  draw.rectangle([x0,y0,x0+div_7s_hlen-1,y0+div_7s_hwidth-1],fgcolor if segments & segment_a else div_7s_bgcol)
  # draw segment 'b'
  x0 = x + div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if segments & segment_b else div_7s_bgcol)
  # draw segment 'c'
  x0 = x + div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if segments & segment_c else div_7s_bgcol)
  # draw segment 'd'
  x0 = x + div_7s_vwidth + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_hlen-1,y0+div_7s_hwidth-1],fgcolor if segments & segment_d else div_7s_bgcol)
  # draw segment 'e'
  x0 = x 
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if segments & segment_e else div_7s_bgcol)
  # draw segment 'f'
  x0 = x 
  y0 = y + div_7s_hwidth + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_vlen-1],fgcolor if segments & segment_f else div_7s_bgcol)
  # draw segment 'g'
  x0 = x + div_7s_vwidth + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_hlen-1,y0+div_7s_hwidth-1],fgcolor if segments & segment_g else div_7s_bgcol)
  # draw segment 'P'
  x0 = x + div_7s_vwidth + div_7s_xsep + div_7s_hlen + div_7s_xsep + div_7s_vwidth + div_7s_xsep
  y0 = y + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep + div_7s_hwidth + div_7s_ysep + div_7s_vlen + div_7s_ysep
  draw.rectangle([x0,y0,x0+div_7s_vwidth-1,y0+div_7s_hwidth-1],fgcolor if segments & segment_p else div_7s_bgcol)
  

def table_ascii_7segment(font7s,font7sname) :
  # Compute size for image to generate
  width  = div_grid_xcount*div_grid_hlen + (div_grid_xcount+1)*div_grid_hsep
  height = grid_ycount*div_grid_vlen + (grid_ycount+1)*div_grid_vsep       + div_grid_title
  # Create image of computed size
  image = Image.new("RGBA", (width,height), div_grid_linecol ) 
  draw = ImageDraw.Draw(image)
  char_font = ImageFont.truetype(getPath(div_char_fontname), div_char_fontsize)
  # Create the grid
  for y in range(div_grid_yfirst,div_grid_ylast+1) :
    y0 = (y-div_grid_yfirst)*(div_grid_vlen+div_grid_vsep) + div_grid_vsep
    y1 = y0+div_grid_vlen-1
    for x in range(div_grid_xcount) :
      ascii = y*div_grid_xcount+x
      # draw the grid cell
      x0 = x*(div_grid_hlen+div_grid_hsep) + div_grid_hsep
      x1 = x0+div_grid_hlen-1
      draw.rectangle([x0,y0,x1,y1],fill=div_grid_bgcol)
      # draw the hex value
      label = f"{ascii:02X}"
      sizex,sizey= draw.textsize( label, font=char_font)
      draw.text( (x0+2,y0+2), label, div_grid_linecol, font=char_font)
      head=sizey+2
      # draw the char
      if   ascii==0x20 : label = "□"
      elif ascii==0x7F : label = "←"
      else : label = chr(ascii)
      sizex,sizey= draw.textsize( label, font=char_font)
      draw.text( (x1-sizex-2,y0+2), label, div_char_fontcolor, font=char_font)
      # draw the 7-segment
      segments = font7s[ascii]
      chars = [ascii for ascii,segs in enumerate(font_unique) if segs==segments and ascii>=32]
      if len(chars)==0 : col = div_7s_fgfree
      if len(chars)==1 : col = div_7s_fgcol
      if len(chars)>=2 : col = div_7s_fgerror
      draw_7s( draw, x0+(div_grid_hlen-width_7s)//2, y1+2-height_7s-(div_grid_vlen-head-height_7s)//2, segments, col )
  # Draw the title
  label = f"ASCII to 7-Segment for font '{font7sname}'"
  sizex,sizey= draw.textsize( label, font=char_font)
  draw.text( ((width-sizex)/2,height-sizey-2), label, div_char_fontcolor, font=char_font)
  return image


def table_7segment_ascii(font7s,font7sname) :
  # Compute size for image to generate
  width  = 16*div_grid_hlen + (16+1)*div_grid_hsep
  height = 8*div_grid_vlen + (8+1)*div_grid_vsep       + div_grid_title
  # Create image of computed size
  image = Image.new("RGBA", (width,height), div_grid_linecol ) 
  draw = ImageDraw.Draw(image)
  char_font = ImageFont.truetype(getPath(div_char_fontname), div_char_fontsize)
  # Create the grid
  for y in range(8) :
    y0 = y*(div_grid_vlen+div_grid_vsep) + div_grid_vsep
    y1 = y0+div_grid_vlen-1
    for x in range(16) :
      segments = y*div_grid_xcount+x
      # draw the grid cell
      x0 = x*(div_grid_hlen+div_grid_hsep) + div_grid_hsep
      x1 = x0+div_grid_hlen-1
      draw.rectangle([x0,y0,x1,y1],fill=div_grid_bgcol)
      # draw the hex value
      label = f"{segments:02X}"
      sizex,sizey= draw.textsize( label, font=char_font)
      draw.text( (x0+2,y0+2), label, div_grid_linecol, font=char_font)
      head=sizey+2
      # draw the chars
      chars = [ascii for ascii,segs in enumerate(font_unique) if segs==segments and ascii>=32]
      label = ''.join(map(chr,chars)).replace("\x20","□").replace("\x7F","←")
      sizex,sizey= draw.textsize( label, font=char_font)
      draw.text( (x1-sizex-2,y0+2), label, div_char_fontcolor, font=char_font)
      # draw the 7-segment
      if len(chars)==0 : col = div_7s_fgfree
      if len(chars)==1 : col = div_7s_fgcol
      if len(chars)>=2 : col = div_7s_fgerror
      draw_7s( draw, x0+(div_grid_hlen-width_7s)//2, y1+2-height_7s-(div_grid_vlen-head-height_7s)//2, segments, col )
  # Draw the title
  label = f"7-Segment to ASCII for font '{font7sname}'"
  sizex,sizey= draw.textsize( label, font=char_font)
  draw.text( ((width-sizex)/2,height-sizey-4), label, div_char_fontcolor, font=char_font)
  return image



# The entry point for command line test
if __name__ == "__main__":
  image_ascii_7segment = table_ascii_7segment(font_unique,"unique")
  filename = "table_ascii_7segment.png"
  image_ascii_7segment.save(filename)
  print(filename)
  image_7segment_ascii = table_7segment_ascii(font_unique,"unique")
  filename = "table_7segment_ascii.png"
  image_7segment_ascii.save(filename)
  print(filename)


# A␣A︎␣️A