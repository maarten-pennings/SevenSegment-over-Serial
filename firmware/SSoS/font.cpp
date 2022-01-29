// font.cpp - ASCII fonts for 7 segment displays

#include "Arduino.h"
#include "font.h"

// Font Unique7s maps all ASCII characters to a 7-segment display
// such that every ASCII character maps to a _unique_ display pattern. 
// It makes some characters hard to read (see uppercase S or lower case f).
static const uint8_t font_unique7s[0x80] PROGMEM = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // padding for 0x0_
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // padding for 0x1_
  //pgfedcba
  0b00000000, // 20 spc
  0b00101000, // 21 !
  0b00100010, // 22 "
  0b01100011, // 23 //
  0b01001001, // 24 $
  0b00100100, // 25 %
  0b01111110, // 26 &
  0b00000010, // 27 '
  0b00101001, // 28 (
  0b00001011, // 29 )
  0b00000001, // 2A *
  0b01000010, // 2B +
  0b00001100, // 2C ,
  0b01000000, // 2D -
  0b00010000, // 2E .
  0b01010010, // 2F /
  //pgfedcba
  0b00111111, // 30 0
  0b00000110, // 31 1
  0b01011011, // 32 2
  0b01001111, // 33 3
  0b01100110, // 34 4
  0b01101101, // 35 5
  0b01111101, // 36 6
  0b00000111, // 37 7
  0b01111111, // 38 8
  0b01101111, // 39 9
  0b00001001, // 3A :
  0b00001010, // 3B ;
  0b00100001, // 3C <
  0b01001000, // 3D =
  0b00000011, // 3E >
  0b01001011, // 3F ?
  //pgfedcba
  0b00111011, // 40 @
  0b01110111, // 41 A
  0b01111100, // 42 B
  0b00111001, // 43 C
  0b00011111, // 44 D
  0b01111001, // 45 E
  0b01110001, // 46 F
  0b00111101, // 47 G
  0b01110110, // 48 H
  0b00110000, // 49 I
  0b00011110, // 4A J
  0b01110101, // 4B K
  0b00111100, // 4C L
  0b01010101, // 4D M
  0b00110111, // 4E N
  0b00101111, // 4F O
  //pgfedcba
  0b01110011, // 50 P
  0b01101011, // 51 Q
  0b00110011, // 52 R
  0b00101101, // 53 S
  0b00101011, // 54 T
  0b00111110, // 55 U
  0b01110010, // 56 V
  0b01101010, // 57 W
  0b00110110, // 58 X
  0b01101110, // 59 Y
  0b00011011, // 5A Z
  0b00110001, // 5B [
  0b01100100, // 5C \ (\ shall not end line in C)
  0b00001110, // 5D ]
  0b00100011, // 5E ^
  0b00001000, // 5F _
  //pgfedcba
  0b00100000, // 60 `
  0b01011111, // 61 a
  0b01101100, // 62 b
  0b01011000, // 63 c
  0b01011110, // 64 d
  0b01111011, // 65 e
  0b01010001, // 66 f
  0b01100111, // 67 g
  0b01110100, // 68 h
  0b00000101, // 69 i
  0b00001101, // 6A j
  0b01111010, // 6B k
  0b00111000, // 6C l
  0b00010101, // 6D m
  0b01010100, // 6E n
  0b01011100, // 6F o
  //pgfedcba
  0b01010011, // 70 p
  0b01100101, // 71 q
  0b01010000, // 72 r
  0b00100101, // 73 s
  0b01111000, // 74 t
  0b00011100, // 75 u
  0b00110010, // 76 v
  0b00101010, // 77 w
  0b00010100, // 78 x
  0b00101110, // 79 y
  0b00010011, // 7A z
  0b01000110, // 7B {
  0b00000100, // 7C |
  0b01110000, // 7D }
  0b01000001, // 7E ~
  0b01011101  // 7F del
};


