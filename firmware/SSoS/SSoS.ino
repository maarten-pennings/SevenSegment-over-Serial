// SSoS.ino - Seven Segment over Serial firmware
#define APP_LONGNAME "Seven Segment over Serial"
#define APP_NAME     "SSoS"
#define APP_VERSION  "4.0"

#include "drv7s.h"
#include "font.h"


// State introduced by the app (driver also has some)
uint8_t app_fontid;
uint8_t app_cursor;
uint8_t app_dotenabled;
uint8_t app_charenabled;
uint8_t app_chartime10ms;


// Reset the complete state (driver and app)
void app_reset() {
  drv7s_reset();
  app_fontid = FONT_MIMIC7S; // FONT_UNIQUE7S;
  app_cursor = 0;
  app_dotenabled = 1;
  app_charenabled = 1;
  app_chartime10ms = 50;
}


// This is the "print to 7-segment", filling the framebuffer exposed by drv7s.
// `pattern`is a 8-bit code telling which LEDs of the 7-segment must be switched on (bit-0=segment-a, bit-1=segment-b, etc).
// The segment is identified by `app_cursor`. 
// If the cursor is beyond the display, the display is first shifted one position ("scroll").
void app_putpattern(uint8_t pattern) {
  // Shift display if cursor is at end
  if( app_cursor>=DRV7S_UNITCOUNT ) {
    for( uint8_t i=1; i<DRV7S_UNITCOUNT; i++ ) drv7s_framebuf[i-1] = drv7s_framebuf[i];
    app_cursor = DRV7S_UNITCOUNT-1;
  } 
  // Determine pattern: lookup font
  // Write to driver
  drv7s_framebuf[app_cursor] = pattern;
  // Advance cursor
  app_cursor++;
}


// Makes the dot or p-segment (of the character just before cursor) hi.
void app_putdot( ) {
  if( app_cursor>0 ) {
    drv7s_framebuf[app_cursor-1] |= 0x80;    
  }
}


// Writes the string `chars` to the display, starting from position 0, 
// clipping if longer than DRV7S_UNITCOUNT, padding with blanks if shorted than DRV7S_UNITCOUNT.
// Does not change `cursor`.
void app_putchars(const char *chars) {
  uint8_t i = 0;
  while( i<DRV7S_UNITCOUNT && *chars!=0 ) {
    if( i>0 && *chars=='.' ) {
      drv7s_framebuf[i-1] |= 0x80;
    } else {
      drv7s_framebuf[i] = font_get(app_fontid,*chars);
      i++;
    }
    chars++;
  }
  while( i<DRV7S_UNITCOUNT ) {
    drv7s_framebuf[i] = 0;  
    i++;
  }
}


// Returns chars s[p1..p2), but any `dot` character is replaced by a dot. Truncates at DRV7S_UNITCOUNT.
// Appends \0. Uses global buf, so one call at a time.
char app_string_buf[DRV7S_UNITCOUNT+2];
const char* app_slice(const char * s, uint8_t p1, uint8_t p2, char dot='\0' ) {
  uint8_t i=0;
  while( p1<p2 && i<DRV7S_UNITCOUNT+1 ) {
    uint8_t ch= s[p1++];
    if( ch==dot ) ch='.';
    app_string_buf[i++]= ch;
  }
  app_string_buf[i] = 0;
  return app_string_buf;
}


// Converts `val` to a string of the form 0xXX
// Appends \0. Uses global buf, so one call at a time.
const char* app_int(uint8_t val) {
  app_string_buf[0] = '0';
  app_string_buf[1] = 'x';
  uint8_t digit0 = val/16;
  app_string_buf[2] = (digit0<10?'0':'A'-10) + digit0;
  uint8_t digit1 = val%16;
  app_string_buf[3] = (digit1<10?'0':'A'-10) + digit1;
  app_string_buf[4] = '\0';
  return app_string_buf;
}


// String table usable to display all known strings (versions, driver state, app state)
#define APP_STRING_ID_COUNT 34
const char* app_string(uint8_t id) {
  // App versions, compiler, date/time
  if( id== 0 ) return "APP";
  if( id== 1 ) return APP_VERSION;
  
  if( id== 2 ) return "IDE";
  #if ARDUINO!=10813 // I was lazy here
    #assert Next lined needs its returned string to be updated
  #endif
  if( id== 3 ) return "1.8.13";

  if( id== 4 ) return "CC";
  if( id== 5 ) return __VERSION__;
  
  if( id== 6 ) return "YEAR";
  if( id== 7 ) return app_slice(__DATE__,7,11);

  if( id== 8 ) return "Mnth";
  if( id== 9 ) return app_slice(__DATE__,0,3);
  
  if( id==10 ) return "DAY";
  if( id==11 ) return app_slice(__DATE__,4,6);
  
  if( id==12 ) return "TIME";
  if( id==13 ) return app_slice(__TIME__,0,6,':');
  
  // Driver state
  if( id==14 ) return "BRIT";
  if( id==15 ) return app_int( drv7s_brightness_get() );
  
  if( id==16 ) return "BL.en";
  if( id==17 ) return app_int( drv7s_blinking_mode_get() );

  if( id==18 ) return "BL.hi";
  if( id==19 ) return app_int( drv7s_blinking_hi_get()/5 - 1 );

  if( id==20 ) return "BL.lo";
  if( id==21 ) return app_int( drv7s_blinking_lo_get()/5 - 1 );

  if( id==22 ) return "BL.mk";
  if( id==23 ) return app_int( drv7s_blinking_mask_get() );

  // App state
  if( id==24 ) return "FONT";
  if( id==25 ) return app_int(app_fontid);
  
  if( id==26 ) return "CUR";
  if( id==27 ) return app_int(app_cursor);
  
  if( id==28 ) return "DOT";
  if( id==29 ) return app_int(app_dotenabled);

  if( id==30 ) return "CH.en";
  if( id==31 ) return app_int(app_charenabled);

  if( id==32 ) return "CH.tm";
  if( id==33 ) return app_int(app_chartime10ms);

  return " N.A.";
}


