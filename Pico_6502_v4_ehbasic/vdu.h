// vdu.h

#ifndef _VDU_h
#define _VDU_h

#include "Arduino.h"

#define FRAMERATE       10 // frames per sec

#define FRAMETIME     1000 / FRAMERATE  // msec
#define WIDTH          320
#define HEIGHT         240
#define FONT_CHAR_WIDTH  6  
#define FONT_CHAR_HEIGHT 8 
#define LINES       HEIGHT / (FONT_CHAR_HEIGHT + 1)
#define LINECHARS   WIDTH / FONT_CHAR_WIDTH


// VDU register definitions
#define VDU_CMD   0xD020  // CMD register
#define VDU_XL    0xD021  // X low
#define VDU_XH    0xD022  // X high
#define VDU_YL    0xD023  // Y low
#define VDU_YH    0xD024  // Y high
#define VDU_WL    0xD025  // Width low
#define VDU_WH    0xD026  // Width high
#define VDU_HL    0xD027  // Height low
#define VDU_HH    0xD028  // Height high

#define VDU_DMOD  0xD02B  // Draw mode - RECT :: fill-mode
#define VDU_MOD   0xD02C  // VDU mode
#define VDU_PAL   0xD02D  // PAL index
#define VDU_BCOL  0xD02E  // BG color -- TBI
#define VDU_COL   0xD02F  // FG color


// VDU commands -- TODO: reorg
#define CMD_NONE  0x00
#define CMD_CLS   0x01    // CLS
#define CMD_MOVE  0x02    // MOVE
#define CMD_COLOR 0x03    // COLOR
#define CMD_PIXEL 0x04    // PIXEL
#define CMD_DRAW  0x05    // DRAW
#define CMD_RECT  0x06    // RECT
#define CMD_CIRC  0x07    // CIRCLE
#define CMD_PAL   0x08    // PALETTE
#define CMD_VDU   0x09    // VDU
#define CMD_REFR  0x0A    // REFRESH
#define CMD_TRI   0x0B    // TRIANGLE
#define CMD_SANE  0xFF    // SANE


extern boolean autoUpdate;
extern uint32_t hasDisplayUpdate;
extern boolean  traceOn;


void scanChar();
void scanVDU();

void resetDisplay();
void initDisplay();
void helloDisplay();
void swapDisplay();

#endif

