/*
 * xyc-als21c-k1 interrupt example.
 * The processor is interrupted when light intensity changes 12.5% or 10 counts, whichever is bigger.
 *
 * stm32f103 pins:
 * PB11 xyc-als21c-k1 SDA
 * PB10 xyc-als21c-k1 SCL
 * PB1 xyc-als21c-k1 INT
 */

#include <Wire.h>
#include "xyc_als21c.h"
#include "Adafruit_VEML7700.h"
using namespace als21c;

#define PIN_INTERRUPT PB1

volatile bool interrupted = false;

void als21c_interrupt() {
  interrupted = true;
  als21c_clear_interrupt();
}

void setup() {
  // put your setup code here, to run once:
  while (!Serial)
    ;
  Serial.begin(9600);

  Wire.setSDA(PB11);
  Wire.setSCL(PB10);
  Wire.begin();

  pinMode(PIN_INTERRUPT, INPUT);
  attachInterrupt(PIN_INTERRUPT, als21c_interrupt, FALLING);

  if (!als21c_begin()) {
    Serial.println("xyc_als21c not found");
    while (1)
      ;
  } else
    Serial.println("xyc_als21c found");

  als21c_set_gain(ALS21C_GAIN_256X);
  als21c_set_integration(ALS21C_INT_TIME_64T, 0);
  als21c_set_wait_time(250);
  als21c_enable(true);

  als21c_set_low_threshold(1000);
  als21c_set_high_threshold(1000);
  als21c_set_persistence(3);
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