// Some chars below 0x20 are "commands" (eg BLINK-ENABLE).
// Some commands need one or more arguments (eg SET-FONT needs one more byte, the font id).
// The table below shows the length of each command plus arguments.
// A length of 0 indicates not-a-command, 1 indicates command-without-args, 2 indicates command-with-one byte arg, etc
// The table also shows the defaults (DFLT column), and actual ascii value in DEC/HEX/NAME/KEY and DESCRIPTION/
uint8_t app_cmd_len[0x20] = { 
//LEN     COMMAND                  DFLT  DEC HEX NAME KEY ESC DESCRIPTION
    0, // ignored                          0  00  NUL  ^@  \0 Null
    2, // SET-FONT(0..1)             01    1  01  SOH  ^A     Start of Heading
    2, // SET-BRIGHTNESS(1..4)       03    2  02  STX  ^B     Start of Text
    2, // SET-BLINK-MASK(0..15)      0F    3  03  ETX  ^C     End of Text
    2, // SET-BLINK-TIMES(0..F|0..F) 55    4  04  EOT  ^D     End of Transmission
    3, // SHOW-STRINGS(from,to)      -     5  05  ENQ  ^E     Enquiry
    1, // RESET                      -     6  06  ACK  ^F     Acknowledgement
    1, // BLINK-ENABLE              (11)   7  07  BEL  ^G  \a Bell (beep or the flash)
    1, // CURSOR-LEFT                -     8  08  BS   ^H  \b Backspace
    1, // CURSOR-RIGHT               -     9  09  HT   ^I  \t Horizontal Tab
    1, // LINE-COMMIT                -    10  0A  LF   ^J  \n Line Feed (moves line down)
    1, // BLINK-DISABLE              yes  11  0B  VT   ^K  \v Vertical Tab
    1, // CLEAR-AND-HOME             yes  12  0C  FF   ^L  \f Form Feed (lcear the screen)
    1, // CURSOR-HOME                -    13  0D  CR   ^M  \r Carriage Return (start of line)
    1, // DOT-DISABLE               (15)  14  0E  SO   ^N     Shift Out
    1, // DOT-ENABLE                 yes  15  0F  SI   ^O     Shift In
    1, // CHAR-ENABLE                yes  16  10  DLE  ^P     Data Link Escape
    1, // CHAR-DISABLE              (16)  17  11  DC1  ^Q     Device Control 1 (often XON)
    2, // CHAR-TIME                  50   18  12  DC2  ^R     Device Control 2
    2, // PATTERN-ONE(pat)           -    19  13  DC3  ^S     Device Control 3 (often XOFF)
    5, // PATTERN-ALL(p0,p1,p2,p3)   -    20  14  DC4  ^T     Device Control 4
    0, // ignored                         21  15  NAK  ^U     Negative Acknowledgement
    0, // ignored                         22  16  SYN  ^V     Synchronous Idle
    0, // ignored                         23  17  ETB  ^W     End of Transmission Block
    0, // ignored                         24  18  CAN  ^X     Cancel
    0, // ignored                         25  19  EM   ^Y     End of Medium
    0, // ignored                         26  1A  SUB  ^Z     Substitute (windows: end-of-file)
    0, // ignored                         27  1B  ESC  ^[  \e   Escape (start of sequence)
    0, // ignored                         28  1C  FS   ^\     File Separator
    0, // ignored                         29  1D  GS   ^]     Group Separator
    0, // ignored                         30  1E  RS   ^^     Record Separator
    0, // ignored                         31  1F  US   ^_     Unit Separator
};


