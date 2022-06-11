# Seven Segment over Serial (SSoS)
A device consisting of 7-segment display units, controllable via a serial link (with Arduino sketch and LEGO enclosure).

See a videos of the device in action.


[![video](video.jpg)](https://youtu.be/2fTZVQPiG7E) [![video](video.jpg)](https://www.youtube.com/watch?v=muLhuRukZes)

![logo](SSoS180x90.png)

![SSoS for LEGO](enclosure2/product-cable.jpg)

![Connected to the hub](enclosure2/product-hub.jpg)


## Introduction
I stumbled on this module [Seven Segment for micro:bit](http://www.monkmakes.com/mb_7_seg.html).
I immediately thought that such a thing would be a nice external display for the new LEGO Mindstorms Robot Inventor - it
does not have a display - at least not one that can display bigger numbers. But I want it smaller to better fit LEGO.

Later I found that [sparkfun](https://learn.sparkfun.com/tutorials/using-the-serial-7-segment-display/all) has something similar.
This board is also a bit too wide to my liking, and _retired_.


## Font
There is a [python script](font) to generate font tables. 
I designed two fonts, one _LookAlike7s_ with characters that are as close as possible to the real thing.
The second font _Unique7s_ maps each character to a unique pattern on the 7-segment display.


## Electronics
Design of the [electronics](electronics): the power architecture, display control, wiring (used pins), schematics, and finally PCB with IDC socket.
My project on EasyEDA is [public](https://oshwlab.com/maartenpennings/ssos).


## ISR model
Modeling the [interrupt service routine (ISR)](isr). The ISR needs to support brightness control and blinking.
The model is made in Python, but in the end, the ISR is the central part of the ATmega firmware, driving the 7-segments.


## Firmware
The [firmware](firmware) for the Arduino Nano, the controller on the SSoS board.
First some proof of concept sketches (using SFRs for ports, timer, ISR), finally the [end-product](firmware/SSoS).


## User manual
There is an extensive [user manual](manual) with examples (as implemented by the [firmware](firmware/SSoS)).


## LEGO enclosure 
I 3D-printed an [enclosure](enclosure) so that I can use SSoS with LEGO Mindstorms.


## LEGO demo
See the [LEGO code](legocode) for some examples of how to use the SSoS device with LEGO.

## Generation 2: Seven Segment over Serial Small
The (LEGO) enclosure we made is 4×8 LEGO units (studs). But LEGO Technic components usually have an _odd_ length.
So I set out to miniaturize making a Seven Segment over Serial _Small_ (SSoSS).
Target is 3×7. To achieve this, I made a single PCB, combining both the "Nano" and the 7-segment units.

![pcb2](electronics2/SSoSS-angle-back.jpg)

Section [electronics2](electronics2) shows the schematics and the unexpected firmware flashing problems.

Section [enclosure2](enclosure2) shows the new enclosure, 3D printed via [JLCPCB](https://jlcpcb.com/DMP).

(end)
