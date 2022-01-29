// SSoS.ino - Seven Segment over Serial firmware
#define APP_LONGNAME "Seven Segment over Serial"
#define APP_NAME     "SSoS"
#define APP_VERSION  "5.4"
#define APP_WAIT_MS  2000


#include "drv7s.h"
#include "font.h"


////////////////////////////////////////////////////////////
// State maintained by the app
////////////////////////////////////////////////////////////


// State introduced by the app (driver also has some).
uint8_t app_fontid;
uint8_t app_cursor;
uint8_t app_dotenabled;
uint8_t app_charenabled;
uint8_t app_chartime20ms;


// Reset the complete state (driver and app).
void app_reset() {
  drv7s_reset(); // framebuf:0,0,0,0; brightness:4; blinking:disabled,25/25,0b0000
  app_fontid = FONT_LOOKALIKE7S; // FONT_UNIQUE7S;
  app_cursor = 0;
  app_dotenabled = 1;
  app_charenabled = 1;
  app_chartime20ms = 0x19; // 25*20=500ms
}


////////////////////////////////////////////////////////////
// Printing (updating display via driver)
////////////////////////////////////////////////////////////


// For char mode (if app_charenabled) function app_putpattern() puts to the real drv7s_framebuf[].
// For line mode (if not app_charenabled) function app_putpattern() puts to the app_linebuf[], 
// a \n is needed to commit (copy app_linebuf[] to drv7s_framebuf[]).
uint8_t app_linebuf[DRV7S_UNITCOUNT];


// This is the "print to 7-segment", filling the framebuffer exposed by drv7s or the line buffer, depending on app_charenabled.
// `pattern`is a 8-bit code telling which LEDs of the 7-segment must be switched on (bit-0=segment-a, bit-1=segment-b, etc).
// The segment is identified by `app_cursor`. 
// If the cursor is beyond the display, the display is first shifted one position ("scroll").
void app_putpattern(uint8_t pattern) {
  uint8_t * buf = app_charenabled ? drv7s_framebuf : app_linebuf;
  uint8_t wait = 0;
  // Shift display if cursor is at end
  if( app_cursor>=DRV7S_UNITCOUNT ) {
    for( uint8_t i=1; i<DRV7S_UNITCOUNT; i++ ) buf[i-1] = buf[i];
    app_cursor = DRV7S_UNITCOUNT-1;
    wait = app_charenabled;
  } 
  // Determine pattern: lookup font
  // Write to driver
  buf[app_cursor] = pattern;
  // Advance cursor
  app_cursor++;
  // In charmode, wait when there was a scroll
  if( wait ) delay(app_chartime20ms*20);
}


// Makes the dot or p-segment light up (of the character just before cursor).
void app_putdot( ) {
  if( app_cursor>0 ) {
    uint8_t * buf = app_charenabled ? drv7s_framebuf : app_linebuf;
    buf[app_cursor-1] |= 0x80;    
  }
}


// Used in line mode to copy the linebuffer to the framebuffer
void app_commit_line() {
  memcpy(drv7s_framebuf,app_linebuf,DRV7S_UNITCOUNT);
  memset(app_linebuf,0,DRV7S_UNITCOUNT);
  app_cursor = 0;
}


// Clears the screen and homes the cursor
void app_clear_home() {
  memset(drv7s_framebuf,0,DRV7S_UNITCOUNT); // clear 7-segments
  memset(app_linebuf,0,DRV7S_UNITCOUNT); // clear line buffer
  app_cursor = 0;
}


// Writes the string `chars` to the display, starting from position 0.
// Clips if longer than DRV7S_UNITCOUNT, pads with blanks if shorted than DRV7S_UNITCOUNT.
// Does have "dot enabled". Does not change `cursor`.
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


////////////////////////////////////////////////////////////
// Support SHOW-STRING
////////////////////////////////////////////////////////////


// Returns chars s[p1..p2), but any `dot` character is replaced by a dot. Truncates at DRV7S_UNITCOUNT.
// Appends \0. Uses global buffer, so one call at a time.
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


