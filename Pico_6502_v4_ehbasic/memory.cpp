// 
// Author: Rien Matthijsse
// 
#include "memory.h"
#include "roms.h"
#include "vdu.h"
#include "sound.h"

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
/// <param name=address"</param>
/// <param name=data"</param>
/// <returns></returns>
void readmemory() {
  switch (address) {
  case SND_STAT:
    data = SoundQueueIsEmpty();
    break;

  default:
    data = mem[address];
    break;
  }
}

/// <summary>
/// store a byte into memory
/// </summary>
/// <param name="address"></param>
/// <param name="data"></param>
void writememory() {
  if (address == DSP) {
    // DSP register
    writeChar(data & 0x7F);
    mem[DSP] = 0x00;
  }
  else if (0xD020 <= address && address < 0xD040) { // VDU/SOUND controller
    switch (address) {
    case VDU_CMD: // VDU CMD
      setVDU(data);
      data = 0x00;
      break;

    case SND_CMD: // SOUND CMD
      setSound(data);
      data = 0x00;
      break;
    }

    mem[address] = data;
  }
  else if ((0x8000 <= address && address <= 0xCFFF) || (0xE000 <= address && address <= 0xFFF9)) { // exclude writing ROM
    Serial.printf("access violation [%04X]\n", address);
  }
  else
    mem[address] = data;
}
