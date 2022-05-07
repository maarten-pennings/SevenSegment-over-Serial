SET AVRDUDE=C:\Users\maarten\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17
%AVRDUDE%\bin\avrdude -C%AVRDUDE%\etc\avrdude.conf -v -patmega328pb -cstk500v1 -PCOM5 -b19200 -Uflash:w:ledtest.ino.with_bootloader.eightanaloginputs.hex:i