// Converts `val` to a string of the form 0xXX.
// Appends \0. Uses global buffer, so one call at a time.
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


// String table usable to display all known strings (versions, driver state, app state).
uint8_t app_framebuf_backup[DRV7S_UNITCOUNT];
#define APP_STRING_ID_COUNT 42
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
  if( id==19 ) return app_int( drv7s_blinking_hi_get() );

  if( id==20 ) return "BL.lo";
  if( id==21 ) return app_int( drv7s_blinking_lo_get() );

  if( id==22 ) return "BL.mk";
  if( id==23 ) return app_int( drv7s_blinking_mask_get() );

  if( id==24 ) return "DSP.0";
  if( id==25 ) return app_int( app_framebuf_backup[0] );
  
  if( id==26 ) return "DSP.1";
  if( id==27 ) return app_int( app_framebuf_backup[1] );

  if( id==28 ) return "DSP.2";
  if( id==29 ) return app_int( app_framebuf_backup[2] );

  if( id==30 ) return "DSP.3";
  if( id==31 ) return app_int( app_framebuf_backup[3] );

  // App state
  if( id==32 ) return "FONT";
  if( id==33 ) return app_int(app_fontid);
  
  if( id==34 ) return "CUR";
  if( id==35 ) return app_int(app_cursor);
  
  if( id==36 ) return "DOT";
  if( id==37 ) return app_int(app_dotenabled);

  if( id==38 ) return "CH.en";
  if( id==39 ) return app_int(app_charenabled);

  if( id==40 ) return "CH.tm";
  if( id==41 ) return app_int(app_chartime20ms);

  return " N.A.";
}

void app_show_strings(uint8_t id0, uint8_t id1) {
  // Clip
  if( id1>APP_STRING_ID_COUNT ) id1=APP_STRING_ID_COUNT-1;
  // Backup display
  memcpy(app_framebuf_backup,drv7s_framebuf,DRV7S_UNITCOUNT);
  // Dump all to serial  
  Serial.println("Strings"); 
  for( uint8_t i=0; i<APP_STRING_ID_COUNT; i+=2 ) {
    Serial.print(" "); Serial.print(app_int(i)); Serial.print(" "); Serial.print(app_string(i)); Serial.print(" "); Serial.println( app_string(i+1) );
  }
  // Show requested on display
  for( uint8_t id=id0; id<=id1; id++ ) { 
    app_putchars( app_string(id) ); delay(APP_WAIT_MS); 
  }
  // Restore framebuffer
  memcpy(drv7s_framebuf,app_framebuf_backup,DRV7S_UNITCOUNT);
}


////////////////////////////////////////////////////////////
// Handling characters that are commands (with arguments)
////////////////////////////////////////////////////////////


