/*
    ChibiOS/RT - Copyright (C) 2006-2014 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "hal.h"

static bool i2cOk = false;

static THD_WORKING_AREA(waThread1, 64);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("Blinker");
  while (TRUE) {
    if (i2cOk) {
      palTogglePad(TEENSY_PIN13_IOPORT, TEENSY_PIN13);
      i2cOk=0;
    }
    chThdSleepMilliseconds(500);
  }
}

static I2CConfig i2ccfg = {
  40000
};

/*
 * Application entry point.
 */
int main(void) {

  uint8_t tx[8], rx[8];

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  palSetPadMode(IOPORT2, 0, PAL_MODE_ALTERNATIVE_2);
  palSetPadMode(IOPORT2, 1, PAL_MODE_ALTERNATIVE_2);
  i2cStart(&I2CD1, &i2ccfg);

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  while (1) {
    tx[0] = 0x10;
    tx[1] = 0x02;
    i2cMasterTransmitTimeout(&I2CD1, 0x21, tx, 2, rx, 6, TIME_INFINITE);
    i2cOk = (rx[0] == 0x10) ? true : false;
    chThdSleepMilliseconds(2000);
  }
}
