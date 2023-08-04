// 
// 
// 

#include "ehbasic.h"

const uint8_t vectors_bin[] = {
	0xE3, 0xC9, // NMI
	0x43, 0xC9, // RESET
	0xCF, 0xC9  // IRQ/BRK
};


const uint8_t ehbasic_bin[] = {
#include "C:\cygwin64\home\Rien\pico\ehbasic.neo6502\basic.mon"

};
