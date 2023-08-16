// tile.h

#ifndef _TILE_h
#define _TILE_h
#include "Arduino.h"

#define MAX_TILES 64

#define TILE_BUFFER 0xD200

typedef struct _deftile {
  uint16_t  w;
  uint16_t  h;
  uint8_t   palette_id;
  uint8_t   state; // 0 :: undef, 1 :: defined
  union {
    uint8_t  b[8];
    uint64_t s;
  } cache;
} defTile;

void initTiles();

void drawTile();

#endif

