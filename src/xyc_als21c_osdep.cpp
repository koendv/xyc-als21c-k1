/*!
 *
 * 	I2C Driver for NEWOPT XYC_ALS21C ambient light sensor
 *
 * 	This is a library for the NEWOPT XYC_ALS21C breakout:
 * 	http://oshwlab.com/koendv/xyc_als21c_k1
 *
 *      code for Arduino
 */

#include <xyc_als21c.h>

#ifdef __cplusplus
#include <Wire.h>

namespace als21c {
#endif

#define WIRE Wire

/* I2C operations */

void als21c_i2c_write8(const uint8_t reg, const uint8_t data) {
  WIRE.beginTransmission(ALS21C_I2C_ADDR);
  WIRE.write(reg);
  WIRE.write(data);
  WIRE.endTransmission();
  return;
}

void als21c_i2c_write16(const uint8_t reg, const uint16_t data) {
  WIRE.beginTransmission(ALS21C_I2C_ADDR);
  WIRE.write(reg);
  WIRE.write(uint8_t(data & 0xff));
  WIRE.write(uint8_t(data >> 8));
  WIRE.endTransmission();
  return;
}

uint8_t als21c_i2c_read8(const uint8_t reg) {
  uint8_t data;
  WIRE.beginTransmission(ALS21C_I2C_ADDR);
  WIRE.write(reg);
  WIRE.endTransmission(false);
  if (WIRE.requestFrom(ALS21C_I2C_ADDR, 1) != 1) {
    return 0;
  }
  data = WIRE.read();
  return data;
}

uint16_t als21c_i2c_read16(const uint8_t reg) {
  uint16_t data;
  WIRE.beginTransmission(ALS21C_I2C_ADDR);
  WIRE.write(reg);
  WIRE.endTransmission(false);
  if (WIRE.requestFrom(ALS21C_I2C_ADDR, 2) != 2) {
    return 0;
  }
  data = WIRE.read();
  data |= uint32_t(WIRE.read()) << 8;
  return data;
}

void als21c_dump_regs() {
  Serial.print("reg_sysm_ctrl ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_SYSM_CTRL), HEX);
  Serial.print("reg_int_ctrl ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_INT_CTRL), HEX);
  Serial.print("reg_int_flag ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_INT_FLAG), HEX);
  Serial.print("reg_wait_time ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_WAIT_TIME), HEX);
  Serial.print("reg_als_gain ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_ALS_GAIN), HEX);
  Serial.print("reg_als_time ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_ALS_TIME), HEX);
  Serial.print("reg_persistence ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_PERSISTENCE), HEX);
  Serial.print("reg_als_thres_l ");
  Serial.println(als21c_i2c_read16(ALS21C_REG_ALS_THRES_L), HEX);
  Serial.print("reg_als_thres_h ");
  Serial.println(als21c_i2c_read16(ALS21C_REG_ALS_THRES_H), HEX);
  Serial.print("reg_data_status ");
  Serial.println(als21c_i2c_read8(ALS21C_REG_DATA_STATUS), HEX);
  Serial.print("reg_als_data ");
  Serial.println(als21c_i2c_read16(ALS21C_REG_ALS_DATA), HEX);
  Serial.print("reg_prod_id ");
  Serial.println(als21c_i2c_read16(ALS21C_REG_PROD_ID), HEX);
}

#ifdef __cplusplus
}; /* namespace als21c */
#endif
