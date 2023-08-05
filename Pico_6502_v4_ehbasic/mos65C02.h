// 
// Author: Rien Matthijsse
// 

#ifndef _MOS65C02_h
#define _MOS65C02_h

#include "Arduino.h"

#define USE_DIRECT

void init6502();
void reset6502();
void tick6502(uint32_t);

#endif

