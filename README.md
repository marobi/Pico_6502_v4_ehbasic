# Pico_6502_v4_ehbasic

this version is of the NEO6502 memulator supporting a 320x240 256 color display, dual buffered.
It will boot into Ehanced basic. You do not need to answer the Memory size question

In order to have the board work correctly, connect UEXT.3 to 6502BUS.40

on the 'console' you can use:

^R : reset the 6502 processor.

^L : toggle statistics output, currently the clock-speed is shown.

^D : dumps the 16 VDU registers.

^T : toggle trace of the input chars

The basic version has many extra commands to interact with the VDU/SOUND interface. See the docs
Also have a look at the demo basic-programs