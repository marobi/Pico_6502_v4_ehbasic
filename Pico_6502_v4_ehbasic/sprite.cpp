// 
// 
// 
#include "sprite.h"
#include "vdu.h"
#include "memory.h"

defSprite Sprites[MAX_SPRITES];

extern uint32_t hasDisplayUpdate;

// Plot a pixel at x, y by exclusive-ORing the colour with the display
inline __attribute__((always_inline))
void xorPixel(const uint16_t x, const uint16_t y, const uint8_t color) {
  drawPixel(x, y, color ^ getPixel(x, y));
}

// Plot an 8x8 sprite at x, y by exclusive-ORing the colour with the display
inline __attribute__((always_inline))
void _xorSprite(const uint16_t x0, const uint16_t y0, const uint64_t sprite, const uint8_t color) {
  bool bit;
  if ((x0 >= 0) && (x0 + 7 < WIDTH) && (y0 >= 0) && (y0 + 7 < HEIGHT)) {
    for (uint8_t y = 0; y < 8; y++) {
      uint8_t yy = y * 8;
      for (uint8_t x = 0; x < 8; x++) {
        bit = sprite >> (63 - x -yy) & 1;
        if (bit) xorPixel(x0 + x, y0 + y, color);
      }
    }
  }
}

// Collision detection between an 8x8 sprite at x, y with a specified colour
inline __attribute__((always_inline))
bool _hitSprite(const uint16_t x0, const uint16_t y0, const uint64_t sprite, const uint8_t color) {
//  uint16_t row[8];
  uint8_t col8;
  bool bit;
  if ((x0 >= 0) && (x0 + 7 < WIDTH) && (y0 >= 0) && (y0 + 7 < HEIGHT)) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t x = 0; x < 8; x++) {
        col8 = getPixel(x0 + x, y0 + y);
        bit = sprite >> (63 - x - y * 8) & 1;
        if (bit && (col8 == color)) {
           return true;
        }
      }
    }
  }
  return false;
}

// Move a sprite from x, y by one pixel in any direction by exclusive-ORing only the changed pixels with the display
inline __attribute__((always_inline))
void _moveSprite(const uint16_t x0, const uint16_t y0, const uint64_t sprite, int8_t dx, int8_t dy, const uint8_t color) {
  uint8_t oldbit, newbit;
  if ((x0 >= 0) && (x0 + 7 < WIDTH) && (y0 >= 0) && (y0 + 7 < HEIGHT)) {
    dx = (dx > 0) - (dx < 0); dy = (dy > 0) - (dy < 0);
    for (uint8_t y = 0; y < 10; y++) {
      for (uint8_t x = 0; x < 10; x++) {
        // Sprite's previous position
        uint16_t xs = x - 1, ys = y - 1;
        if (xs >= 0 && xs <= 7 && ys >= 0 && ys <= 7) {
          oldbit = sprite >> (63 - ys * 8 - xs) & 1;
        }
        else oldbit = 0;
        // Sprite's new position
        uint16_t xn = x - 1 - dx, yn = y - 1 - dy;
        if (xn >= 0 && xn <= 7 && yn >= 0 && yn <= 7) {
          newbit = sprite >> (63 - yn * 8 - xn) & 1;
        }
        else newbit = 0;
        if (oldbit != newbit) xorPixel(x0 - 1 + x, y0 - 1 + y, color);
      }
    }
  }
}

/// <summary>
/// 
/// </summary>
/// <param name="sprite"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="color"></param>
void drawSprite() {
  uint16_t cx, cy, col;
  uint8_t id = mem[VDU_SID];
  defSprite* lsprite = &Sprites[id];

  cx = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
  cy = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;
  col = mem[VDU_COL];
  if (lsprite->state) {
    if ((lsprite->x != cx) || (lsprite->y != cy)) {
      _xorSprite(lsprite->x, lsprite->y, lsprite->cache.s, lsprite->color);
    }
    else if (lsprite->color != col) {
      _xorSprite(lsprite->x, lsprite->y, lsprite->cache.s, lsprite->color);
    }
  }
  lsprite->x = cx;
  lsprite->y = cy;
  lsprite->color = col;
  _xorSprite(cx, cy, lsprite->cache.s, col);
  lsprite->state = 1;
  hasDisplayUpdate++;
}

