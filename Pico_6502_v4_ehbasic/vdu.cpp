// 
// 
// 
#include <PicoDVI.h>

#include "memory.h"
#include "vdu.h"

boolean        statusCursor = true;
uint8_t        currentColor;
uint8_t        currentColorIndex = 0;
boolean        autoUpdate = true;
boolean        autoScroll = true;
uint32_t       hasDisplayUpdate = 0;

boolean        traceOn = false;

// Here's how an 320x240 256 colors graphics display is declared.
DVIGFX8 display(DVI_RES_320x240p60, true, pico_neo6502_cfg);

/// <summary>
/// control visibility of cursor
/// </summary>
/// <param name="vSet"></param>
inline __attribute__((always_inline))
void showCursor(boolean vSet) {
  statusCursor = vSet;
}


/// <summary>
/// set current text color (by pallette index)
/// </summary>
/// <param name="vColor"></param>
inline __attribute__((always_inline))
void setColor(uint8_t vColor) {
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
void clearDisplay() {
  display.fillScreen(0);
  display.setCursor(0, 0);
  hasDisplayUpdate++;
}

/// <summary>
/// performa a action on the display
/// </summary>
/// <param name="vCmd"></param>
void setVDU(uint8_t vCmd) {
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
    //           Serial.printf("GCURSOR: %04d %04d\n", cx, cy);
    hasDisplayUpdate++;
   break;
 
  case CMD_COLOR: // set color
    setColor(mem[VDU_COL]);
    //    Serial.printf("COLOR %02d\n", currentColor);
        // bg color ignored
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

  case CMD_VDU: //  VDU control
    switch (mem[VDU_MOD] & 0x01) {
    case 1:
      showCursor(false);
      break;
    default:
      showCursor(true);
      break;
    }

    autoUpdate = ! (mem[VDU_MOD] >> 7);
    autoScroll = !((mem[VDU_MOD] >> 1) & 0x01);
    break;

  case CMD_REFR: // REFRESH
    swapDisplay();
    break;

  case CMD_SANE:
    resetDisplay();
    break;
  }
}

/// <summary>
/// 
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
inline __attribute__((always_inline))
void displayWrite(uint8_t c) {
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
///  write a char to output DVI output
/// </summary>
/// <param name="vChar"></param>
void writeChar(uint8_t vChar) {
  if (traceOn) {
    Serial.printf("OUT [%02X]\n", vChar);
  }

  displayWrite(vChar);
  hasDisplayUpdate++;
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
  display.setColor(0, 0x0000);   // Black
  display.setColor(1, 0XF800);   // Red
  display.setColor(2, 0x07e0);   // Green
  display.setColor(3, 0xffe0);   // Yellow
  display.setColor(4, 0x001f);   // Blue
  display.setColor(5, 0xFA80);   // Orange
  display.setColor(6, 0xF8F9);   // Magenta
  display.setColor(255, 0xFFFF); // Last palette entry = White
  // Clear back framebuffer
  display.fillScreen(0);
  display.setFont();             // Use default font
  display.setCursor(0, 0);       // Initial cursor position
  display.setTextSize(1);        // Default size
  display.setTextWrap(false);
  display.swap(false, true);     // Duplicate same palette into front & back buffers

  mem[VDU_CMD] = 0x00;

  statusCursor = true;
  autoUpdate = true;
  autoScroll = true;
  setColor(2);
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
  setColor(4); // BLUE
  display.print("NEO6502");
  setColor(255); // WHITE
  display.println(" memulator v0.04ehb");
  setColor(2); // GREEN
}