// 
// 
// 
#include <PicoDVI.h>

#include "memory.h"
#include "vdu.h"
#include "palette.h"
#include "sprite.h"

boolean        statusCursor = true;
uint8_t        currentColor;
uint8_t        currentColorIndex = 0;
boolean        autoUpdate = true;
boolean        autoScroll = true;
uint32_t       hasDisplayUpdate = 0;

boolean        traceOn = false;

// Pico HDMI for Olimex Neo6502 
static const struct dvi_serialiser_cfg pico_neo6502_cfg = {
  .pio = DVI_DEFAULT_PIO_INST,
  .sm_tmds = {0, 1, 2},
  .pins_tmds = {14, 18, 16},
  .pins_clk = 12,
  .invert_diffpairs = true
};

// Here's how an 320x240 256 colors graphics display is declared.
DVIGFX8 display(DVI_RES_320x240p60, true, pico_neo6502_cfg);


/// <summary>
/// 
/// </summary>
/// <param name="color_id"></param>
/// <returns></returns>
uint16_t getColor(const uint8_t color_id) {
  return display.getColor(color_id);
}

/// <summary>
/// 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
uint8_t getPixel(const uint16_t x, const uint16_t y) {
  return display.getPixel(x, y);
}

/// <summary>
/// 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="color"></param>
void drawPixel(const uint16_t x, const uint16_t y, const uint16_t color) {
  display.drawPixel(x, y, color);
}

/// <summary>
/// control visibility of cursor
/// </summary>
/// <param name="vSet"></param>
inline __attribute__((always_inline))
void showCursor(const boolean vSet) {
  statusCursor = vSet;
}

/// <summary>
/// set current text color (by pallette index)
/// </summary>
/// <param name="vColor"></param>
inline __attribute__((always_inline))
void setColor(const uint8_t vColor) {
  currentColor = vColor;
  display.setTextColor(vColor);
}

/// <summary>
/// 
/// </summary>
inline __attribute__((always_inline))
void setCursor() {
  int16_t cursor_x, cursor_y;
  // remove optional cursor
  if (statusCursor) {
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    display.fillRect(cursor_x, cursor_y + FONT_CHAR_HEIGHT - 2, FONT_CHAR_WIDTH, 2, currentColor); // show cursor
  }
}

/// <summary>
/// 
/// </summary>
inline __attribute__((always_inline))
void removeCursor() {
  int16_t cursor_x, cursor_y;
  // remove optional cursor
  if (statusCursor) {
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    display.fillRect(cursor_x, cursor_y, FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT, 0); // remove cursor
  }
}

/// <summary>
/// 
/// </summary>
inline __attribute__((always_inline))
void clearDisplay() {
  display.fillScreen(0);
  display.setCursor(0, 0);
  hasDisplayUpdate++;
}

