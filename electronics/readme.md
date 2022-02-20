# Electronics

Seven-Segment over Serial (SSoS) module: a device consisting of four 7-segment display units, 
controllable via a serial link, to be usable as display for LEGO Mindstorms Robot Inventor or Spike Prime.

This section is about the electronics of the SSoS board, 
design made with [EasyEDA](https://easyeda.com/) and manufactured by [JLCPCB](https://jlcpcb.com/).

For firmware, fonts, enclosure, user manual, LEGO code, see parent [page](..).


## Arduino Nano power architecture

The heart of SSoS module is an Arduino Nano. 
We need to power it from a LEGO hub port, which is 3V3.
Is that possible? 

The diagram below is a high level view of the 
[schematics](https://www.arduino.cc/en/uploads/Main/ArduinoNano30Schematic.pdf) 
of a Nano, focusing on power.

![Arduino Nano power architecture](power.png)

A Nano can be powered via
 - the `USB` connector;
 - 6 to 20V _unregulated_ external power supply (pin 30, `VIN`) - it is regulated internally (by V-reg); 
 - 5V _regulated_ external power supply (pin 27, `+5V`).

Note that
 - the `+3V3` (pin 17) is a low-current output supplied by the CH340;
 - the `+5V` (pin 27) can be used as output, provided the Nano is powered via `USB` or `VIN`;
 - `VUSB` is not pinned out;
 - the "power" LED is connected to the internal 5V rail;
 - a diode protects a USB host in case the Nano is inadvertently connected via `USB` and `VIN`/`+5V`;
 - according to the datasheet the operating voltage is 2.7V-5.5V for Atmel ATmega48/88/168.
 - this means that we can use pin 27 `+5V` also as input, provided we attach a _regulated_ power in the range 2.7V-5.5V.

**Conclusion**
We power the board by connecting pin 27 (`+5V`) to the external host (LEGO hub).
This is fine as long as the external host a "standard" regulated voltage (e.g. 3V3 or 5V). 
It also means that the logic pins, e.g. the incoming `RX` will have the host level.


## Display control

How does the Nano control the four seven-segment units?

According to the ATmega48/88/168 datasheet:
 - each I/O port can **source** more than the test conditions (20mA at VCC = 5V, **10mA** at VCC = 3V)
 - the sum of all IOH, for ports C0 - C5, D0- D4, ADC7, RESET should not exceed 150mA.
 - the sum of all IOH, for ports B0 - B5, D5 - D7, ADC6, XTAL1, XTAL2 should not exceed 150mA.

Similarly,
 - each I/O port can **sink** more than the test conditions (20mA at VCC = 5V, **10mA** at VCC = 3V)
 - the sum of all IOL, for ports C0 - C5, ADC7, ADC6 should not exceed 100mA.
 - the sum of all IOL, for ports B0 - B5, D5 - D7, XTAL1, XTAL2 should not exceed 100mA.
 - the sum of all IOL, for ports D0 - D4, RESET should not exceed 100mA.

So, sourcing 8 LEDs via 8 output pins is fine, as long as each LED gets 10mA max.

Sinking 8 LEDs via 1 output port is **not** ok, that would be 80mA where the max is 10mA.

**Conclusion**
We connect the four seven-segment displays in a row/column fashion. 
The individual segments ("rows") can be hooked directly to an output pin of the ATmega, provided that the current limiting resistor limits the LED current and thus the sourcing via the pin to 10mA.
The commons of the display ("columns"), need to sink, worst case, all 8 LED segments of a display unit. As a result, we use a transistor.
The principle is depicted below, for the real schematic see section below.

![Display control](col-row.png)

I decided to have current limiting resistors of 220 ohm for the LEDs.
Red LEDs have a forward voltage of Vf=1.8. 
With an external voltage of 3V3, the resistor drops 1.5V (3.3-1.8), leading to a current of 6.8mA for one segment and 55mA for all 8.
With an external voltage of 5V0, the resistor drops 3.2V, leading to a current of 14.5mA for one segment and 116mA for all 8.

| VCC | V_R (V)| I_R (mA) R=220Ω | I_common (mA) 8 LEDs |
|----:|-------:|----------------:|---------------------:|
| 3.3 |    1.5 |             6.8 |                   55 |
| 5.0 |    3.2 |            14.5 |                  116 |

I'm not completely sure about the dimension of the base resistor.

![Transistor](transistor.png)

If we assume I_C to be 55mA, and the gain (hFE, beta?) to be 100, then I_B is only 55mA/100 or 0.55mA.
There is a voltage drop across BE in the transistor, which is typically V_BE=0.7V. The emitter is grounded so V_B=0.7V.
The Arduino GPIO pin outputs VCC=3.3V, so we need a voltage drop over the base resistor from 3.3V to 0.7V or 2.6V.
With Ohm's law we find R = V_R/I_R = 2.6 / 0.55m = 4k7Ω.

| VCC | I_C (mA) | I_B (mA) β=100 | V_R (V) VCC-V_B | R (Ω) V_R/I_B) |
|----:|---------:|---------------:|----------------:|---------------:|
| 3.3 |       55 |           0.55 |            2.6  |           4727 |
| 5.0 |      116 |           1.16 |            4.3  |           3706 |

So, 2k2 seems save to make sure the transistor is saturated...


## Wiring

We will use the pins as marked in the dark blue bars. 
Note that we have 6+6 row/column pins, which will be used to drive 8 columns and 4 rows.
We tried to group these in so-called ports so that a single SFR assignment suffices.

![Wiring](pinout.png)

The breadboard prototype works, but the LEGO mindstorms 3V3 is on the edge.

![Breadboard](breadboard.jpg)


## PCB production

I used [EasyEDA](https://easyeda.com) to design a PCB.
My project is [public](https://oshwlab.com/maartenpennings/ssos).
First step was to make [schematics](Schematic_SSoS.pdf).

I intend to use SSoS for LEGO Mindstorms.
The output voltage is a bit low, so I decided to reserve room for an optional [boost converter](https://www.aliexpress.com/item/32762622485.html).
The host always connects to J1.
 - Host has regulated power in the range of 3-6V: 
   connect **VR**egulated, **G**round, **S**erial of J1.
 - Otherwise mount buck or boost regulator on J2 and 
   connect **G**round, **S**erial, **VU**nregulated of J1

Next comes layout in [EasyEDA](https://easyeda.com).
I wanted a small board, so used SMDs for the resistors, and populated the board on both sides.

Note that soldering will be a bit tricky: the Nano and seven-segment hold each other in a "brace".
Here the renders of the top and bottom of the PCB.

![PCB rendered top](SSoS-top.png)

![PCB rendered bottom](SSoS-bottom.png)

The final step is to generate the [gerber](Gerber_SSoS.zip) file.
I sent those to [JLCPCB](https://jlcpcb.com/).
For small quantities JLCBCP is great value for money: 
five PCBs for €1.77 plus €3.84 shipping (3 weeks to Europe).

This is what came back - I tried the new PCB color purple :-)

![PCB actual top](pcb-front.jpg)

![PCB actual bottom](pcb-back.jpg)

The result after mounting the components (you can't even see the SMDs anymore).

![Module actual top](module-front3.jpg)

![Module actual bottom](module-back3.jpg)

Possible improvements:

- I should have rotated the Nano board (USB connector on the outside).
- I should have made better provisions for the IDC connector towards the LEGO Mindstorms hub.
- I maybe should have spaced the transistors to match holes for LEGO Technic pins.
- This design is now 8×4×2 studs, which does not match LEGO's philosophy of having an odd number of studs in the Technic world.


## IDC connector

I always confuse myself with the pin-out of the connectors: male and female, front and back view.

This is the LEGO hub pinout.

![hub pinout](pinout-hub.jpg)

I have one LEGO (power functions 2, powered up, spike prime, robot inventor, control +) 
to IDC cable, I use [2×3 6P](https://www.aliexpress.com/item/4001257530318.html).

![IDC cable](LEGO-IDC-cable.jpg)

Here is an example of matching [2×3 6P](https://www.aliexpress.com/item/1005001400147026.html) socket.
The SSoS module needs this wiring on the IDC socket. 

![IDC pinout](pinout-IDC.jpg)

This is the PCB end-result - with the socket glued...

![Module actual bottom](module-idc.jpg)


## Final product

Elsewhere in the [project](..), you find the firmware for the Nano, and the STL for the LEGO case.
This is how the encased PCB-with-cable looks like.

![encased](../enclosure/assembled-cable.jpg)

Here the SSoS module is in [action](https://youtu.be/2fTZVQPiG7E) in a simple robot.

![motor](../legocode/motor.jpg)

## References 

 - [This project](https://oshwlab.com/maartenpennings/ssos)
 - [Arduino Nano description](https://www.arduino.cc/en/pmwiki.php?n=Main/ArduinoBoardNano)
 - [Arduino Nano schematics](https://www.arduino.cc/en/uploads/Main/ArduinoNano30Schematic.pdf)
 - [ATmega168 information (and datasheet)](https://www.microchip.com/en-us/product/ATmega168)
 - [LEGO connector on Philo's site](https://www.philohome.com/wedo2reverse/connect.htm)
 - [EasyEDA](https://easyeda.com)
 - [JLCPCB](https://jlcpcb.com/)
 - [AliExpress](https://www.aliexpress.com/)
    - [Nano](https://www.aliexpress.com/item/32969876875.html)
    - [LEGO cable](https://www.aliexpress.com/item/32871195908.html)
    - [7 segment common cathode](https://www.aliexpress.com/item/33058771440.html)
    - [IDC plug 2×3 6P](https://www.aliexpress.com/item/4001257530318.html)
    - [IDC socket 2×3 6P](https://www.aliexpress.com/item/1005001400147026.html)
    - [SMD resistors](https://www.aliexpress.com/item/32865947306.html)
    - [Capacitor](https://www.aliexpress.com/item/1005002842732971.html)
    - [Transistor](https://www.aliexpress.com/item/1005003363864621.html)

(end)