/// <summary>
/// 
/// </summary>
void moveSprite() {
  int8_t dx, dy;
  uint8_t id = mem[VDU_SID];
  defSprite* lsprite = &Sprites[id];

  if (mem[VDU_XH] > 0x80) dx = -1;
  else if (mem[VDU_XL] == 0) dx = 0;
  else dx = 1;

  if (mem[VDU_YH] > 0x80) dy = -1;
  else if (mem[VDU_YL] == 0) dy = 0;
  else dy = 1;

//  Serial.printf("MS: dx=%d, dy=%d, mode=%d\n", dx, dy, mem[VDU_MOD]);
  switch (mem[VDU_MOD]) {
  case 0x00:  // normal
    if (dx == 0 && dy == 0) {
      dx = lsprite->dx;
      dy = lsprite->dy;
    }
    break;
  case 0x01:  // multiply
    dx = dx * lsprite->dx;
    dy = dy * lsprite->dy;
    break;
  }

//  Serial.printf("dx=%d, dy=%d\n", dx, dy);

  _moveSprite(lsprite->x, lsprite->y, lsprite->cache.s, dx, dy, lsprite->color);
  lsprite->dx = dx;
  lsprite->dy = dy;
  lsprite->x += dx;
  lsprite->y += dy;
  hasDisplayUpdate++;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
void collSprite() {
  uint8_t id = mem[VDU_SID];
  defSprite* lsprite = &Sprites[id];
  uint8_t ret = 0x80;

  if (_hitSprite(lsprite->x + lsprite->dx, lsprite->y, lsprite->cache.s, currentColor))
    ret += 0x01;
  if (_hitSprite(lsprite->x, lsprite->y + lsprite->dy, lsprite->cache.s, currentColor))
    ret += 0x02;

//  Serial.printf("coll: %d %d %d %04x %0x\n", id, lsprite->x, lsprite->y, currentColor, ret);
  mem[VDU_RET] = ret;
}

/// <summary>
/// 
/// </summary>
/// <param name="id"></param>
void dumpSprite(uint8_t id) {
  defSprite* sprite;

  sprite = &Sprites[id];
  Serial.printf("SPRITE: %d X=%03d, Y=%03d, W=%03d, H=%03d, M=%01d, B=", id, sprite->x, sprite->y, sprite->w, sprite->h, sprite->mode);
  Serial.println(sprite->cache.s, HEX);

//  uint16_t m = SPRITE_BUFFER + sprite->buffer;
//  for (uint8_t i = 0; i < sprite->h; i++) {
//    Serial.printf("%04x: %02x ", m+i, mem[m+i]);
//    Serial.println(mem[m + i], BIN);
//  }
}

/// <summary>
/// 
/// </summary>
void initSprite() {
  defSprite *sprite;

  uint8_t id = mem[VDU_SID];

  sprite = &Sprites[id];

  sprite->w = (((uint16_t)mem[VDU_WH] * 256) + mem[VDU_WL]) % WIDTH;
  sprite->h = (((uint16_t)mem[VDU_HH] * 256) + mem[VDU_HL]) % HEIGHT;
  sprite->dx = 0;
  sprite->dy = 0;
  sprite->x = 0;
  sprite->y = 0;
  sprite->mode = mem[VDU_DMOD]; // for now XOR
  sprite->buffer = mem[VDU_PAL];
  sprite->color = 0xff; // white
  sprite->state = 0; // not visible
  
  // 8x8 1 cpp sprite
  for (uint8_t i = 0; i < sprite->h; i++) {
    sprite->cache.b[i] = mem[SPRITE_BUFFER + sprite->buffer + (7-i)];
  }
//  dumpSprite(id);
}
