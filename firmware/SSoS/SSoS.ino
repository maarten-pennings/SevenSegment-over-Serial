// SSoS.ino - Seven Segment over Serial firmware
#define APP_LONGNAME "Seven Segment over Serial"
#define APP_NAME     "SSoS"
#define APP_VERSION  3

#include "drv7s.h"
#include "font.h"

uint8_t app_fontid;
uint8_t app_cursor;

// `pattern`is a 8-bit code telling which LEDs of the 7-segment must be switched on (bit-0=segment-a, bit-1=segment-b, etc).
// The segment is identified by `app_cursor`. If the cursor is beyond the didplay, the display is first shifted one position ("scroll").
void app_putpattern(uint8_t pattern) {
  // Shift display if cursor is at end
  if( app_cursor>=DRV7S_UNITCOUNT ) {
    // The follwing three lines rely on DRV7S_UNITCOUNT==4
    drv7s_framebuf[0] = drv7s_framebuf[1];
    drv7s_framebuf[1] = drv7s_framebuf[2];
    drv7s_framebuf[2] = drv7s_framebuf[3];
    app_cursor = DRV7S_UNITCOUNT-1;
  } 
  // Determine pattern: lookup font
  // Write to driver
  drv7s_framebuf[app_cursor] = pattern;
  // Advance cursor
  app_cursor++;
}

// Writes the string `chars` to the display, starting from position 0, 
// clipping if longer than DRV7S_UNITCOUNT, padding with blanks if shorted than DRV7S_UNITCOUNT.
// Does not change `cursor`.
void app_putchars(const char *chars) {
  uint8_t i = 0;
  while( i<DRV7S_UNITCOUNT && *chars!=0 ) {
    drv7s_framebuf[i] = font_get(app_fontid,*chars);
    chars++;
    i++;
  }
  while( i<DRV7S_UNITCOUNT ) {
    drv7s_framebuf[i] = 0;  
    i++;
  }
}

void app_reset() {
  // Wipe screen
  memset(drv7s_framebuf,0,DRV7S_UNITCOUNT);
  
  drv7s_reset();
  Serial.println( "Driver settings");
  Serial.print  ( "  brightness    " ); Serial.print( drv7s_brightness_get() );  Serial.print( "/" );  Serial.println( DRV7S_SLOTCOUNT );
  Serial.print  ( "  blinking mode " ); Serial.println( drv7s_blinking_mode_get() );
  Serial.print  ( "  blinking hi   " ); Serial.print( drv7s_blinking_hi_get() ); Serial.println( "ms" );
  Serial.print  ( "  blinking lo   " ); Serial.print( drv7s_blinking_lo_get() ); Serial.println( "ms" );
  Serial.print  ( "  blinking mask " ); Serial.println( drv7s_blinking_mask_get( ),BIN );
  Serial.println();

  app_fontid = FONT_MIMIC7S; // FONT_UNIQUE7S;
  app_cursor = 0;
  Serial.println( "App settings");
  Serial.print  ( "  fontid        " ); Serial.println( app_fontid ); 
  Serial.print  ( "  cursor        " ); Serial.println( app_cursor ); 
}


void setup() {
  // Setup serial
  Serial.begin(115200);
  Serial.print("\n\n" APP_LONGNAME " (" APP_NAME ")\n\n");
  Serial.println( "Versions");
  Serial.print  ( "  app           " ); Serial.println( APP_VERSION );
  Serial.print  ( "  arduino       " ); Serial.println( ARDUINO );
  Serial.print  ( "  compiler      " ); Serial.println( __VERSION__ );
  Serial.print  ( "  compiled      " ); Serial.println( __DATE__ ", " __TIME__ );
  Serial.println();

  drv7s_setup();
  app_reset();

  // Welcome banner
  app_putchars(APP_NAME);
}

