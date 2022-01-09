// SSoS.ino - Seven Segment over Serial firmware
#define APP_LONGNAME "Seven Segment over Serial"
#define APP_NAME     "SSoS"
#define APP_VERSION  "1"

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
  // Determine pattern: lookup font, add dot if outside font
  uint8_t pattern = font_get(app_fontid)[ch%FONT_SIZE];
  if( ch>=FONT_SIZE  ) pattern |= 0x80; // switch dot on
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
  while( ! Serial ) delay(200);
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

void loop() {
  int ch= Serial.read();
  // todo: all escape sequence
  if( ch!=-1 ) {
    app_putch(ch);  
  }
}
