// 
// Author: Rien Matthijsse
// 
#include "memory.h"
#include "roms.h"

/// <summary>
/// 64k RAM
/// </summary>
uint8_t mem[MEMORY_SIZE];

// address and data registers
uint16_t address;
uint8_t  data;

/// <summary>
/// initialise memory
/// </summary>
void initMemory() {
  address = 0UL;
  data = 0;

  // lets install some ROMS
  if (loadROMS()) {
    Serial.println("ROMs installed");
  }
}

/// <summary>
/// read a byte from memory
/// </summary>
void readmemory() {
  data = mem[address];
}

/// <summary>
/// store a byte into memory
/// </summary>
void writememory() {
  if ((0xA000 <= address && address <= 0xCFFF) || (0xE000 <= address && address <= 0xFFF9)) { // exclude writing ROM
    Serial.printf("access violation [%04X]\n", address);
  }
  else
    mem[address] = data;
}
