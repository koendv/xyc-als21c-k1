/*
 * xyc-als21c-k1 example: polling with automatic adjusting of gain and integration time.
 * prints light intensity every second.
 *
 * stm32f103 pins:
 * PB6 xyc-als21c-k1 SCL
 * PB7 xyc-als21c-k1 SDA
 * PB8 xyc-als21c-k1 INT
 */

#include <Wire.h>
#include "xyc_als21c_k1.h"
using namespace als21c;

void setup() {
  // put your setup code here, to run once:
  while (!Serial)
    ;
  Serial.begin(9600);

  Wire.begin();

  if (!als21c_begin()) {
    Serial.println("xyc_als21c not found");
    while (1)
      ;
  } else
    Serial.println("xyc_als21c found");

  als21c_set_auto_lux(true);
  als21c_set_wait_time_millisec(250);
  als21c_enable(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  int32_t lux = als21c_read_lux();
  if (lux == ALS21C_ERR_NOT_READY) Serial.println("WAIT");
  else if (lux == ALS21C_ERR_SATURATION) Serial.println("SATURATION");
  else if (lux == ALS21C_ERR_OVERFLOW) Serial.println("OVERFLOW");
  else {
    Serial.print("lux: ");
    Serial.println(lux);
  }
  delay(1000);
}
