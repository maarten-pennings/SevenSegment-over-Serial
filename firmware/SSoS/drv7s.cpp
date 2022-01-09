// drv7s.cpp - the 7-segment driver

#include "Arduino.h"
#include "drv7s.h"

// Define the following macro to have the builtin LED (it is on D13 = PB5) pulse on every click tick
//#define DRV7S_DEBUG 1


// The frame buffer is exposed publicly
uint8_t drv7s_framebuf[DRV7S_UNITCOUNT] = { 0 }; // Desired content on the 7-segment units (each 1-bit is a segment that is on)

// Private part of 7-segment driver
static uint8_t drv7s_slot;       // current time subdivision of the control interval of a unit (for brightness control)
static uint8_t drv7s_unit;       // the 7-segment unit currently under control
static uint8_t drv7s_frame;      // the 7-segment unit currently under control
static uint8_t drv7s_brightness; // desired brightness level (1..DRV7S_SLOTCOUNT)
static uint8_t drv7s_alwayshi;   // desired blinking state (0 is blinking, 1 is always hi)
static uint8_t drv7s_framecount; // desired blinking period (in frames)
static uint8_t drv7s_frameshi;   // desired hi-time for blinking (in frames)
static uint8_t drv7s_noblinkmask;// desired bit mask indicating which units are exempt from blinking (are always on)

// Resets state (brightness:4, blinking:disabled,25/25,0b0000)
void drv7s_reset() {
  drv7s_slot       = DRV7S_SLOTCOUNT-1; // First ISR will roll over to slot 0
  drv7s_unit       = DRV7S_UNITCOUNT-1; // First ISR will roll over to unit 0  

  drv7s_brightness = DRV7S_SLOTCOUNT-1; // Brightness to 4/5

  // 1 frame is 20ms
  drv7s_alwayshi   = 1;  // blinking disabled
  drv7s_framecount = 50; // 1000 ms
  drv7s_frameshi   = 25; //  500 ms
  drv7s_noblinkmask= 0;  // all units blink
  drv7s_frame      = drv7s_framecount-1;// First ISR will roll over to frame 0  
}

// Sets the brightness level to `val`, iff 1<=val<=DRV7S_SLOTCOUNT.
void drv7s_brightness_set( uint8_t val ) {
  if( 1<=val && val<=DRV7S_SLOTCOUNT ) drv7s_brightness = val;
}

// Returns current brightness level.
uint8_t drv7s_brightness_get(  ) {
  return drv7s_brightness;
}

// Sets the blinking mode to `enabled` (0 is disabled, otherwise enabled).
void drv7s_blinking_mode_set( uint8_t enabled ) {
  drv7s_alwayshi = enabled==0; 
}

// Returns current blinking mode.
uint8_t drv7s_blinking_mode_get(  ) {
  return ! drv7s_alwayshi;
}

// Sets the blinking to `hi` frames "on", then `lo` frames "off", iff hi>=1 and lo>=1 and hi+lo<255.
void drv7s_blinking_hilo_set( uint8_t hi, uint8_t lo ) {
  uint8_t sum = hi+lo; // prepare to check for overflow
  if( hi>=1 && lo>=1 && sum>=hi ) { drv7s_framecount=sum; drv7s_frameshi=hi; }
}

// Returns current blinking hi time (in frames).
uint8_t drv7s_blinking_hi_get(  ) {
  return drv7s_frameshi;
}

// Returns current blinking lo time (in frames).
uint8_t drv7s_blinking_lo_get(  ) {
  return drv7s_framecount - drv7s_frameshi;
}

// Sets the blinking mask: if bit i is set, unit i will blink.
void drv7s_blinking_mask_set( uint8_t mask ) {
  drv7s_noblinkmask = ~mask; 
}

// Returns current blinking mask.
uint8_t drv7s_blinking_mask_get(  ) {
  return (~ drv7s_noblinkmask ) & ((1<<DRV7S_UNITCOUNT)-1);
}

// How the ports are wired. Letter denotes segment, digit denotes common of unit, nc means not connected
//   bit     7  6  5  4  3  2  1  0
//   Port C: nc nc p  g  f  e  d  c
//   label : A7 A6 A5 A4 A3 A2 A1 A0
//   Port D: b  a  3  2  1  0  nc nc
//   label : D7 D6 D5 D4 D3 D2 Rx TX

// drv7s_isr() allocated to TIMER2_COMPA_vect
ISR( TIMER2_COMPA_vect ) {
  #if DRV7S_DEBUG
    // Debug: track ISR ticks using builtin LED (it is on D13 = PB5)
    PORTB |= 1<<5; // ON
    PORTB &= ~(1<<5); // OFF
  #endif
  // Timer tick: move to next slot
  drv7s_slot += 1;
  // If slot exceeds brightness level, switch current unit off 
  if( drv7s_slot >= drv7s_brightness ) {
    PORTD = 0; // all columns off (also switches off rows, but don't care)
  }
  // If slot wraps around, move to next unit
  if( drv7s_slot >= DRV7S_SLOTCOUNT ) {
    drv7s_slot = 0;
    drv7s_unit += 1;    
    // Wrap around the unit
    if( drv7s_unit >= DRV7S_UNITCOUNT ) {
      drv7s_unit = 0;
      drv7s_frame += 1;
      // Wrap around the frame
      if( drv7s_frame >= drv7s_framecount) {
        drv7s_frame = 0;
      }
    }
    // New unit under control, switch it on (unless in blinking off)
    if( drv7s_alwayshi || (drv7s_noblinkmask & (1<<drv7s_unit)) || (drv7s_frame < drv7s_frameshi)  ) {
      uint8_t rows = drv7s_framebuf[drv7s_unit];
      uint8_t col = 1 << drv7s_unit;
      PORTC = (rows>>2);
      PORTD = ((rows&3)<<6) | (col<<2); // colums on after rows
    }
  }
}

// Setup (hardware registers) for 7-segments
void drv7s_setup() {
  // Setup row/column drivers
  PORTC = 0;   // all port C outputs low
  PORTD = 0;   // all port D outputs low
  DDRC = 0xFF; // all port C pins are output
  DDRD = 0xFF; // all port D pins are output

  #if DRV7S_DEBUG
    // Debug: track ISR ticks using builtin LED (it is on D13 = PB5)
    DDRB  = 1<<5; // pin PB5 as output
  #endif
  
  // Configure initial state
  drv7s_reset();
  
  // Setup timer 2 to calling IST every 1 ms
  noInterrupts(); // Disable all interrupts - cli()
  TCCR2A = 0; // Do not understand why, but without first setting to 0, we get 4.063us instead of 1ms
  // Set Output Compare Register A for 1kHz (1ms)
  OCR2A = 16000000/*CPU*/  /  64/*prescaler*/  /  1000/*targetfreq*/  -  1;
  // Turn on CTC mode (Clear Timer on Compare Match) WGM2[210]=010 (so we only set WGM21)
  TCCR2A = 1 << WGM21;
  // Select prescaler of 64 with Clock Select CS2[210]=100 (so we only set CS22)
  TCCR2B = 1 << CS22;   
  // Enable timer compare interrupt
  TIMSK2 = 1 << OCIE2A;
  interrupts(); // Re-enable all interrupts - sei()
}
