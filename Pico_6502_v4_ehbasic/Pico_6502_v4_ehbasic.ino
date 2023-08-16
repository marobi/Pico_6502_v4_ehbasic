/*
 Name:		Pico_NEO6502
 Author:	Rien Matthijsse

 Inspired by work  http://www.8bitforce.com/blog/2019/03/12/retroshield-6502-operation/
*/

#include "pico/stdio.h"

#include "pico/stdlib.h"
#include "pico/time.h"

#include "mos65C02.h"
#include "memory.h"
#include "vdu.h"
#include "sound.h"

//#define UCASE_ONLY

//
uint32_t       clockCount = 0UL;
unsigned long  lastClockTS;
unsigned long  frameClockTS;

boolean        logState = false;


/// <summary>
/// setup memulator
/// </summary>
void setup() {
  Serial.begin(115200);
  //  while (!Serial);

  sleep_ms(2500);
  Serial.println("NEO6502 memulator v0.04ehbp2");

  Serial.printf("Starting ...\n");

  initDisplay();

  initMemory();

  initSound();

  init6502();
  reset6502();

  // 4 stats
  clockCount = 0UL;
  lastClockTS = millis();

  helloDisplay();
}

////////////////////////////////////////////////////////////////////
// Serial Event
////////////////////////////////////////////////////////////////////

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX. Multiple bytes of data may be available.
 */
inline __attribute__((always_inline))
void serialEvent1()
{
  if (Serial.available()) {
    switch (Serial.peek()) {

    case 0x12: // ^R
      Serial.read();
      Serial.println("RESET");
      //      showCursor(true);
      resetDisplay();
      reset6502();
      break;

    case 0x0C: // ^L
      Serial.read();
      Serial.println("LOGGING");
      logState = !logState;
      clockCount = 0UL;
      break;

    case 0x04: // ^D
      Serial.read();
      Serial.print("VDU: ");
      for (uint8_t i = 0; i < 18; i++) {
        Serial.printf("%02X ", mem[0XD020 + i]);
      }
      Serial.println("\n\SPRITE:");
      for (uint8_t i = 0; i < 16; i++) {
        for (uint8_t j = 0; j < 16; j++) {
          Serial.printf("%02X ", mem[0XD100 + i*16 + j]);
        }
        Serial.println();
      }
      Serial.println();

      Serial.println("\n\TILE:");
      for (uint8_t i = 0; i < 16; i++) {
        for (uint8_t j = 0; j < 16; j++) {
          Serial.printf("%02X ", mem[0XD200 + i * 16 + j]);
        }
        Serial.println();
      }
      Serial.println();
      break;

    case 0x14: // ^T
      Serial.read();
      Serial.println("TRACE");
      traceOn = !traceOn;
      break;

    default:
      if (mem[KBD] == 0x00) {             // read serial byte only if we can
#ifdef UCASE_ONLY
        byte ch = toupper(Serial.read()); // apple1 expects upper case
        mem[KBD] = ch | 0x80;             // apple1 expects bit 7 set for incoming characters.
#else
        byte ch = Serial.read();
        mem[KBD] = ch;
#endif
        if (traceOn) {
          Serial.printf("IN: [%02X]\n", ch);
        }
      }
      break;
    }
  }
  return;
}

/// <summary>
/// 
/// </summary>
void loop() {
  static uint32_t i, j, f = 1;

  //forever
  for (;;) {
//    delay(100);

    tick6502();
    clockCount++;

    if (j-- == 0) {
      serialEvent1();
      scanSound();
      scanChar();
      scanVDU();

      j = 750UL;
    }

    if (autoUpdate) {
      if (f-- == 0) {
        if ((millis() - frameClockTS) >= FRAMETIME) {
          swapDisplay();
          frameClockTS = millis();
        }

        f = 7500UL;
      }
    }

    // only do stats when in logging mode
    if (logState) {
      if (i-- == 0) {
        if ((millis() - lastClockTS) >= 5000UL) {
          Serial.printf("kHz = %0.1f\n", clockCount / 5000.0);

          clockCount = 0UL;
          lastClockTS = millis();
        }

        i = 20000UL;
      }
    }
  }
}
