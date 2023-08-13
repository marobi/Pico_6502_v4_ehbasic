// sound.h

#ifndef _SOUND_h
#define _SOUND_h

#include "Arduino.h"

// SOUND register definitions
#define SND_CMD   0xD040  // CMD register
#define SND_NOTE  0xD041  // NOTE
#define SND_DUR   0xD042  // DUR
#define SND_STAT  0xD04F  // STATUS

#define SND_PLAY  0x01  // PLAY
#define SND_STOP  0x02  // NOPLAY

void initSound();
void scanSound();

#endif

