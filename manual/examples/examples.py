# Used as examples in the user manual

import serial
import time

ssos = serial.Serial(None,115200) # port=None, so not yet opened
ssos.dtr = False # Makes sure the board does not reset (would take 2 seconds)
ssos.port='COM3' # Specify port
ssos.open()      # Now open (fast)

def demo_0x00_RESET():
  ssos.write( b'\0HI')
  time.sleep(1)
  ssos.write( b'\08.8.8.8.')
  time.sleep(1)
  ssos.write( b'\00075')
  time.sleep(1)
  ssos.write( b'\x0075')
  time.sleep(1)
  ssos.write( b'\0.75')
  time.sleep(1)

def demo_0x01_SET_FONT(id=0):
  ssos.write( b'\0dD5S')
  time.sleep(1)
  ssos.write( b'\0\1udD5S')
  time.sleep(1)
  ssos.write( b'\0S\x01US\x01LS')
  time.sleep(1)

def demo_0x02_SET_BRIGHTNESS(level=4) :
  ssos.write( b'\0l=df')
  time.sleep(1)
  ssos.write( b'\0\x025l=5')
  time.sleep(1)
  ssos.write( b'\0\x024l=4')
  time.sleep(1)
  ssos.write( b'\0\x023l=3')
  time.sleep(1)
  ssos.write( b'\0\x023l=2')
  time.sleep(1)
  ssos.write( b'\0\x021l=1')
  time.sleep(1)

def demo_0x03_SET_BLINK_MASK(mask=0b1111) :
  ssos.write( b'\0ABCD')
  time.sleep(3)
  ssos.write( b'\a')
  time.sleep(3)
  ssos.write( b'\x03\x06')
  time.sleep(3)
  ssos.write( b'\x03\x09')
  time.sleep(3)
  ssos.write( b'\v')
  time.sleep(3)

def demo_0x04_SET_BLINK_TIMES(hi=0x19, lo=0x19) :
  ssos.write( b'\0\aABCD')
  time.sleep(3)
  ssos.write( b'\x04\x0A\x0A')
  time.sleep(3)
  ssos.write( b'\x04\x15\x04')
  time.sleep(3)
  ssos.write( b'\x04\x04\x15')
  time.sleep(3)

def demo_0x05_SHOW_STRINGS(id0=0x00,id1=0x29) :
  ssos.write( b'\0-8\x05\x18\x23')

def demo_0x06_CURSOR_RIGHT() :
  ssos.write( b'\0')
  time.sleep(1)
  ssos.write( b'\x06A')
  time.sleep(1)
  ssos.write( b'\x06B')
  time.sleep(1)

def demo_0x08_CURSOR_LEFT() :
  ssos.write( b'\0n=3')
  time.sleep(1)
  ssos.write( b'\b2')
  time.sleep(1)
  ssos.write( b'\b1')
  time.sleep(1)
  ssos.write( b'\x08 ')
  time.sleep(1)

def demo_0x09_CURSOR_EOLN() :
  ssos.write( b'\0\t3.14')
  time.sleep(3)
  ssos.write( b'\0\x11\t2.7\n') # line mode
  time.sleep(1)

def demo_0x0A_LINE_COMMIT() :
  ssos.write( b'\0character\n')
  time.sleep(6)
  ssos.write( b' mode')
  time.sleep(2)
  ssos.write( b'\x11')
  time.sleep(1)
  ssos.write( b'now line\n')
  time.sleep(1)
  ssos.write( b'Yes')
  time.sleep(3)
  ssos.write( b'\n')

def demo_0x0C_CLEAR_AND_HOME() :
  ssos.write( b'\0a=1')
  time.sleep(3)
  ssos.write( b'b=2')
  time.sleep(3)
  ssos.write( b'\fc=3')
  time.sleep(3)

def demo_0x0D_CURSOR_HOME() :
  ssos.write( b'\0 59')
  time.sleep(2)
  ssos.write( b'\r-')
  time.sleep(2)