// Some chars below 0x20 are "commands" (eg BLINK-ENABLE).
// Some commands need one or more arguments (eg SET-FONT needs one more byte, the font id).
// The table below shows the length of each command plus arguments.
// A length of 0 indicates not-a-command, 1 indicates command-without-args, 2 indicates command-with-one byte arg, etc
// The table also shows the defaults (DFLT column), and actual ascii value in HEX/DEC/NAME/KEY and DESCRIPTION/
uint8_t app_cmd_len[0x20] = { 
//LEN     COMMAND                        DFLT HEX DEC NAME KEY ESC DESCRIPTION
    1, // RESET                                00   0  NUL  ^@  \0 Null
    2, // SET-FONT(0..1)                   01  01   1  SOH  ^A     Start of Heading
    2, // SET-BRIGHTNESS(1..5)             04  02   2  STX  ^B     Start of Text
    2, // SET-BLINK-MASK(0..F)             0F  03   3  ETX  ^C     End of Text
    3, // SET-BLINK-TIMES(0..FF,0..FF)  19,19  04   4  EOT  ^D     End of Transmission
    3, // SHOW-STRINGS(from,to)             -  05   5  ENQ  ^E     Enquiry
    1, // CURSOR-RIGHT                      -  06   6  ACK  ^F     Acknowledgment
    1, // BLINK-ENABLE                 see#0B  07   7  BEL  ^G  \a Bell (beep or the flash)
    1, // CURSOR-LEFT                       -  08   8  BS   ^H  \b Backspace
    1, // CURSOR-EOLN                       -  09   9  HT   ^I  \t Horizontal Tab
    1, // LINE-COMMIT                       -  0A  10  LF   ^J  \n Line Feed (moves line down)
    1, // BLINK-DISABLE                   yes  0B  11  VT   ^K  \v Vertical Tab
    1, // CLEAR-AND-HOME                  yes  0C  12  FF   ^L  \f Form Feed (clear the screen)
    1, // CURSOR-HOME                       -  0D  13  CR   ^M  \r Carriage Return (start of line)
    1, // DOT-DISABLE                  see#0F  0E  14  SO   ^N     Shift Out
    1, // DOT-ENABLE                      yes  0F  15  SI   ^O     Shift In
    1, // CHAR-ENABLE                     yes  10  16  DLE  ^P     Data Link Escape
    1, // CHAR-DISABLE                 see#10  11  17  DC1  ^Q     Device Control 1 (often XON)
    2, // CHAR-TIME                        19  12  18  DC2  ^R     Device Control 2
    2, // PATTERN-ONE(pat)                  -  13  19  DC3  ^S     Device Control 3 (often XOFF)
    5, // PATTERN-ALL(p0,p1,p2,p3)          -  14  20  DC4  ^T     Device Control 4
    0, // ignored                              15  21  NAK  ^U     Negative Acknowledgment
    0, // ignored                              16  22  SYN  ^V     Synchronous Idle
    0, // ignored                              17  23  ETB  ^W     End of Transmission Block
    0, // ignored                              18  24  CAN  ^X     Cancel
    0, // ignored                              19  25  EM   ^Y     End of Medium
    0, // ignored                              1A  26  SUB  ^Z     Substitute (windows: end-of-file)
    0, // ignored                              1B  27  ESC  ^[  \e Escape (start of sequence)
    0, // ignored                              1C  28  FS   ^\     File Separator
    0, // ignored                              1D  29  GS   ^]     Group Separator
    0, // ignored                              1E  30  RS   ^^     Record Separator
    0, // ignored                              1F  31  US   ^_     Unit Separator
};


