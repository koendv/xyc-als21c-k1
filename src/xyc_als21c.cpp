/*!
 *
 * 	I2C Driver for NEWOPT XYC_ALS21C ambient light sensor
 *
 * 	This is a library for the NEWOPT XYC_ALS21C breakout:
 * 	http://oshwlab.com/koendv/xyc_als21c_k1
 *
 */

#include <xyc_als21c.h>

#ifdef __cplusplus
#include <cstring>

namespace als21c {
#else
#include <string.h>
#endif

als21c_data_s als21c_data;

/*
 * lookup table for lux values.
 * lux = 478.233 * x^1 + 0.0416391 * x^3 + -1.18758e-06 * x^5
 * where x = count / (gain * itime * (als_conv + 1))
 * lux values multiplied by 256 for added precision.
 * obtained by curve-fitting veml7700 and als21c measurements.
 */

static const uint32_t lux_256[] = {
    0,        122438,   244941,   367571,   490392,   613470,   736866,
    860645,   984869,   1109602,  1234906,  1360843,  1487476,  1614866,
    1743074,  1872160,  2002185,  2133209,  2265290,  2398487,  2532857,
    2668458,  2805345,  2943575,  3083201,  3224279,  3366860,  3510997,
    3656742,  3804143,  3953251,  4104114,  4256778,  4411289,  4567692,
    4726031,  4886347,  5048682,  5213076,  5379566,  5548189,  5718982,
    5891978,  6067209,  6244707,  6424501,  6606619,  6791087,  6977929,
    7167169,  7358827,  7552923,  7749474,  7948496,  8150002,  8354004,
    8560513,  8769534,  8981075,  9195140,  9411728,  9630841,  9852474,
    10076624, 10303283, 10532440, 10764085, 10998203, 11234777, 11473788,
    11715214, 11959032, 12205214, 12453731, 12704552, 12957642, 13212962,
    13470475, 13730135, 13991899, 14255718, 14521539, 14789310, 15058972,
    15330466, 15603728, 15878692, 16155289, 16433445, 16713087, 16994133,
    17276503, 17560111, 17844868, 18130682, 18417457, 18705095, 18993492,
    19282544, 19572141, 19862170, 20152513, 20443051, 20733660, 21024212,
    21314576, 21604616, 21894194, 22183166, 22471385, 22758701, 23044960,
    23330001, 23613663, 23895778, 24176175, 24454680, 24731113, 25005290,
    25277023, 25546121, 25812386, 26075617, 26335611, 26592155, 26845037,
    27094038, 27338933, 27579496, 27815494, 28046689, 28272839
};

/*!
 * @brief  initializes ambient light sensor
 */
bool als21c_begin() {
  if (als21c_get_product_id() != ALS21C_PRODUCT_ID)
    return false;
  /* reset */
  als21c_reset();

  return true;
}

/*!
 * @brief  stop ambient light sensor measurement and interrupts
 */
void als21c_end() {
  als21c_i2c_write8(ALS21C_REG_SYSM_CTRL, 0x0); /* disable als */
  als21c_i2c_write8(ALS21C_REG_INT_CTRL, 0x0);  /* disable interrupts */
}

/*!
 * @brief  reset ALS sensor
 */
void als21c_reset() {
  /* clear */
  memset(&als21c_data, 0, sizeof(als21c_data));
  /* reset */
  als21c_data.swrst = 1;
  als21c_set_reg_sysm_ctrl();
  als21c_data.swrst = 0;
  /* default values after reset */
  als21c_data.en_aint = 0x1;
  als21c_data.pga_als = 0x1;
  als21c_data.int_time = 0x3;
  als21c_data.prs_als = 0x1;
}

/*!
 * @brief  switch ambient light sensor on/off
 * @param  onoff
 */
void als21c_enable(bool onoff) {
  als21c_data.en_als = onoff ? 0x1 : 0x0;
  als21c_set_reg_sysm_ctrl();
}

/*!
 * @brief  switch ambient light sensor on for a single measurement
 */
void als21c_enable_once(bool onoff) {
  als21c_data.en_once = onoff ? 0x1 : 0x0;
  als21c_set_reg_sysm_ctrl();
}

/*!
 * @brief  set the ambient light sensor gain
 * @param  gain
 */
void als21c_set_gain(als21c_gain_t gain) {
  als21c_data.pga_als = gain;
  als21c_set_reg_als_gain();
}

/*!
 * @brief  set the integration time for the ADC in millis
 * @param  millisec
 *         integration time
 */
void als21c_set_integration_time(uint16_t millisec) {
  uint32_t count;
  count = millisec;
  count = count * 416 / 487;  /* 416/487 = 1.17ms */

  if (count == 0) {
    als21c_data.int_time = ALS21C_INT_TIME_1T;
    als21c_data.als_conv = 0;
  } else if (count <= 16) {
    als21c_data.int_time = ALS21C_INT_TIME_1T;
    als21c_data.als_conv = count - 1;
  } else if (count <= 4 * 16) {
    als21c_data.int_time = ALS21C_INT_TIME_4T;
    als21c_data.als_conv = count / 4 - 1;
  } else if (count <= 16 * 16) {
    als21c_data.int_time = ALS21C_INT_TIME_16T;
    als21c_data.als_conv = count / 16 - 1;
  } else if (count <= 64 * 16) {
    als21c_data.int_time = ALS21C_INT_TIME_64T;
    als21c_data.als_conv = count / 64 - 1;
  } else {
    als21c_data.int_time = ALS21C_INT_TIME_64T;
    als21c_data.als_conv = 15;
  }

  als21c_set_reg_als_time();
}


/*!
 * @brief  set the integration time for the ADC
 * @param  itime
 * @param  icount
 */
void als21c_set_integration(als21c_int_time_t itime, uint8_t icount) {
  als21c_data.int_time = itime;
  als21c_data.als_conv = icount;
  als21c_set_reg_als_time();
}
/*!
 * @brief  set the wait time between two measurements in millis
 * @param  millisec
 *         wait time 0 switches waiting off
 */
void als21c_set_wait_time(uint16_t millisec) {

  if (millisec <= 8) {
    als21c_data.wtime_unit = 0x0;
    als21c_data.wtime = 0;
  } else if (millisec <= 512) {
    als21c_data.wtime_unit = 0x0;
    als21c_data.wtime = millisec / 8 - 1;
  } else if (millisec <= 1024) {
    als21c_data.wtime_unit = 0x1;
    als21c_data.wtime = millisec / 16 - 1;
  } else if (millisec <= 2048) {
    als21c_data.wtime_unit = 0x2;
    als21c_data.wtime = millisec / 32 - 1;
  } else if (millisec <= 4096) {
    als21c_data.wtime_unit = 0x3;
    als21c_data.wtime = millisec / 64 - 1;
  } else {
    /* overflow */
    als21c_data.wtime_unit = 0x3;
    als21c_data.wtime = 0x3f;
  }
  als21c_set_reg_wait_time();

  /* disable wait if millisec == 0 */
  if (millisec == 0)
    als21c_data.en_wait = 0x0;
  else
    als21c_data.en_wait = 0x1;
  als21c_set_reg_sysm_ctrl();
}

/*!
 * @brief  return time in millis between two measurements
 * @return millisec
 *         sum of integration time and wait time
 */
uint32_t als21c_get_delay_millisec() {
  uint32_t itime, wtime, millisec;

  switch (als21c_data.int_time) {
    case ALS21C_INT_TIME_1T: itime = 1; break;
    case ALS21C_INT_TIME_4T: itime = 4; break;
    case ALS21C_INT_TIME_16T: itime = 16; break;
    case ALS21C_INT_TIME_64T: itime = 64; break;
    default: itime = 1; break;
  }
  itime = itime * (als21c_data.als_conv + 1);
  millisec = (487 * itime + 416) / 417;

  if (als21c_data.en_wait) {
    wtime = 8 << als21c_data.wtime_unit;
    wtime = wtime * (als21c_data.wtime + 1);
    millisec = millisec + wtime;
  }

  return millisec;
}

/*!
 * @brief  return raw ALS count
 * @return count
 *         positive or zero value is als count
 *         negative value is error condition
 *         ALS21C_ERR_SATURATION: error in the analog part (amplifier, comparator)
 *         ALS21C_ERR_NOT_READY: reading too soon (sensor still counting)
 */
int32_t als21c_read_als() {
  uint16_t count;
  als21c_get_reg_data_status();
  if (!als21c_data.data_ready) return ALS21C_ERR_NOT_READY;
  if (als21c_data.saturation_als || als21c_data.saturation_comp) return ALS21C_ERR_SATURATION;
  count = als21c_i2c_read16(ALS21C_REG_ALS_DATA);
  return count;
}

/*!
 * @brief  return ALS light intensity in lux
 * @return lux
 *         positive or zero value is lux
 *         negative value is error condition
 *         ALS21C_ERR_SATURATION: error in the analog part (amplifier, comparator)
 *         ALS21C_ERR_OVERFLOW: error in the digital part (counter)
 *         ALS21C_ERR_NOT_READY: reading too soon (sensor still counting)
 */
int32_t als21c_read_lux() {
  int32_t count, gain, itime, als_conv, max_count;
  int32_t lux;

  als21c_get_reg_data_status();
  if (!als21c_data.data_ready) return ALS21C_ERR_NOT_READY;

  count = als21c_i2c_read16(ALS21C_REG_ALS_DATA);

  switch (als21c_data.pga_als) {
    case ALS21C_GAIN_1X: gain = 1; break;
    case ALS21C_GAIN_4X: gain = 4; break;
    case ALS21C_GAIN_16X: gain = 16; break;
    case ALS21C_GAIN_64X: gain = 64; break;
    case ALS21C_GAIN_256X: gain = 256; break;
    default: gain = 1; break;
  }
  if (als21c_data.pd_sel) gain *= 2;

  switch (als21c_data.int_time) {
    case ALS21C_INT_TIME_1T: itime = 1; break;
    case ALS21C_INT_TIME_4T: itime = 4; break;
    case ALS21C_INT_TIME_16T: itime = 16; break;
    case ALS21C_INT_TIME_64T: itime = 64; break;
    default: itime = 1; break;
  }

  als_conv = als21c_data.als_conv;

  max_count = 1024 * itime * (als_conv + 1) - 1;
  if (max_count > 0xffff) max_count = 0xffff;

  /* linear interpolation in lookup table. integer math, suitable for small microcontroller */
  const uint32_t last_index = sizeof(lux_256) / sizeof(lux_256[0]) - 1;
  uint32_t q = (256 * count) / (gain * itime * (als_conv + 1));  /* normalized counts, multiplied by 256 */
  uint32_t x1 = q >> 8;
  if (x1 > last_index - 1) x1 = last_index - 1;
  uint32_t y1 = lux_256[x1];
  uint32_t y2 = lux_256[x1 + 1];
  uint32_t delta_x = q & 0xff;
  uint32_t delta_y = (y2 - y1) * delta_x / 256;
  lux = (y1 + delta_y) / 256;

#if 0
  Serial.print("count: ");
  Serial.print(count);
  Serial.print(" max_count: ");
  Serial.print(max_count);
  Serial.print(" gain: ");
  Serial.print(gain);
  Serial.print(" itime: ");
  Serial.print(itime);
  Serial.print(" als_conv: ");
  Serial.print(als_conv);
  Serial.print(" lux: ");
  Serial.println(lux);
#endif

  /* auto configuration of pga_als, int_time and als_conv */
  if (als21c_data.auto_lux) {
    if (als21c_data.saturation_als || als21c_data.saturation_comp || (count > max_count - max_count / 8)) {
      if (als21c_data.als_conv > 0)
        /* decrease als_conv */
        als21c_set_integration((als21c_int_time_t)als21c_data.int_time, als21c_data.als_conv - 1);
      else if (als21c_data.int_time != ALS21C_INT_TIME_1T)
        /* decrease int_time */
        als21c_set_integration((als21c_int_time_t)(als21c_data.int_time - 1), 0);
      else if (als21c_data.pga_als != ALS21C_GAIN_1X)
        /* decrease gain */
        als21c_set_gain((als21c_gain_t)(als21c_data.pga_als >> 1));
    } else if (count < max_count / 8) {
      if (als21c_data.pga_als != ALS21C_GAIN_256X)
        /* increase gain */
        als21c_set_gain((als21c_gain_t)(als21c_data.pga_als << 1));
      else if (als21c_data.int_time != ALS21C_INT_TIME_64T)
        /* increase int_time */
        als21c_set_integration((als21c_int_time_t)(als21c_data.int_time + 1), 0);
      else if (als21c_data.als_conv < 15)
        /* increase als_conv */
        als21c_set_integration((als21c_int_time_t)als21c_data.int_time, als21c_data.als_conv + 1);
    }
  }

  if (als21c_data.saturation_als || als21c_data.saturation_comp)
    return ALS21C_ERR_SATURATION; /* analog */
  else if (count >= max_count)
    return ALS21C_ERR_OVERFLOW; /* digital */

  return lux;
}

/*!
 * @brief  set auto lux adjust
 * @param  onoff enable or disable
 *         when enabled, automatically adjusts ALS gain and integration time
 */
void als21c_set_auto_lux(bool onoff) {
  als21c_data.auto_lux = onoff;
  if (onoff) {
    /* begin with lowest amplification, avoid saturation */
    als21c_set_gain(ALS21C_GAIN_1X);
    als21c_set_integration(ALS21C_INT_TIME_1T, 0);
  }
}

/*!
 * @brief  enable or disable interrupt
 * @param  onoff
 */
void als21c_enable_interrupt(bool onoff) {
  als21c_data.en_aint = onoff ? 0x1 : 0x0;
  als21c_set_reg_int_ctrl();
}

/*!
 * @brief  set ALS synchronisation
 * @param  onoff
 *         when enabled, ALS light measurement waits until interrupt is cleared
 */
void als21c_enable_als_sync(bool onoff) {
  als21c_data.als_sync = onoff ? 0x1 : 0x0;
  als21c_set_reg_int_ctrl();
}

/*!
 * @brief  get ALS interrupt status
 * @return true if interrupt pending
 */
bool als21c_interrupt_status() {
  als21c_get_reg_int_flag();
  return als21c_data.int_por || als21c_data.int_als;
}

/*!
 * @brief  clear ALS interrupt
 */
void als21c_clear_interrupt() {
  als21c_i2c_write8(ALS21C_REG_INT_FLAG, 0x0);
}

/*!
 * @brief  set interrupt persistence
 * @param  pers
 *         sets number of consecutive measurements that have to be outside threshold to trigger an interrupt
 *         if pers is zero, thresholds are ignored and every measurement triggers an interrupt
 */
void als21c_set_persistence(uint8_t pers) {
  if (pers > 15) pers = 15;
  als21c_data.prs_als = pers;
  als21c_set_reg_persistence();
}

/*!
 * @brief  set lower threshold for ALS count to trigger an interrupt
 * @param  value
 */
void als21c_set_low_threshold(uint16_t value) {
  als21c_i2c_write16(ALS21C_REG_ALS_THRES_L, value);
}

/*!
 * @brief  set upper threshold for ALS count to trigger an interrupt
 * @param  value
 */
void als21c_set_high_threshold(uint16_t value) {
  als21c_i2c_write16(ALS21C_REG_ALS_THRES_H, value);
}

/*!
 * @brief  get ALS product id
 * @return product id
 */
uint16_t als21c_get_product_id() {
  return als21c_i2c_read16(ALS21C_REG_PROD_ID);
}

/* sysm_ctrl register */
void als21c_set_reg_sysm_ctrl() {
  uint8_t dta = als21c_data.swrst << 7 | als21c_data.en_wait << 6 | als21c_data.en_frst << 5 | als21c_data.en_once << 1 | als21c_data.en_als;
  als21c_i2c_write8(ALS21C_REG_SYSM_CTRL, dta);
}

/* int_ctrl register */
void als21c_set_reg_int_ctrl() {
  uint8_t dta = als21c_data.als_sync << 4 | als21c_data.en_aint;
  als21c_i2c_write8(ALS21C_REG_INT_CTRL, dta);
}

/* int_flag register */
void als21c_set_reg_int_flag() {
  uint8_t dta = als21c_data.int_por << 7 | als21c_data.data_flag << 6 | als21c_data.int_als;
  als21c_i2c_write8(ALS21C_REG_INT_FLAG, dta);
}

/* wait_time register */
void als21c_set_reg_wait_time() {
  uint8_t dta = als21c_data.wtime_unit << 6 | als21c_data.wtime;
  als21c_i2c_write8(ALS21C_REG_WAIT_TIME, dta);
}

/* als_gain register */
void als21c_set_reg_als_gain() {
  uint8_t dta = als21c_data.pd_sel << 7 | als21c_data.pga_als;
  als21c_i2c_write8(ALS21C_REG_ALS_GAIN, dta);
}

/* als_time register */
void als21c_set_reg_als_time() {
  uint8_t dta = als21c_data.als_conv << 4 | als21c_data.int_time;
  als21c_i2c_write8(ALS21C_REG_ALS_TIME, dta);
}

/* persistence register */
void als21c_set_reg_persistence() {
  uint8_t dta = als21c_data.int_src << 4 | als21c_data.prs_als;
  als21c_i2c_write8(ALS21C_REG_PERSISTENCE, dta);
}

/* data status register */
void als21c_get_reg_data_status() {
  uint8_t data = als21c_i2c_read8(ALS21C_REG_DATA_STATUS);
  als21c_data.data_ready = (data >> 7) & 0x1;
  als21c_data.saturation_als = (data >> 1) & 0x1;
  als21c_data.saturation_comp = data & 0x1;
}

/* interrupt flag register */
void als21c_get_reg_int_flag() {
  uint8_t data = als21c_i2c_read8(ALS21C_REG_INT_FLAG);
  als21c_data.int_por = (data >> 7) & 0x1;
  als21c_data.data_flag = (data >> 6) & 0x1;
  als21c_data.int_als = data & 0x1;
}

#ifdef __cplusplus
}; /* namespace als21c */
#endif
