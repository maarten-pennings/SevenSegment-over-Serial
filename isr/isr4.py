# isr4.py - Timer to activate units in a cycle - split in slots for brightness control - support for unit level blinking

# The 7-segment units are controlled one-by one in a cycle, using a timer interrupt for consistent light output.
# Model 1: every timer interrupt, the control switches from one unit to the next.
# However, we want to control brightness. 
# Model 2: the control interval of a unit is split in slots, and the unit is only powered in the lower slots: 
#          slots with a number not exceeding the brightness level.
# We also like the display to support blinking. 
# Model 3: count frames, and a unit is only powered in the "on frames" but not in the "off frames"
# Blinking can now be controlled per unit
# Model 4: add noblink mask

# User configuration
framebuf = "ABCD"     # User configuration: desired content on the 7-segment units
brightness = 4        # User configuration: desired brightness level
framecount= 5         # User configuration: blinking period
frameson  = 3         # User configuration: number of frames during blinking period that the units are on
noblink   = 0b0101    # User configuration: which units are always on (exempt from blinking)

# Static system configuration
TIMER_MS  = 1         # Static system configuration: timer fire rate
SLOTCOUNT = 5         # Static system configuration: number of slots (subdivisions of unit control) for brightness levels
UNITCOUNT = 4         # Static system configuration: number of 7-segment units

assert TIMER_MS>=1    # Depends on CPU clock speed but should not too low or the CPU load becomes to high.
assert TIMER_MS*SLOTCOUNT*UNITCOUNT<=20 # Refresh time should not be too long; refresh frequencies above 50Hz cause visual flicker.
assert UNITCOUNT==len(framebuf) # frame buffer content matches UNITCOUNT
assert 1<=brightness<=SLOTCOUNT # brightness has correct value - 0 would not show anything
assert 1<=frameson<=framecount  # blinking is correctly configured
assert noblink < 2**UNITCOUNT   # blink exemption is correctly configured

# ISR state (private)
_slot  = SLOTCOUNT-1  # ISR state: current time subdivision of the control interval of a unit (for brightness control)
_unit  = UNITCOUNT-1  # ISR state: the 7-segment unit currently under control
_frame = framecount-1 # ISR state: current frame number

# Hardware state (private)
_rows  = '?'          # Hardware state: control of the rows (LED segments of the units)
_column= '?'          # Hardware state: control of the column (the "common" pin of the units)

# Helper to visualize the result
_slotlog   = ""
_unitlog   = ""
_framelog  = ""
_rowslog   = ""
_columnlog = ""
def log(sep=None) :
  global _slotlog, _unitlog, _framelog, _rowslog, _columnlog
  global _slot, _unit, _rows, _column
  if sep=='][' and _slotlog=="" : sep = '['
  if sep :
    _slotlog  += sep
    _unitlog  += sep
    _framelog  += sep
    _rowslog  += sep
    _columnlog+= sep
  _slotlog  += chr(_slot+48)
  _unitlog  += chr(_unit+48)
  _framelog  += chr(_frame+48)
  _rowslog  += _rows
  _columnlog+= _column

def isr() :
  global _slot, _unit, _frame, _rows, _column
  sep = None
  # Timer expired: move to next slot slot
  _slot += 1
  # If slot exceeds brightness level switch unit off
  if _slot>= brightness :
    _column = '-'
  # If slot exceeds chosen slot slots per unit, move to next unit
  if _slot>=SLOTCOUNT :
    _slot = 0
    _unit += 1    
    sep = ' '
    # Wrap around the unit
    if _unit>=UNITCOUNT :
      _unit = 0
      sep = '|'
      _frame += 1
      if _frame>=framecount :
        _frame = 0
        sep = ']['
    # New unit under control, switch it on
    if _frame<frameson or noblink & (1<<_unit):
      _rows = framebuf[_unit]
      _column = chr(_unit+48)
  log(sep)
      
for interrupt in range(SLOTCOUNT*UNITCOUNT*framecount+5) :
  isr()

print( f"user  : content '{framebuf}', brightness {brightness}/{SLOTCOUNT}, blink={bin(noblink)}-{frameson}/{framecount}")
print( f"system: frame is {SLOTCOUNT*UNITCOUNT} ISR ticks of {TIMER_MS} ms ({SLOTCOUNT} brightness levels for {UNITCOUNT} units), blinking period is {framecount} frames")
print()
print( f"slot  : {_slotlog}")
print( f"unit  : {_unitlog}")
print( f"frame : {_framelog}")
print( f"rows  : {_rowslog}")
print( f"column: {_columnlog}")
print()
print( f"For brightness, column is only on {brightness} of {SLOTCOUNT} ISRs; for blinking {frameson} of {framecount} frames, but on for non-blinking units in last {framecount-frameson} frames")

# user  : content 'ABCD', brightness 4/5, blink=0b101-3/5
# system: frame is 20 ISR ticks of 1 ms (5 brightness levels for 4 units), blinking period is 5 frames
#
# slot  : [01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234][01234
# unit  : [00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333][00000
# frame : [00000 00000 00000 00000|11111 11111 11111 11111|22222 22222 22222 22222|33333 33333 33333 33333|44444 44444 44444 44444][00000
# rows  : [AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB CCCCC DDDDD|AAAAA AAAAA CCCCC CCCCC|AAAAA AAAAA CCCCC CCCCC][AAAAA
# column: [0000- 1111- 2222- 3333-|0000- 1111- 2222- 3333-|0000- 1111- 2222- 3333-|0000- ----- 2222- -----|0000- ----- 2222- -----][0000-
# 
# For brightness, column is only on 4 of 5 ISRs; for blinking 3 of 5 frames, but on for non-blinking units in last 2 frames
