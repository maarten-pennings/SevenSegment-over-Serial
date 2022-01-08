# Firmware

The firmware for the Arduino Nano, the controller on the SSoS board.


## Proof of Concept 1 - SFRs

A first proof-of-concept Arduino [sketch](poc1) is available.
It tests
 - Direct SFR writes
 - Timing (5ms per unit cause no visible flicker, this is 20ms per frame, or 50Hz)
 - Receiving data to print via Serial
 - Having a font

Using SFRs (Special Function Registers) means accessing hardware registers
that control the IO pins. Instead of calling `digitalWrite()`, we call
`PORTC=...`. Not only saves this the function call overhead, more importantly,
with one write to PORTC, we set up to 8 IO pins in one go.

See the [datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48P_88P_168P-DS40002065A.pdf?page=102) at page 102.


## Proof of Concept 2 - Timer and ISR

A second proof-of-concept Arduino [sketch](poc2) is available.
It tests using a timer to fire an ISR every 1ms.

Notes:
 - The ATmega168 has 3 timers: timer0, timer1 and timer2. 
 - All timers depend on the system clock, normally that is 16MHz, but there is a pre-scaler.
 - Timer0 is an 8bit timer; in Arduino it is used for e.g. `delay()` and `millis()`.
 - Timer1 is a 16bit timer; in Arduino it seems to be used for the Servo library.
 - Timer2 is an 8bit timer; in Arduino it seems to be used for `tone()`. PoC2 uses this one.

There is a fair bit of standardization by microchip in naming.
The register names but also the bitfields in the registers are introduced in the 
[datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48P_88P_168P-DS40002065A.pdf?page=155) on page 155.
The names are also available in the Arduino header files.
In the case of the ATmega168, here is the header (on my windows PC)
`C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\iom168p.h`.

In the names below, replace `n` with the timer index (0, 1, or 2) - again, we use 2.
```
TCCRnA with bits      COMnA1 COMnA0 COMnB1 COMnB0 ----- -----  WGMn1  WGMn0
TCCRnB with bits      FOCnA  FOCnB  -----  -----  WGMn2 CSn2   CSn1   CSn0
TCNTn  bits not named
OCRnA  bits not named
OCRnB  bits not named
TIMSK2                -----  -----  -----  -----  ----- OCIEnB OCIEnA TOIEn
TIFR2                 -----  -----  -----  -----  ----- OCFnB  OCFnA  TOVn
```

With registers `TCCR2A` and `TCCR2B` we set the mode (Clear Timer on Compare Match: `WGM2=010`)
and the prescaler (64 with Clock Select: `CS2=100`). 
The `TCNT2` is the actual counter, we do not read or write it (but the hardware does).

With `OCR2A` we set the value to which we want the timer to count, generate and interrupt, and clear.
Since the sustem clock (`F_CPU`) is 16MHz, and the pre-scaler is 64, timer 2 is fed with a frequency of 250 000 Hz, so it will increment
every 4us. Therefore we need 250 increments for an interrupt every 1ms. Therefore we need to set `OCR2A` to 249 (because it starts counting at 0).

In `TIMSK2` we set `OCIE2A`, i.e. Interrupt Enable for Output Compare Match A.
This means that we have to install an ISR for ` TIMER2_COMPA_vect`.

See e.g. [Arduino 101: Timers and Interrupts](https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072).
or [instructables](https://www.instructables.com/Arduino-Timer-Interrupts/).

```c
void setup() {
  // Setup builtin LED (it is on D13 = PB5)
  DDRB  = 1<<5; // pin PB5 as output

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
}

ISR( TIMER2_COMPA_vect ) {
  PORTB |= 1<<5; // ON
  PORTB &= ~(1<<5); // OFF
}
```

The logic analyzer shows the PoC2 is successful:

![ISR 1ms](isr1ms.png)


(end)

