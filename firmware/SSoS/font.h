// font.h - interface to ASCII fonts for 7 segment displays
#ifndef __FONT__H__
#define __FONT__H__

// Version of the font definitions
#define FONT_VERSION  3
#define FONT_SIZE     128

// ID for  font "Unique7s" (every ASCII character maps to a _unique_ display pattern)
#define FONT_UNIQUE7S 0

// ID for  font "Mimic7s" (every ASCII character maps closest to normal way it looks)
#define FONT_MIMIC7S  1

// Pass one of the IDs above to get a (pointer to a) font table
// A font table is an array of 128 bytes, mapping an ASCII character to a 7-segment pattern (8 LEDs, since there is also a dot)
uint8_t * font_get(uint8_t id);

#endif