// Actual command in argv[0].
// Rest of arguments for that command (as specified in app_cmd_len[]) is available in rest of argv[].
// If app_cmd_len[ch]==0, this function will not be called with argv[0]==ch.
void app_cmd_exec(uint8_t * argv ) {
  uint8_t cmd = argv[0];
  if(         cmd==0x01 ) { // 2, SET-FONT
    app_fontid = argv[1] % 2;
  } else  if( cmd==0x02 ) { // 2, SET-BRIGHTNESS
    uint8_t val = constrain( argv[1]%16, 1, DRV7S_SLOTCOUNT); // This allows \x01,1,A,a all act as brightness 1
    drv7s_brightness_set( val );
  } else  if( cmd==0x03 ) { // 2, SET-BLINK-MASK
    drv7s_blinking_mask_set(argv[1]%16);
  } else  if( cmd==0x04 ) { // 2, SET-BLINK-TIMES
    uint8_t hi = (argv[1]>>4) + 1;
    uint8_t lo = (argv[1]&0x0F) + 1;
    drv7s_blinking_hilo_set(hi*5,lo*5);
  } else  if( cmd==0x05 ) { // 3, SHOW-STRINGS
    uint8_t id0 = argv[1];
    uint8_t id1 = argv[2];
    if( id1>APP_STRING_ID_COUNT ) id1=APP_STRING_ID_COUNT-1;
    Serial.println("Strings"); 
    for( uint8_t i=0; i<APP_STRING_ID_COUNT; i+=2 ) {
      Serial.print(" "); Serial.print(i); Serial.print(" "); Serial.print( app_string(i) ); Serial.print(" "); Serial.println( app_string(i+1) );
    }
    for( uint8_t id=id0; id<=id1; id++ ) { 
      app_putchars( app_string(id) ); delay(2000); 
    }
  } else  if( cmd==0x06 ) { // 1, RESET
    app_reset();
  } else  if( cmd==0x07 ) { // 1, BLINK-ENABLE
    drv7s_blinking_mode_set(1);
  } else  if( cmd==0x08 ) { // 1, CURSOR-LEFT
    if( app_cursor>0 ) app_cursor--;
  } else  if( cmd==0x09 ) { // 1, CURSOR-RIGHT
    if( app_cursor<DRV7S_UNITCOUNT ) app_cursor++;
  } else  if( cmd==0x0A ) { // 1, LINE-COMMIT
    Serial.println( "todo LINE-COMMIT");
  } else  if( cmd==0x0B ) { // 1, BLINK-DISABLE
    drv7s_blinking_mode_set(0);
  } else  if( cmd==0x0C ) { // 1, CLEAR-AND-HOME
    memset(drv7s_framebuf,0,DRV7S_UNITCOUNT); // clear screen
    app_cursor = 0; // cursor home
  } else  if( cmd==0x0D ) { // 1, CURSOR-HOME
    app_cursor = 0; // cursor home
  } else  if( cmd==0x0E ) { // 1, DOT-DISABLE
    app_dotenabled = 0;
  } else  if( cmd==0x0F ) { // 1, DOT-ENABLE
    app_dotenabled = 1;
  } else  if( cmd==0x10 ) { // 1, CHAR-ENABLE
    app_charenabled = 1;
  } else  if( cmd==0x11 ) { // 1, CHAR-DISABLE
    app_charenabled = 0;
  } else  if( cmd==0x12 ) { // 2, CHAR-TIME
    app_chartime10ms = argv[1];
  } else  if( cmd==0x13 ) { // 2, PATTERN-ONE
    // Spec-point: inserts a "raw" character so moves cursor
    app_putpattern( argv[1] );  
  } else  if( cmd==0x14 ) { // 5, PATTERN-ALL
    // Spec-point: fills display but does not change cusror
    for( uint8_t i=0; i<DRV7S_UNITCOUNT; i++ ) drv7s_framebuf[i] = argv[i+1];
  }
}


void setup() {
  // Setup serial
  Serial.begin(115200);
  Serial.println("\n\n" APP_LONGNAME " (" APP_NAME ")" );

  // Setup driver and app
  drv7s_setup();
  app_reset();

  // Welcome banner
  app_putchars(APP_NAME);
}


#define APP_CMD_BUFSIZE  5 // so that biggest command (PATTERN-ALL) fits
#define APP_CMD_ARGC     (app_cmd_len[app_cmd_argv[0]])
uint8_t app_cmd_argv[APP_CMD_BUFSIZE];
uint8_t app_cmd_argc;


void loop() {
  int ch= Serial.read();
  uint8_t ascii = ch & 0x7F;
  if( ch==-1 ) {
    // no char, skip
  } else if( app_cmd_argc < APP_CMD_ARGC ) {
    app_cmd_argv[app_cmd_argc] = ch;
    app_cmd_argc++;
    if( app_cmd_argc==APP_CMD_ARGC ) {
      app_cmd_exec(app_cmd_argv); // command has all args, exec
    }
  } else if( ascii<0x20 ) {
    if( app_cmd_len[ascii]>0 ) {
      app_cmd_argv[0] = ascii;
      app_cmd_argc = 1;
      if( app_cmd_argc==APP_CMD_ARGC ) app_cmd_exec(app_cmd_argv); // command needs no args, exec now
    }
  } else if( ascii=='.' && app_dotenabled ) {
    app_putdot( );  
  } else {
    app_putpattern( font_get(app_fontid,ch) );  
  }
}
