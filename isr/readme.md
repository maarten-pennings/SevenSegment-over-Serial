# ISR model

Modeling the interrupt service routine (ISR), a part of the future firmware


## Model 1 - basic

Recall that the eight segments of the four 7-segment units are controlled in a row/column fashion.

The control cycle is as follows.
The 8 row lines are configured for unit 0, and then only the common of unit 0 is enabled.
Next, the 8 row lines are configured for unit 1, and only the common of unit 1 is enabled.
In the next step , the rows are configured for unit 2, and unit 2 is enabled.
Finally, the rows are configured for unit 3, and unit 3 is enabled.
Then control restarts with unit 0.

This means the the units are turned on one by one. 
When this happens in rapid succession, the human eye does not see the flickering.
It does mean that the units are off 3/4 of the time, which has impact on brightness.

Model 1 assumes a hardware timer triggers an interrupt service routine, which each time controls the next unit.
This guarantees no brightness variations due to timing variations.

This is the output of [model 1](isr1.py), the user displays the content `"ABCD"`.

```
user  : content 'ABCD'
system: frame is 4 ISR ticks of 5 ms, one per 7-segment unit

unit  : |0123|0123|0123|012
rows  : |ABCD|ABCD|ABCD|ABC
column: |0123|0123|0123|012

Every ISR tick, the rows and column of a (next) unit are powered
```

Each of the letters ABCD is shown 5ms, so a complete frame takes 20ms.
The unit/rows/column part is a log of the ISR; 
it shows for each tick what the rows and column is configured with.


## Model 2 - brightness

We want to support _brightness_ control.
This means that each unit should be on a fraction of the 5ms. 
To achieve this, we split the 5ms in 5 _slots_ of 1ms.

This is the output of [Model 2](isr2.py), the user selects a brightness of 2 out of 5.

```
user  : content 'ABCD', brightness 2/5
system: frame is 20 ISR ticks of 1 ms (5 brightness levels for 4 units)

slot  : |01234 01234 01234 01234|01234 01234 0
unit  : |00000 11111 22222 33333|00000 11111 2
rows  : |AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB C
column: |00--- 11--- 22--- 33---|00--- 11--- 2

The column is only on during 2 of 5 ISR ticks due to brightness setting
```

## Model 3 - blinking

We want to support _blinking_. 
Of course the host could send `"ABCD"`, `"    "`, `"ABCD"`, `"    "` etc, but that burdens the host.
We want to offload the host and have the SSoS device take care of blinking.


This is the output of [Model 3](isr3.py). 

```
user  : content 'ABCD', brightness 1/5, blink=3/5
system: frame is 20 ISR ticks of 1 ms (5 brightness levels for 4 units), blinking period is 5 frames

slot  : [01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234][01234
unit  : [00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333][00000
frame : [00000 00000 00000 00000|11111 11111 11111 11111|22222 22222 22222 22222|33333 33333 33333 33333|44444 44444 44444 44444][00000
rows  : [AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB CCCCC DDDDD|DDDDD DDDDD DDDDD DDDDD|DDDDD DDDDD DDDDD DDDDD][AAAAA
column: [0---- 1---- 2---- 3----|0---- 1---- 2---- 3----|0---- 1---- 2---- 3----|----- ----- ----- -----|----- ----- ----- -----][0----

For brightness, column is only on 1 of 5 ISRs; for blinking 3 of 5 frames
```

The user selected a blinking period of 5 frames, of which 0, 1 and 2 are on (and 3 and 4 off).
The user also has a brightness level of 1/5.

## Model 4 - blink exemption

We want to enable blinking for _only a part_ of the display.

This is the output of [Model 4](isr4.py). 
The user exempted units 0 and 2 (the `noblink` mask).

```
user  : content 'ABCD', brightness 4/5, blink=0b101-3/5
system: frame is 20 ISR ticks of 1 ms (5 brightness levels for 4 units), blinking period is 5 frames

slot  : [01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234|01234 01234 01234 01234][01234
unit  : [00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333|00000 11111 22222 33333][00000
frame : [00000 00000 00000 00000|11111 11111 11111 11111|22222 22222 22222 22222|33333 33333 33333 33333|44444 44444 44444 44444][00000
rows  : [AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB CCCCC DDDDD|AAAAA BBBBB CCCCC DDDDD|AAAAA AAAAA CCCCC CCCCC|AAAAA AAAAA CCCCC CCCCC][AAAAA
column: [0000- 1111- 2222- 3333-|0000- 1111- 2222- 3333-|0000- 1111- 2222- 3333-|0000- ----- 2222- -----|0000- ----- 2222- -----][0000-

For brightness, column is only on 4 of 5 ISRs; for blinking 3 of 5 frames, but on for non-blinking units in last 2 frames
```

Here we see that frame 3 and 4 (blink off) does not drive unit 1 and 3, but it does drive unit 0 and 2 (exempted).

(end)
