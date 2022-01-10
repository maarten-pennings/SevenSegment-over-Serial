// font.h - interface to ASCII fonts for 7 segment displays
#ifndef __FONT__H__
#define __FONT__H__


// Version of the font definitions
#define FONT_VERSION  4


// Font IDs
#define FONT_UNIQUE7S 0 // ID for  font "Unique7s" (every ASCII character maps to a _unique_ display pattern)
#define FONT_MIMIC7S  1 // ID for  font "Mimic7s" (every ASCII character maps closest to normal way it looks)


// Pass one of the IDs above and a character. Returns
// Returns a pattern for a 7-segment display: bit 0 for segment a, bit 1 for b, ... bit 6 for g and bit 7 for p.
uint8_t font_get(uint8_t id, uint8_t ch);


#endif