// Font LookAlike7s maps all ASCII characters to a 7-segment display
// such that is as close as possible to what the character normally looks like. 
// The downside is that one pattern may be used for several characters (see 5, uppercase S and lowercase s).
static const uint8_t font_lookalike7s[0x80] PROGMEM = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // padding for 0x0_
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // padding for 0x1_
  //pgfedcba
  0b00000000, // 20 spc
  0b00101000, // 21 !
  0b00100010, // 22 "
  0b01100011, // 23 //
  0b01001001, // 24 $
  0b00100100, // 25 %
  0b01111110, // 26 &
  0b00000010, // 27 '
  0b00111001, // 28 (
  0b00001111, // 29 )
  0b00000001, // 2A *
  0b01000010, // 2B +
  0b00001100, // 2C ,
  0b01000000, // 2D -
  0b00010000, // 2E .
  0b01010010, // 2F /
  //pgfedcba
  0b00111111, // 30 0
  0b00000110, // 31 1
  0b01011011, // 32 2
  0b01001111, // 33 3
  0b01100110, // 34 4
  0b01101101, // 35 5
  0b01111101, // 36 6
  0b00000111, // 37 7
  0b01111111, // 38 8
  0b01101111, // 39 9
  0b00001001, // 3A :
  0b00001010, // 3B ;
  0b01011000, // 3C <
  0b01001000, // 3D =
  0b01001100, // 3E >
  0b01001011, // 3F ?
  //pgfedcba
  0b00111011, // 40 @
  0b01110111, // 41 A
  0b01111100, // 42 B
  0b00111001, // 43 C
  0b01011110, // 44 D
  0b01111001, // 45 E
  0b01110001, // 46 F
  0b00111101, // 47 G
  0b01110110, // 48 H
  0b00110000, // 49 I
  0b00011110, // 4A J
  0b01110101, // 4B K
  0b00111100, // 4C L
  0b01010101, // 4D M
  0b00110111, // 4E N
  0b00111111, // 4F O
  //pgfedcba
  0b01110011, // 50 P
  0b01101011, // 51 Q
  0b00110011, // 52 R
  0b01101101, // 53 S
  0b01111000, // 54 T
  0b00111110, // 55 U
  0b01110010, // 56 V
  0b01101010, // 57 W
  0b00110110, // 58 X
  0b01101110, // 59 Y
  0b01011011, // 5A Z
  0b00111001, // 5B [
  0b01100100, // 5C \ (\ shall not end line in C)
  0b00001111, // 5D ]
  0b00100011, // 5E ^
  0b00001000, // 5F _
  //pgfedcba
  0b00100000, // 60 `
  0b01011111, // 61 a
  0b01111100, // 62 b
  0b01011000, // 63 c
  0b01011110, // 64 d
  0b01111011, // 65 e
  0b01110001, // 66 f
  0b01101111, // 67 g
  0b01110100, // 68 h
  0b00000101, // 69 i
  0b00001101, // 6A j
  0b01110101, // 6B k
  0b00111000, // 6C l
  0b01010101, // 6D m
  0b01010100, // 6E n
  0b01011100, // 6F o
  //pgfedcba
  0b01110011, // 70 p
  0b01100111, // 71 q
  0b01010000, // 72 r
  0b01101101, // 73 s
  0b01111000, // 74 t
  0b00011100, // 75 u
  0b01110010, // 76 v
  0b01101010, // 77 w
  0b00010100, // 78 x
  0b00101110, // 79 y
  0b01011011, // 7A z
  0b01000110, // 7B {
  0b00000110, // 7C |
  0b01110000, // 7D }
  0b01000001, // 7E ~
  0b01011101  // 7F del
};

static const uint8_t * const font_variants[] PROGMEM = { font_lookalike7s, font_unique7s };

uint8_t font_get(uint8_t id, uint8_t ch) {
  // We only have patterns for the lower half of the characters (below 0x80).
  // For the upper half, we add the dot of the 7-segment.
  const uint8_t * font = pgm_read_word(&font_variants[id]);
  return pgm_read_byte(&font[ch & 0x7F]) | (ch&0x80);
}
