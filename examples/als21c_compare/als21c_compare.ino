/*
 * compares xyc-als21c-k1 and veml7700 using linear regression.
 *
 * stm32f103 pins:
 * PB6 xyc-als21c-k1 SCL
 * PB7 xyc-als21c-k1 SDA
 * PB8 xyc-als21c-k1 INT
 */

#include <Wire.h>
#include "Adafruit_VEML7700.h"
#include "xyc_als21c_k1.h"
#include "curveFitting.h"

using namespace als21c;

linFit lin_reg;
Adafruit_VEML7700 veml = Adafruit_VEML7700();

void setup() {
  // put your setup code here, to run once:

  while (!Serial)
    ;
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(100000L);

  if (!veml.begin(&Wire)) {
    Serial.println("veml7700 not found");
    while (1)
      ;
  } else Serial.println("veml7700 found");

  if (!als21c_begin()) {
    Serial.println("xyc_als21c not found");
    while (1)
      ;
  } else
    Serial.println("xyc_als21c found");

  als21c_set_gain_value(256);
  als21c_set_integration_time(64);
  als21c_set_auto_lux(true);
  als21c_set_wait_time_millisec(250);
  als21c_enable(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  int32_t als21c_lux;
  float veml7700_lux;

  als21c_lux = als21c_read_lux();
  veml7700_lux = veml.readLux(VEML_LUX_AUTO);

  Serial.print("veml7700: ");
  Serial.print(veml7700_lux);

  Serial.print("\t als21c: ");
  if (als21c_lux == ALS21C_ERR_NOT_READY) Serial.println("WAIT");
  else if (als21c_lux == ALS21C_ERR_SATURATION) Serial.println("SATURATION");
  else if (als21c_lux == ALS21C_ERR_OVERFLOW) Serial.println("OVERFLOW");
  else Serial.println(als21c_lux);

  if (als21c_lux >= 0) {
    lin_reg.add(veml7700_lux, als21c_lux);
    if (lin_reg.n() % 100 == 0) {
      /* print linear regression every 100 samples */
      Serial.print("samples: ");
      Serial.print(lin_reg.n());
      Serial.print("\t slope: ");
      Serial.print(lin_reg.a(), 4);
      Serial.print("\t intercept: ");
      Serial.print(lin_reg.b(), 4);
      Serial.print("\t correlation: ");
      Serial.println(lin_reg.r(), 4);
    }
  }

  delay(1000);
}
