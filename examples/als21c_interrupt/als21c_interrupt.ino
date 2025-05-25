/*
 * xyc-als21c-k1 interrupt example.
 * The processor is interrupted when light intensity changes 12.5% or 10 counts, whichever is bigger.
 *
 * stm32f103 pins:
 * PB6 xyc-als21c-k1 SCL
 * PB7 xyc-als21c-k1 SDA
 * PB8 xyc-als21c-k1 INT
 */

#include <Wire.h>
#include "xyc_als21c_k1.h"
using namespace als21c;

#define PIN_INTERRUPT PB8

volatile bool interrupted = false;

void als21c_interrupt() {
  interrupted = true;
  als21c_clear_interrupt();
}

void setup() {
  // put your setup code here, to run once:
  while (!Serial)
    ;
  Serial.begin(115200);

  Wire.begin();

  pinMode(PIN_INTERRUPT, INPUT);
  attachInterrupt(PIN_INTERRUPT, als21c_interrupt, FALLING);

  if (!als21c_begin()) {
    Serial.println("xyc_als21c not found");
    while (1)
      ;
  } else
    Serial.println("xyc_als21c found");

  als21c_set_gain_value(256);
  als21c_set_integration_time(64);
  als21c_set_wait_time_millisec(250);
  als21c_enable(true);

  als21c_set_low_threshold(1000);
  als21c_set_high_threshold(1000);
  als21c_set_persistence(3);
  als21c_clear_interrupt();
  als21c_enable_interrupt(true);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (interrupted) {
    int32_t als, delta, als_low, als_high;

    interrupted = false;
    als = als21c_read_als();

    if (als == ALS21C_ERR_NOT_READY)
      Serial.println("WAIT");
    else if (als == ALS21C_ERR_SATURATION)
      Serial.println("SATURATION");
    else if (als == ALS21C_ERR_OVERFLOW)
      Serial.println("OVERFLOW");
    else {
      Serial.print("als: ");
      Serial.println(als);
    }
    if (als >= 0) {
      delta = als / 8;
      if (delta < 10) delta = 10;
      als_low = als - delta;
      if (als_low < 0) als_low = 0;
      als21c_set_low_threshold(als_low);
      als_high = als + delta;
      if (als_high > 0xffff) als_high = 0xffff;
      als21c_set_high_threshold(als_high);
    }
  }
  delay(1000);
}
