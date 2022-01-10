// SSoS.ino - Seven Segment over Serial firmware
#define APP_LONGNAME "Seven Segment over Serial"
#define APP_NAME     "SSoS"
#define APP_VERSION  2

#include "drv7s.h"
#include "font.h"

uint8_t app_fontid;
uint8_t app_cursor;
  
void app_putch(uint8_t ch) {
  // Shift display if cursor is at end
  if( app_cursor>=DRV7S_UNITCOUNT ) {
    // The follwing three lines rely on DRV7S_UNITCOUNT==4
    drv7s_framebuf[0] = drv7s_framebuf[1];
    drv7s_framebuf[1] = drv7s_framebuf[2];
    drv7s_framebuf[2] = drv7s_framebuf[3];
    app_cursor = DRV7S_UNITCOUNT-1;
  } 
  // Determine pattern: lookup font
  uint8_t pattern = font_get(app_fontid,ch);
  // Write to driver
  drv7s_framebuf[app_cursor] = pattern;
  // Advance cursor
  app_cursor++;
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

  app_fontid = FONT_MIMIC7S;
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
  Serial.print  ( "  driver        " ); Serial.println( DRV7S_VERSION );
  Serial.print  ( "  font          " ); Serial.println( FONT_VERSION );
  Serial.println();

  drv7s_setup();
  app_reset();

  // Welcome banner
  app_putch(APP_NAME[0]);
  app_putch(APP_NAME[1]);
  app_putch(APP_NAME[2]);
  app_putch(APP_NAME[3]);
}

// TODO: add 1 to all numargs
// TODO: use 0 for not in use
// TODO: numargs -~-> argc
uint8_t app_cmd_numargs[0x20] = {
       // DEC HEX NAME KEY ESC DESCRIPTION
  255, //   0  00  NUL  ^@  \0 Null
    1, //   1  01  SOH  ^A     Start of Heading
    4, //   2  02  STX  ^B     Start of Text
    0, //   3  03  ETX  ^C     End of Text
  255, //   4  04  EOT  ^D     End of Transmission
    1, //   5  05  ENQ  ^E     Enquiry
    0, //   6  06  ACK  ^F     Acknowledgement
    0, //   7  07  BEL  ^G  \a Bell (bell, beep sound or the screen flashing)
  255, //   8  08  BS   ^H  \b Backspace
  255, //   9  09  HT   ^I  \t Horizontal Tab
  255, //  10  0A  LF   ^J  \n Line Feed (moves one line down)
  255, //  11  0B  VT   ^K  \v Vertical Tab
  255, //  12  0C  FF   ^L  \f Form Feed (lcear the screen)
  255, //  13  0D  CR   ^M  \r Carriage Return (cursor to start of line)
  255, //  14  0E  SO   ^N     Shift Out
  255, //  15  0F  SI   ^O     Shift In
  255, //  16  10  DLE  ^P     Data Link Escape
  255, //  17  11  DC1  ^Q     Device Control 1 (often XON)
  255, //  18  12  DC2  ^R     Device Control 2
  255, //  19  13  DC3  ^S     Device Control 3 (often XOFF)
  255, //  20  14  DC4  ^T     Device Control 4
  255, //  21  15  NAK  ^U     Negative Acknowledgement
  255, //  22  16  SYN  ^V     Synchronous Idle
  255, //  23  17  ETB  ^W     End of Transmission Block
  255, //  24  18  CAN  ^X     Cancel
  255, //  25  19  EM   ^Y     End of Medium
  255, //  26  1A  SUB  ^Z     Substitute (windows: end-of-file)
  255, //  27  1B  ESC  ^[  \e Escape (start of sequence)
  255, //  28  1C  FS   ^\     File Separator
  255, //  29  1D  GS   ^]     Group Separator
  255, //  30  1E  RS   ^^     Record Separator
  255, //  31  1F  US   ^_     Unit Separator
};

void app_cmd_exec(uint8_t argc, uint8_t * argv ) {
  Serial.print("exec ");
  for( uint8_t i=0; i<argc; i++ ) {
    Serial.print(argv[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
  if( argv[0]==0x05 ) { // ENQ
    drv7s_blinking_mask_set(argv[1]);
  } else 
  if( argv[0]==0x06 ) { // ACK
    drv7s_blinking_mode_set(0);
  } else 
  if( argv[0]==0x07 ) { // BEL
    drv7s_blinking_mode_set(1);
  }
}

#define APP_CMD_BUFSIZE  5
uint8_t app_cmd_args[APP_CMD_BUFSIZE];
uint8_t app_cmd_argc;
uint8_t app_cmd_ix;

void loop() {
  int ch= Serial.read();
  uint8_t ascii = ch & 0x7F;
  if( ch==-1 ) {
    // no char, skip
  } else if( app_cmd_ix<app_cmd_argc ) {
    app_cmd_args[app_cmd_ix] = ch;
    app_cmd_ix++;
    if( app_cmd_ix==app_cmd_argc ) {
      app_cmd_exec(app_cmd_argc,app_cmd_args); // command has all args, exec
    }
  } else if( ascii<0x20 ) {
    if( app_cmd_numargs[ascii]==255 ) {
      // command not in use, skip
    } else {
      app_cmd_args[0] = ascii;
      app_cmd_ix = 1;
      app_cmd_argc = 1+app_cmd_numargs[ascii];
      if( app_cmd_argc==1 ) app_cmd_exec(app_cmd_argc,app_cmd_args); // command needs no args, exec now
    }
  } else {
    app_putch(ch);  
  }
}
