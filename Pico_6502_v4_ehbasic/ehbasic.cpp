// 
// 
// 

#include "ehbasic.h"

const uint8_t vectors_bin[] = {
	0x6C, 0xCA, // NMI
	0xCB, 0xC9, // RESET
	0x58, 0xCA  // IRQ/BRK
};

//#include "C:\cygwin64\home\Rien\pico\ehbasic.neo6502\basic.mon"
const uint8_t ehbasic_bin[] = {
#include "C:\cygwin64\home\Rien\pico\ehbasic.neo6502\basic.mon"
};
