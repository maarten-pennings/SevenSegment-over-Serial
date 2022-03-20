# Electronics - version 2

Seven-Segment over Serial (SSoS) module: a device consisting of four 7-segment display units, 
controllable via a serial link, to be usable as display for LEGO Mindstorms Robot Inventor or Spike Prime.

This section is about the second iteration of the electronics.
In this second iteration I tried to make the design smaller by integrating two boards: SSoS and Nano. 
The SSoS board is the board from the [first iteration](../electronics), 
and the Nano is a standard Arduino Nano.
The integrated board is nicknamed SSoSS.

The design made with [EasyEDA](https://easyeda.com/) and manufactured by [JLCPCB](https://jlcpcb.com/DMP),
who sources components from [LCSC](https://lcsc.com/).

For firmware, fonts, enclosure, user manual, LEGO code, see parent [page](..).

This article is under construction.


## Contents

There is

 - [schematic](Schematic_SSoSS.pdf)
 - [front](front-3D.png) view  
   ![front](front-3D.png)
 - [back](back-3D.png) view  
   ![back](back-3D.png)
 - all files for [ordering](order)


## Considerations

 - I used the ATMega328, not the ATM168 from SSoS, LCSC doesn't have the 168.
 - The ATMega328 on LCSC is very expensive ($10), so I only make two boards.
 - Do I have the correct oscillator (I need 8MHZ for ATMega328 at 3V3) - correct capacitance?
 - Is the ICMP header correct; can I program the ATMega328 (withe the bootloader, which fuse settings).
 - Are the transistor correct?
 - I will solder J2 (IDC connector) myself.
 - I will solder RA..RP myself (JLCPCB can only do one side), they are 0805.
 - Is the IDC/LEGO connector correctly pinned?
 - There is no cap on the RESET# pin.
 - No ground poor.
 - Are the ros/cols correctly connected to the ATMega328?
 
 
(end)