// Actual command is in argv[0].
// Rest of arguments for that command (as specified in app_cmd_len[]) is available in rest of argv[].
// If app_cmd_len[ch]==0, this function will not be called with argv[0]==ch.
void app_cmd_exec(uint8_t * argv ) {
  uint8_t cmd = argv[0];
  if(        cmd==0x00 ) { // RESET (0x00/0)
    app_reset();
  } else if( cmd==0x01 ) { // SET-FONT (0x01/1 + font_id)
    app_fontid = argv[1] % 2;
  } else if( cmd==0x02 ) { // SET-BRIGHTNESS (0x02/2 + brightness_level)
    drv7s_brightness_set( argv[1]%16 ); // %16 allows \x01,1,A,a all act as brightness 1
  } else if( cmd==0x03 ) { // SET-BLINK-MASK (0x03/3 + blink_mask)
    drv7s_blinking_mask_set(argv[1]%16);
  } else if( cmd==0x04 ) { // SET-BLINK-TIMES (0x04/4 + hitime + lotime)
    drv7s_blinking_hilo_set(argv[1],argv[2]);
  } else if( cmd==0x05 ) { // SHOW-STRINGS (0x05/5 + from_index + to_index)
    app_show_strings(argv[1],argv[2]);
  } else if( cmd==0x06 ) { // CURSOR-RIGHT (0x06/6)
    if( app_cursor<DRV7S_UNITCOUNT ) app_cursor++;
  } else if( cmd==0x07 ) { // BLINK-ENABLE (0x07/7/\a)
    drv7s_blinking_mode_set(1);
  } else if( cmd==0x08 ) { // CURSOR-LEFT (0x08/8/\b)
    if( app_cursor>0 ) app_cursor--;
  } else if( cmd==0x09 ) { // CURSOR-EOLN (0x09/9/\t)
    app_cursor = DRV7S_UNITCOUNT;
  } else if( cmd==0x0A ) { // LINE-COMMIT (0x0A/10/\n)
    if( !app_charenabled ) app_commit_line();
  } else if( cmd==0x0B ) { // BLINK-DISABLE (0x0B/11/\v)
    drv7s_blinking_mode_set(0);
  } else if( cmd==0x0C ) { // CLEAR-AND-HOME (0x0C/12/\f)
    app_clear_home();
  } else if( cmd==0x0D ) { // CURSOR-HOME (0x0D/13/\r)
    app_cursor = 0; // cursor home
  } else if( cmd==0x0E ) { // DOT-DISABLE (0x0E/14)
    app_dotenabled = 0;
  } else if( cmd==0x0F ) { // DOT-ENABLE (0x0F/15)
    app_dotenabled = 1;
  } else if( cmd==0x10 ) { // CHAR-ENABLE (0x10/16)
    app_charenabled = 1;
  } else if( cmd==0x11 ) { // CHAR-DISABLE (0x11/17)
    app_charenabled = 0;
  } else if( cmd==0x12 ) { // CHAR-TIME (0x12/18 + char_time)
    app_chartime20ms = argv[1];
  } else if( cmd==0x13 ) { // PATTERN-ONE (0x13/19 + pattern)
    // Spec-point: inserts a "raw" character so moves cursor
    app_putpattern( argv[1] );  
  } else if( cmd==0x14 ) { // PATTERN-ALL (0x14/20 + pattern + pattern + pattern + pattern)
    // Spec-point: fills display but does not change cusror
    for( uint8_t i=0; i<DRV7S_UNITCOUNT; i++ ) drv7s_framebuf[i] = argv[i+1];
  }
}


////////////////////////////////////////////////////////////
// App setup and loop
////////////////////////////////////////////////////////////


// Enable Serial for incoming characters.
// Enable 7-Segment driver (timer based interrupt service routine)
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


// Some incoming characters are actually a command, optionally with some arguments (extra characters).
#define APP_CMD_BUFSIZE  5 // Size for command buffer; biggest command (PATTERN-ALL) must fit.
#define APP_CMD_ARGC     (app_cmd_len[app_cmd_argv[0]]) // The number of bytes required for the command in app_cmd_argv[0].
uint8_t app_cmd_argv[APP_CMD_BUFSIZE]; // At index 0 the command, next its arguments. Note on startup this hold command 0 with len 0.
uint8_t app_cmd_argc;  // Number of bytes for app_cmd_argv[0] that are already collected


// Process characters coming in from Serial. 
// Either send char to display with app_putpattern(), or treat it as a command app_cmd_exec().
void loop() {
  int ch= Serial.read();
  uint8_t ascii = ch & 0x7F;
  if( ch==-1 ) {
    // no char, skip
  } else if( app_cmd_argc < APP_CMD_ARGC ) { // command pending (collecting arguments)
    app_cmd_argv[app_cmd_argc] = ch;
    app_cmd_argc++;
    if( app_cmd_argc==APP_CMD_ARGC ) {
      app_cmd_exec(app_cmd_argv); // all arguments are collected for command, exec
    }
  } else if( ascii<0x20 ) { // incoming char is a (potentially) a command
    if( app_cmd_len[ascii]>0 ) {
      app_cmd_argv[0] = ascii;
      app_cmd_argc = 1;
      if( app_cmd_argc==APP_CMD_ARGC ) app_cmd_exec(app_cmd_argv); // command needs no arguments, exec now
    }
  } else if( ascii=='.' && app_dotenabled ) {
    app_putdot( );  
  } else {
    app_putpattern( font_get(app_fontid,ch) );  
  }
}
