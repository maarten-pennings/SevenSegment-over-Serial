// SSoS - proof of concept 2 - testing timers

// Arduino 101: Timers and Interrupts
// https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
// The ATmega168 has 3 timers: timer0, timer1 and timer2. 
// All timers depend on the system clock, normally that is 16MHz, check with `Serial.println(F_CPU)`. 
// Timer0 is an 8bit timer; in Arduino it is used for e.g. delay() and millis().
// Timer1 is a 16bit timer; in Arduino it seems to be used for the Servo library.
// Timer2 is an 8bit timer; in Arduino it seems to be used for tone().

// There is a fair bit of standardization by microchip.
// The timers have registers (n=0/1/2)
//   TCCRnA with bits      COMnA1 COMnA0 COMnB1 COMnB0 ----- -----  WGMn1  WGMn0
//   TCCRnB with bits      FOCnA  FOCnB  -----  -----  WGMn2 CSn2   CSn1   CSn0
//   TCNTn  bits not named
//   OCRnA  bits not named
//   OCRnB  bits not named
//   TIMSK2                -----  -----  -----  -----  ----- OCIEnB OCIEnA TOIEn
//   TIFR2                 -----  -----  -----  -----  ----- OCFnB  OCFnA  TOVn

// https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48P_88P_168P-DS40002065A.pdf?page=155
// See C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\iom168p.h

void setup() {
  // Setup serial
  Serial.begin(115200);
  while( ! Serial ) delay(200);
  Serial.println("\n\nSSoS - POC2 - timers");

  // Setup builtin LED (it is on D13 = PB5)
  DDRB  = 1<<5; // pin PB5 as output
  // Flash 5 times
  for( int i=0; i<5; i++ ) {
    PORTB |= 1<<5; // ON
    delay(100);  
    PORTB &= ~(1<<5); // OFF
    delay(100);
  }

  // Setup timer2
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

  Serial.print("F_CPU  "); Serial.println(F_CPU); // 16000000
  Serial.print("OCR2A  "); Serial.println(OCR2A); // 249
  Serial.print("ISR    "); Serial.print(F_CPU/64/(OCR2A+1)); Serial.println(" ms"); // 1000 ms
}

// ISR() is how you tell the compiler to associate a function with a specific interrupt source. 
// Function attachInterrupt() is the Arduino way: dynamic mapping, based on an ISR()
ISR( TIMER2_COMPA_vect ) {
  PORTB |= 1<<5; // ON
  PORTB &= ~(1<<5); // OFF
}



void loop() {
}
