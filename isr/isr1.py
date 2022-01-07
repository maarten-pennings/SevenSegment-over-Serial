# isr1.py - Timer to activate units in a cycle

# The 7-segment units are controlled one-by one in a cycle, using a timer interrupt for consistent light output.
# Model 1: every timer interrupt, the control switches from one unit to the next.

# User configuration
framebuf = "ABCD"     # User configuration: desired content on the 7-segment units

# Static system configuration
TIMER_MS  = 5         # Static system configuration: timer fire rate
UNITCOUNT = 4         # Static system configuration: number of 7-segment units

assert TIMER_MS>=1    # Depends on CPU clock speed but should not too low or the CPU load becomes to high.
assert TIMER_MS*UNITCOUNT<=20 # Refresh time should not be too long; refresh frequencies above 50Hz cause visual flicker.
assert UNITCOUNT==len(framebuf) # frame buffer content matches UNITCOUNT

# ISR state (private)
_unit  = UNITCOUNT-1  # ISR state: the 7-segment unit currently under control

# Hardware state (private)
_rows  = '?'          # Hardware state: control of the rows (LED segments of the units)
_column= '?'          # Hardware state: control of the column (the "common" pin of the units)

# Helper to visualize the result
_unitlog   = ""
_rowslog   = ""
_columnlog = ""
def log(sep=None) :
  global _unitlog, _rowslog, _columnlog
  global _slot, _unit, _rows, _column
  if sep :
    _unitlog  += sep
    _rowslog  += sep
    _columnlog+= sep
  _unitlog  += chr(_unit+48)
  _rowslog  += _rows
  _columnlog+= _column

def isr() :
  global _unit, _rows, _column
  sep = None
  # Timer expired: control next unit
  _column = '-'
  _unit += 1
  # Wrap around the unit
  if _unit>=UNITCOUNT :
    _unit = 0
    sep = '|'
  # New unit under control
  _rows = framebuf[_unit]
  _column = chr(_unit+48)
  log(sep)
    
for interrupt in range(UNITCOUNT+11) :
  isr()

print( f"user  : content '{framebuf}'")
print( f"system: frame is {UNITCOUNT} ISR ticks of {TIMER_MS} ms, one per 7-segment unit")
print()
print( f"unit  : {_unitlog}")
print( f"rows  : {_rowslog}")
print( f"column: {_columnlog}")
print()
print( "Every ISR tick, the rows and column of a (next) unit are powered")

# user  : content 'ABCD'
# system: frame is 4 ISR ticks of 5 ms, one per 7-segment unit
#
# unit  : |0123|0123|0123|012
# rows  : |ABCD|ABCD|ABCD|ABC
# column: |0123|0123|0123|012
# 
# Every ISR tick, the rows and column of a (next) unit are powered