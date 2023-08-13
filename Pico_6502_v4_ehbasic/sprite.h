// sprite.h

#ifndef _SPRITE_h
#define _SPRITE_h

#include "Arduino.h"

#define MAX_SPRITES 32

#define SPRITE_BUFFER 0xD100

typedef struct _defsprite {
  uint16_t  x;
  uint16_t  y;
  uint16_t  w;
  uint16_t  h;
  int8_t    dx;
  int8_t    dy;
  uint8_t   mode;
  uint8_t   palette_id;
  uint8_t   color;
  uint8_t   buffer;
  uint8_t   state;
  union {
    uint8_t  b[8];
    uint64_t s;
  } cache;
} defSprite;


void initSprite();
void drawSprite();
void moveSprite();
void collSprite();

#endif