/// <summary>
/// performa a action on the display
/// </summary>
/// <param name="vCmd"></param>
inline __attribute__((always_inline))
void setVDU(const uint8_t vCmd) {
  int16_t cx, cy, ex, ey;
  uint16_t cw, ch, cr;

  switch (vCmd) {
  case CMD_CLS: // clearscreen
    clearDisplay();
    break;

  case CMD_MOVE: // set cursor
    cx = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
    cy = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;
    display.setCursor(cx, cy);
    hasDisplayUpdate++;
    break;

  case CMD_COLOR: // set color
    setColor(mem[VDU_COL]);
    break;

  case CMD_PIXEL: // set pixel (ignore screen mode)
    cx = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
    cy = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;

    display.drawPixel(cx, cy, currentColor);
    hasDisplayUpdate++;
    break;

  case CMD_DRAW: // draw line
    cx = display.getCursorX();
    cy = display.getCursorY();
    ex = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
    ey = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;

    display.drawLine(cx, cy, ex, ey, currentColor);
    display.setCursor(ex, ey);
    hasDisplayUpdate++;
    break;

  case CMD_RECT:
    cx = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
    cy = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;
    cw = (((uint16_t)mem[VDU_WH] * 256) + mem[VDU_WL]) % WIDTH;
    ch = (((uint16_t)mem[VDU_HH] * 256) + mem[VDU_HL]) % HEIGHT;

    //    Serial.printf("RECT %04x %04x %04x %04x\n", cx, cy, cw, ch);
    switch (mem[VDU_DMOD]) {
    case 0:
      display.drawRect(cx, cy, cw, ch, currentColor);
      break;
    default:
      display.fillRect(cx, cy, cw, ch, currentColor);
      break;
    }

    hasDisplayUpdate++;
    break;

  case CMD_CIRC:
    cx = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
    cy = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;
    cr = (((uint16_t)mem[VDU_WH] * 256) + mem[VDU_WL]) % WIDTH;

    //    Serial.printf("CIRC %04x %04x %04x\n", cx, cy, cr);
    switch (mem[VDU_DMOD]) {
    case 0:
      display.drawCircle(cx, cy, cr, currentColor);
      break;
    default:
      display.fillCircle(cx, cy, cr, currentColor);
      break;
    }

    hasDisplayUpdate++;
    break;

  case CMD_TRI:
    cx = (((uint16_t)mem[VDU_XH] * 256) + mem[VDU_XL]) % WIDTH;
    cy = (((uint16_t)mem[VDU_YH] * 256) + mem[VDU_YL]) % HEIGHT;
    ex = (((uint16_t)mem[VDU_WH] * 256) + mem[VDU_WL]) % WIDTH;
    ey = (((uint16_t)mem[VDU_HH] * 256) + mem[VDU_HL]) % WIDTH;

    //    Serial.printf("TRI %04x %04x %04x %04x\n", cx, cy, ex, ey);
    switch (mem[VDU_DMOD]) {
    case 0:
      display.drawTriangle(display.getCursorX(), display.getCursorY(), cx, cy, ex, ey, currentColor);
      break;
    default:
      display.fillTriangle(display.getCursorX(), display.getCursorY(), cx, cy, ex, ey, currentColor);
      break;
    }

    hasDisplayUpdate++;
    break;

  case CMD_PAL:
    display.setColor(mem[VDU_PAL], ((uint16_t)mem[VDU_BCOL] * 256) + mem[VDU_COL]);
    display.swap(false, true);
    break;

  case CMD_VDU: //  VDU control
    switch (mem[VDU_MOD] & 0x01) {
    case 1:
      showCursor(false);
      break;
    default:
      showCursor(true);
      break;
    }

    autoUpdate = !(mem[VDU_MOD] >> 7);
    autoScroll = !((mem[VDU_MOD] >> 1) & 0x01);
    break;

  case CMD_REFR: // REFRESH
    swapDisplay();
    break;

  case CMD_SANE:
    resetDisplay();
    break;

  case CMD_SPRITE:
    initSprite();
    break;

  case CMD_SDRAW:
    drawSprite();
    break;

  case CMD_SMOVE:
    moveSprite();
    break;

  case CMD_COLL:
    collSprite();
  }
}

/// <summary>
/// 
/// </summary>
void scanVDU() {
  uint8_t cmd = mem[VDU_CMD];

  if (cmd != 0x00) {
    setVDU(cmd);
    mem[VDU_CMD] = 0x00;
  }
}

/// <summary>
/// 
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
inline __attribute__((always_inline))
void displayWrite(const uint8_t c) {
  int16_t cursor_x, cursor_y;

  cursor_x = display.getCursorX();
  cursor_y = display.getCursorY();

  removeCursor();
  if (c == '\n') { // Carriage return
    cursor_x = 0;
  }
  else if ((c == '\r') || (cursor_x >= WIDTH)) { // Newline OR right edge
    cursor_x = 0;
    if (cursor_y >= (HEIGHT - 9)) { // Vert scroll?
      if (autoScroll) {
        memmove(display.getBuffer(), display.getBuffer() + WIDTH * (FONT_CHAR_HEIGHT + 1), WIDTH * (HEIGHT - (FONT_CHAR_HEIGHT + 1)));
        display.drawFastHLine(0, HEIGHT - 9, WIDTH, 0); // Clear bottom line
        display.fillRect(0, HEIGHT - 9, WIDTH, FONT_CHAR_HEIGHT + 1, 0);

        cursor_y = HEIGHT - 9;
      }
    }
    else {
      cursor_y += FONT_CHAR_HEIGHT + 1;
    }

    display.setCursor(cursor_x, cursor_y);
  }

  switch (c) {
  case '\r':
  case '\n':
    break;
  case 0x08:
  case 0x7F:
    display.fillRect(cursor_x - FONT_CHAR_WIDTH, cursor_y, FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT, 0);
    display.setCursor(cursor_x - FONT_CHAR_WIDTH, cursor_y);
    break;
  case 0x20:
    display.fillRect(cursor_x, cursor_y, FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT, 0);
    display.setCursor(cursor_x + FONT_CHAR_WIDTH, cursor_y);
    break;
  default:
    display.setTextColor(currentColor);
    display.write(c);
    break;
  }
  setCursor();

  hasDisplayUpdate++;
}

