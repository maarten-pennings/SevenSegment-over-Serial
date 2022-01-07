# isr2.py - Timer to activate units in a cycle - but split in slots for brightness control

# The 7-segment units are controlled one-by one in a cycle, using a timer interrupt for consistent light output.
# Model 1: every timer interrupt, the control switches from one unit to the next.
# However, we want to control brightness. 
# Model 2: the control interval of a unit is split in slots, and the unit is only powered in the lower slots: 
#          slots with a number not exceeding the brightness level.

# User configuration
framebuf = "ABCD"     # User configuration: desired content on the 7-segment units
brightness = 2        # User configuration: desired brightness level

# Static system configuration
TIMER_MS  = 1         # Static system configuration: timer fire rate
SLOTCOUNT = 5         # Static system configuration: number of slots (subdivisions of unit control) for brightness levels
UNITCOUNT = 4         # Static system configuration: number of 7-segment units

assert TIMER_MS>=1    # Depends on CPU clock speed but should not too low or the CPU load becomes to high.
assert TIMER_MS*SLOTCOUNT*UNITCOUNT<=20 # Refresh time should not be too long; refresh frequencies above 50Hz cause visual flicker.
assert UNITCOUNT==len(framebuf) # frame buffer content matches UNITCOUNT
assert 1<=brightness<=SLOTCOUNT # brightness has correct value - 0 would not show anything

# ISR state (private)
_slot  = SLOTCOUNT-1  # ISR state: current time subdivision of the control interval of a unit (for brightness control)
_unit  = UNITCOUNT-1  # ISR state: the 7-segment unit currently under control

# Hardware state (private)
_rows  = '?'          # Hardware state: control of the rows (LED segments of the units)
_column= '?'          # Hardware state: control of the column (the "common" pin of the units)

# Helper to visualize the result
_slotlog   = ""
_unitlog   = ""
_rowslog   = ""
_columnlog = ""
def log(sep=None) :
  global _slotlog, _unitlog, _rowslog, _columnlog
  global _slot, _unit, _rows, _column
  if sep :
    _slotlog  += sep
    _unitlog  += sep
    _rowslog  += sep
    _columnlog+= sep
  _slotlog  += chr(_slot+48)
  _unitlog  += chr(_unit+48)
  _rowslog  += _rows
  _columnlog+= _column

def isr() :
  global _slot, _unit, _rows, _column
  sep = None
  # Timer expired: move to the next slot
  _slot += 1
  # If slot exceeds brightness level, switch unit off
  if _slot>= brightness :
    _column = '-'
  # If slot exceeds chosen slots per unit, move to next unit
  if _slot>=SLOTCOUNT :
    _slot = 0
    _unit += 1    
    sep = ' '
    # Wrap around the unit
    if _unit>=UNITCOUNT :
      _unit = 0
      sep = '|'
    # New unit under control
    _rows = framebuf[_unit]
    _column = chr(_unit+48)
  log(sep)

for interrupt in range(SLOTCOUNT*UNITCOUNT+11) :
  isr()

print( f"user  : content '{framebuf}', brightness {brightness}/{SLOTCOUNT}")
print( f"system: frame is {SLOTCOUNT*UNITCOUNT} ISR ticks of {TIMER_MS} ms ({SLOTCOUNT} brightness levels for {UNITCOUNT} units)")
print()
print( f"slot  : {_slotlog}")
print( f"unit  : {_unitlog}")
print( f"rows  : {_rowslog}")
print( f"column: {_columnlog}")
print()
print( f"The column is only on during {brightness} of {SLOTCOUNT} ISR ticks due to brightness setting")

# user  : content 'ABCD', brightness 2/5
# system: frame is 20 ISR ticks of 1 ms (5 brightness levels for 4 units)
#
# slot  : |01234 01234 01234 01234|01234 01234 0
# unit  : |00000 11111 22222 33333|00000 11111 2
# rows  : |AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB C
# column: |00--- 11--- 22--- 33---|00--- 11--- 2
# 
# The column is only on during 2 of 5 ISR ticks due to brightness setting