// Some chars below 0x20 are "commands" (eg BLINK-ENABLE).
// Some commands need one or more arguments (eg SET-FONT needs one more byte, the font id).
// The table below shows the length of each command plus arguments.
// So 0 indicates not-a-command, 1 indicates command-without-args, 2 indicates command-with-one byte arg, etc
// The table also shows the defauls (DFLT column), and actual ascii value in DEC/HEX/NAME/KEY and DESCRIPTION/
uint8_t app_cmd_len[0x20] = { // todo move SHOW_STRING out
//LEN     COMMAND            DFLT  SHOW-STRING DEC HEX NAME KEY ESC DESCRIPTION
    0, // ignored                                0  00  NUL  ^@  \0 Null
    2, // SET-FONT             01  Font __xx     1  01  SOH  ^A     Start of Heading
    2, // SET-BRIGHTNESS       03  brig __xx     2  02  STX  ^B     Start of Text
    2, // SET-BLINK-MASK       0F  blim xxxx     3  03  ETX  ^C     End of Text
    2, // SET-BLINK-TIMES      55  blit __xx     4  04  EOT  ^D     End of Transmission
    3, // SHOW-STRINGS         -                 5  05  ENQ  ^E     Enquiry
    1, // RESET                -                 6  06  ACK  ^F     Acknowledgement
    1, // BLINK-ENABLE        (11)               7  07  BEL  ^G  \a Bell (beep or the flash)
    1, // CURSOR-LEFT          -                 8  08  BS   ^H  \b Backspace
    1, // CURSOR-RIGHT         -                 9  09  HT   ^I  \t Horizontal Tab
    1, // LINE-COMMIT          -                10  0A  LF   ^J  \n Line Feed (moves line down)
    1, // BLINK-DISABLE        yes blie ___x    11  0B  VT   ^K  \v Vertical Tab
    1, // CLEAR-AND-HOME       yes              12  0C  FF   ^L  \f Form Feed (lcear the screen)
    1, // CURSOR-HOME          -                13  0D  CR   ^M  \r Carriage Return (start of line)
    1, // DOT-DISABLE         (15)              14  0E  SO   ^N     Shift Out
    1, // DOT-ENABLE           yes dot_ ___x    15  0F  SI   ^O     Shift In
    1, // MODE-CHAR            yes modc ___x    16  10  DLE  ^P     Data Link Escape
    1, // MODE-LINE           (16)              17  11  DC1  ^Q     Device Control 1 (often XON)
    2, // MODE-CHAR-TIME       50  modt __xx    18  12  DC2  ^R     Device Control 2
    2, // PATTERN-1            -                19  13  DC3  ^S     Device Control 3 (often XOFF)
    5, // PATTERN-ALL          -                20  14  DC4  ^T     Device Control 4
    0, // ignored                               21  15  NAK  ^U     Negative Acknowledgement
    0, // ignored                               22  16  SYN  ^V     Synchronous Idle
    0, // ignored                               23  17  ETB  ^W     End of Transmission Block
    0, // ignored                               24  18  CAN  ^X     Cancel
    0, // ignored                               25  19  EM   ^Y     End of Medium
    0, // ignored                               26  1A  SUB  ^Z     Substitute (windows: end-of-file)
    0, // ignored                               27  1B  ESC  ^[  \e   Escape (start of sequence)
    0, // ignored                               28  1C  FS   ^\     File Separator
    0, // ignored                               29  1D  GS   ^]     Group Separator
    0, // ignored                               30  1E  RS   ^^     Record Separator
    0, // ignored                               31  1F  US   ^_     Unit Separator
       //                          app_ ___x
       //                          IDE_ 1.8.16
       //                          bord _1.8.3
};

