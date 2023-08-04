// 
// Author: Rien Matthijsse
// 

#ifndef _MEMORY_h
#define _MEMORY_h

#include "Arduino.h"

#define MEMORY_SIZE  0x10000 // 64k

#define KBD   0xd010
#define DSP   0xd012

extern uint8_t  mem[];
extern uint16_t address;
extern uint8_t  data;

//externally supplied functions
void initMemory();

void readmemory();

void writememory();

#endif

