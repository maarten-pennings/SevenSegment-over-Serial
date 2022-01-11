// drv7s.h - interface to the 7-segment driver
#ifndef __DRV7S__H__
#define __DRV7S__H__

#define DRV7S_UNITCOUNT 4                                  // Number of 7-segment units
extern uint8_t drv7s_framebuf[DRV7S_UNITCOUNT];            // Desired content on the 7-segment units (each 1-bit is a segment that is on)

#define DRV7S_SLOTCOUNT 5                                  // Number of brightness slots
void    drv7s_brightness_set( uint8_t val );               // Sets the brightness level to `val`, iff 1<=val<=DRV7S_SLOTCOUNT.
uint8_t drv7s_brightness_get(  );                          // Returns current brightness level.

void    drv7s_blinking_mode_set( uint8_t enabled );        // Sets the blinking mode to `enabled` (0 is disabled, otherwise enabled).
uint8_t drv7s_blinking_mode_get(  );                       // Returns current blinking mode.

void    drv7s_blinking_hilo_set( uint8_t hi, uint8_t lo ); // Sets the blinking to `hi` frames "on", then `lo` frames "off", iff hi>=1 and lo>=1 and hi+lo<255.
uint8_t drv7s_blinking_hi_get(  );                         // Returns current blinking hi time (in frames).
uint8_t drv7s_blinking_lo_get(  );                         // Returns current blinking lo time (in frames).

void    drv7s_blinking_mask_set( uint8_t mask );           // Sets the blinking mask: if bit i is set, unit i will blink.
uint8_t drv7s_blinking_mask_get(  );                       // Returns current blinking mask.

void    drv7s_setup();                                     // Setup (hardware registers) for 7-segments
void    drv7s_reset();                                     // Resets state (framebuf:0,0,0,0; brightness:4; blinking:disabled,25/25,0b0000)

#endif