/// <summary>
///  scan for a char to output DVI output
/// </summary>
/// <param name="vChar"></param>
void scanChar() {
  uint8_t ch = mem[DSP];

  if (ch != 0x00) {
    if (traceOn) {
      Serial.printf("OUT [%02X]\n", ch);
    }

    displayWrite(ch);
    hasDisplayUpdate++;

    mem[DSP] = 0x00;
  }
}

/// <summary>
/// 
/// </summary>
void swapDisplay() {
  if (hasDisplayUpdate > 0) {
    display.swap(true, false);
    hasDisplayUpdate = 0;
  }
}

/// <summary>
/// 
/// </summary>
void resetDisplay() {
  uint16_t colr, r, g, b;
  // load palette, convert from RGB888 to RGB565
  for (uint8_t c = 0; c <= 254; c++) {
    r = default_palette[c] >> 16;
    g = (default_palette[c] & 0x00FF00) >> 8;
    b = default_palette[c] & 0x0000FF;
    colr = ((r & 0b11111000) << 8) + ((g & 0b11111100) << 3) + (b >> 3);
//    Serial.printf("PAL: %d %04x\n", c, colr);
    display.setColor(c,colr);
  }
#if 0
  display.setColor(0, 0x0000);   // Black
  display.setColor(1, 0XF800);   // Red
  display.setColor(2, 0x07e0);   // Green
  display.setColor(3, 0xffe0);   // Yellow
  display.setColor(4, 0x001f);   // Blue
  display.setColor(5, 0xFA80);   // Orange
  display.setColor(6, 0xF8F9);   // Magenta
#endif
  display.setColor(255, 0xFFFF); // Last palette entry = White
//  Serial.println("Default palette loaded");

  // Clear back framebuffer
  display.fillScreen(0);
  display.setFont();             // Use default font
  display.setCursor(0, 0);       // Initial cursor position
  display.setTextSize(1);        // Default size
  display.setTextWrap(false);
  display.swap(true, true);     // Duplicate same palette into front & back buffers

  mem[VDU_CMD] = 0x00;

  statusCursor = true;
  autoUpdate = true;
  autoScroll = true;
  setColor(DEFAULT_TEXT_COLOR);
  clearDisplay();
}

///
void initDisplay() {
  if (!display.begin()) {
    Serial.println("ERROR: not enough RAM available");
    for (;;);
  }

  resetDisplay();
}


/// <summary>
/// 
/// </summary>
void helloDisplay() {
  // and we have lift off
  setColor(12); // BLUE
#if 1
  display.println("      N   N           666  5555   000   222");
  display.println("      N   N          6     5     0   0 2   2");
  display.println("      NN  N          6     5     0   0     2");
  display.println("      N N N EEE  OO  6666  5555  0   0    2");
  display.println("      N  NN E   O  O 6   6     5 0   0   2");
  display.println("      N   N EE  O  O 6   6     5 0   0  2");
  display.println("      N   N E   O  O 6   6 5   5 0   0 2");
  display.println("      N   N EEE  OO   666   555   000  22222");
#endif
#if 0
  display.println("nn    nn  eeeee   ooo     666   55555    00    222");
  display.println("nn n  nn eeeeee  ooooo   66 66  55      0000  22222");
  display.println("nn nn nn        ooo ooo 66      5555   00  00    222");
  display.println("nn nn nn eeeeee oo   oo 6 666   55555  00  00    22");
  display.println("nn nn nn eeeeee oo   oo 66   66     55 00  00   22");
  display.println("nn nn nn        ooo ooo 66   66     55 00  00  22");
  display.println("nn  n nn eeeeee  ooooo   66666  55555   0000  222222");
  display.println("nn    nn  eeeee   ooo     666    555     00  2222222");
#endif
//  display.print("NEO6502");
  setColor(255); // WHITE
  display.println("\n            memulator v0.04ehbp2");
  setColor(DEFAULT_TEXT_COLOR); // GREEN
}