def demo_0x0E_DOT_DISABLE() :
  ssos.write( b'\0')
  time.sleep(2)
  ssos.write( b'\f2.7')
  time.sleep(2)
  ssos.write( b'\f.E.F.G.H.')
  time.sleep(2)
  ssos.write( b'\x0E')
  time.sleep(2)
  ssos.write( b'\f2.7')
  time.sleep(2)
  ssos.write( b'\f.E.F.G.H.')
  time.sleep(2)
  ssos.write( b'\x0F')
  time.sleep(2)
  ssos.write( b'\f2.7')
  time.sleep(2)

def demo_0x10_CHAR_ENABLE() :
  ssos.write( b'\0\x10')
  ssos.write( b'\f12345678')
  time.sleep(5)
  ssos.write( b'\fABCDEFGH')
  time.sleep(5)
  ssos.write( b'IJKLMNOP\n')
  time.sleep(5)
  ssos.write( b'\f    abcdefghi    ')
  time.sleep(5)

def demo_0x10_CHAR_ENABLE_overflow() :
  ssos.write( b'\0\x10')
  ssos.write( b"\f1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ(-{`='}-)abcdefghijklmnopqrstuvwxyz")
  ##########################################################################Rest is lost
  ################         1         2         3         4         5
  ################1234567890123456789012345678901234567890123456789012345678

def demo_0x11_CHAR_DISABLE() :
  ssos.write( b'\0\x11')
  ssos.write( b'abcd') # no show (\n missing)
  time.sleep(2)
  ssos.write( b'efgh\n')
  time.sleep(2)
  ssos.write( b'12345678\n')
  time.sleep(2)
  ssos.write( b'ABCDEFGH\n')
  time.sleep(2)

def demo_0x11_CHAR_DISABLE_overflow() :
  ssos.write( b'\0\x11')
  ssos.write( b"\f1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ(-{`='}-)abcdefghijklmnopqrstuvwxyz1.2.3.4.5.6.7.8.9.0.A.B.C.D.E.F.G.H.I.J.K.L.M.N.O.P.Q.R.S.T.U.V.W.X.Y.Z.\n")

def demo_0x12_CHAR_TIME() :
  ssos.write( b'\0    ABCDEFGH    ')
  time.sleep(4)
  ssos.write( b'\x12\x05\f    ABCDEFGH    ')
  time.sleep(4)

def demo_0x13_PATTERN_ONE(pat=0) :
  ssos.write( b'\0A\x13\x49a')
  time.sleep(2)

def demo_0x14_PATTERN_ALL(p0=0,p1=0,p2=0,p3=0) :
  ssos.write( b'\0A\x14\x21\x01\x08\x8C')
  time.sleep(2)
  ssos.write( b'B')
  time.sleep(2)


def demo_0x1F_RESET():
  ssos.write( b'\x00HI')
  time.sleep(1)
  ssos.write( b'\x1f75')
  time.sleep(1)

demo_0x00_RESET()
demo_0x01_SET_FONT()
demo_0x02_SET_BRIGHTNESS()
demo_0x03_SET_BLINK_MASK()
demo_0x04_SET_BLINK_TIMES()
demo_0x05_SHOW_STRINGS()
demo_0x06_CURSOR_RIGHT()
#demo_0x07_BLINK_ENABLE()
demo_0x08_CURSOR_LEFT()
demo_0x09_CURSOR_EOLN()
demo_0x0A_LINE_COMMIT()
#demo_0x0B_BLINK_DISABLE()
demo_0x0C_CLEAR_AND_HOME()
demo_0x0D_CURSOR_HOME()
demo_0x0E_DOT_DISABLE()
#demo_0x0F_0x0F DOT_ENABLE()
demo_0x10_CHAR_ENABLE()
demo_0x10_CHAR_ENABLE_overflow()
demo_0x11_CHAR_DISABLE()
demo_0x11_CHAR_DISABLE_overflow()
demo_0x12_CHAR_TIME()
demo_0x13_PATTERN_ONE()
demo_0x14_PATTERN_ALL()
demo_0x1F_RESET()