// Actual command in argv[0].
// Rest of arguments for that command (as specified in app_cmd_len[]) is available in argv.
// If app_cmd_len[ch]==0, this function will not be called with argv[0]==ch.
void app_cmd_exec(uint8_t * argv ) {
  Serial.print("exec ");
  for( uint8_t i=0; i<app_cmd_len[argv[0]]; i++ ) {
    Serial.print(argv[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
  uint8_t cmd = argv[0];
  if(         cmd==0x01 ) { // 2, SET-FONT             01  Font __xx     1  01  SOH  ^A     Start of Heading
    app_fontid = argv[1] % 2;
  } else  if( cmd==0x02 ) { // 2, SET-BRIGHTNESS       03  brig __xx     2  02  STX  ^B     Start of Text
    uint8_t val = ((argv[1]%20) + 3 ) % 4 + 1; // This allows \x01,1,A,a all act as brightness 1
    drv7s_brightness_set( val );
  } else  if( cmd==0x03 ) { // 2, SET-BLINK-MASK       0F  blim xxxx     3  03  ETX  ^C     End of Text
    drv7s_blinking_mask_set(argv[1]);
  } else  if( cmd==0x04 ) { // 2, SET-BLINK-TIMES      55  blit __xx     4  04  EOT  ^D     End of Transmission
    uint8_t hi = (argv[1]>>4) + 1;
    uint8_t lo = (argv[1]&0x0F) + 1;
    drv7s_blinking_hilo_set(hi*5,lo*5);
  } else  if( cmd==0x05 ) { // 3, SHOW-STRINGS         -                 5  05  ENQ  ^E     Enquiry
    // todo
  } else  if( cmd==0x06 ) { // 1, RESET                -                 6  06  ACK  ^F     Acknowledgement
    app_reset();
  } else  if( cmd==0x07 ) { // 1, BLINK-ENABLE        (11)               7  07  BEL  ^G  \a Bell (beep or the flash)
    drv7s_blinking_mode_set(1);
  } else  if( cmd==0x08 ) { // 1, CURSOR-LEFT          -                 8  08  BS   ^H  \b Backspace
    if( app_cursor>0 ) app_cursor--;
  } else  if( cmd==0x09 ) { // 1, CURSOR-RIGHT         -                 9  09  HT   ^I  \t Horizontal Tab
    if( app_cursor<DRV7S_UNITCOUNT ) app_cursor++;
  } else  if( cmd==0x0A ) { // 1, LINE-COMMIT          -                10  0A  LF   ^J  \n Line Feed (moves line down)
    // todo
  } else  if( cmd==0x0B ) { // 1, BLINK-DISABLE        yes blie ___x    11  0B  VT   ^K  \v Vertical Tab
    drv7s_blinking_mode_set(0);
  } else  if( cmd==0x0C ) { // 1, CLEAR-AND-HOME       yes              12  0C  FF   ^L  \f Form Feed (clear the screen)
  } else  if( cmd==0x0D ) { // 1, CURSOR-HOME          -                13  0D  CR   ^M  \r Carriage Return (start of line)
  } else  if( cmd==0x0E ) { // 1, DOT-DISABLE         (15)              14  0E  SO   ^N     Shift Out
  } else  if( cmd==0x0F ) { // 1, DOT-ENABLE           yes dot_ ___x    15  0F  SI   ^O     Shift In
  } else  if( cmd==0x10 ) { // 1, MODE-CHAR            yes modc ___x    16  10  DLE  ^P     Data Link Escape
  } else  if( cmd==0x11 ) { // 1, MODE-LINE           (16)              17  11  DC1  ^Q     Device Control 1 (often XON)
  } else  if( cmd==0x12 ) { // 2, MODE-CHAR-TIME       50  modt __xx    18  12  DC2  ^R     Device Control 2
  } else  if( cmd==0x13 ) { // 2, PATTERN-1            -                19  13  DC3  ^S     Device Control 3 (often XOFF)
  } else  if( cmd==0x14 ) { // 5, PATTERN-ALL          -                20  14  DC4  ^T     Device Control 4
  }
}

#define APP_CMD_BUFSIZE  5
#define APP_CMD_ARGC (app_cmd_len[app_cmd_args[0]])
uint8_t app_cmd_args[APP_CMD_BUFSIZE];
uint8_t app_cmd_argc;

void loop() {
  int ch= Serial.read();
  uint8_t ascii = ch & 0x7F;
  if( ch==-1 ) {
    // no char, skip
  } else if( app_cmd_argc < APP_CMD_ARGC ) {
    app_cmd_args[app_cmd_argc] = ch;
    app_cmd_argc++;
    if( app_cmd_argc==APP_CMD_ARGC ) {
      app_cmd_exec(app_cmd_args); // command has all args, exec
    }
  } else if( ascii<0x20 ) {
    if( app_cmd_len[ascii]>0 ) {
      app_cmd_args[0] = ascii;
      app_cmd_argc = 1;
      if( app_cmd_argc==APP_CMD_ARGC ) app_cmd_exec(app_cmd_args); // command needs no args, exec now
    }
  } else {
    app_putpattern( font_get(app_fontid,ch) );  
  }
}  
