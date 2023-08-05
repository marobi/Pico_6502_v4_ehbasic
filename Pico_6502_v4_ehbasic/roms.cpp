// 
// Author: Rien Matthijsse
// 

#include "roms.h"

#include "memory.h"
#include "ehbasic.h"

#define MAX_ROMS  4
#define ROM_FOLDER  "ROM"

// ROM image:
typedef struct _sROMImage {
  char     name[32];
  uint16_t startAddress;
  uint16_t romsize;
  const uint8_t  *image;
} sROMImage;


/// <summary>
///  the ROM images to load
/// </summary>
sROMImage ROMs[] = {
  { "Ehbasic v2.22p5a", EHBASIC_START, EHBASIC_SIZE, ehbasic_bin },
  { "Vectors",          0xFFFA,        0X0006,       vectors_bin },
  { "", 0x0000, 0x0000 }
};

/// <summary>
/// 
/// </summary>
/// <param name="vROMname"></param>
/// <returns></returns>
uint8_t* readROM(String vROMname) {
 // TBI

  return 0;
}

/// <summary>
/// 
/// </summary>
/// <param name="vROM"></param>
/// <param name="startAddress"></param>
/// <param name="romSize"></param>
/// <returns></returns>
bool loadROM(const uint8_t *vROM, uint16_t startAddress, uint16_t romSize) {
  // copy ROM in memory space
  memcpy(&mem[startAddress], vROM, romSize);
//  for (uint16_t i = 0; i < romSize; i++) {
//    mem[i + startAddress] = vROM[i];
//  }

  return true;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
bool loadROMS() {
  for (uint8_t i = 0; i < MAX_ROMS; i++) {
    if (strcmp(ROMs[i].name, "") != 0) {
      Serial.printf("ROM: %16s\t@0X%04X\t[0X%04x]\n", ROMs[i].name, ROMs[i].startAddress,ROMs[i].romsize);
      loadROM(ROMs[i].image, ROMs[i].startAddress, ROMs[i].romsize);
    }
    else
      break;
  }

  return true;
}
