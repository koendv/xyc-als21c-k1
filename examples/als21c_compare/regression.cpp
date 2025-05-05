#include "regression.h"
#include <math.h>

regression::regression() {
  clear();
}

/*! @brief: clear statistics */
void regression::clear() {
  _sum_x = 0.0;
  _sum_x2 = 0.0;
  _sum_y = 0.0;
  _sum_y2 = 0.0;
  _sum_xy = 0.0;
  _n = 0;
}

/*! @brief: add a sample */
void regression::add(float x, float y) {
  _sum_x += x;
  _sum_x2 += x * x;
  _sum_y += y;
  _sum_y2 += y * y;
  _sum_xy += x * y;
  _n++;
}

/*! @brief: remove previously added sample */
void regression::remove(float x, float y) {
  _sum_x -= x;
  _sum_x2 -= x * x;
  _sum_y -= y;
  _sum_y2 -= y * y;
  _sum_xy -= x * y;
  --_n;
}

/*! @brief: number of samples */
int regression::n() {
  return _n;
}

/* y = a * x + b */

/*! @brief: slope */
float regression::a() {
  return (_n * _sum_xy - _sum_x * _sum_y) / (_n * _sum_x2 - _sum_x * _sum_x);
}

/*! @brief: intercept */
float regression::b() {
  return (_sum_y * _sum_x2 - _sum_x * _sum_xy) / (_n * _sum_x2 - _sum_x * _sum_x);
}

/*! @brief: correlation*/
float regression::r() {
  return (_n * _sum_xy - _sum_x * _sum_y) / sqrtf((_n * _sum_x2 - _sum_x * _sum_x) * (_n * _sum_y2 - _sum_y * _sum_y));
}

/*! @brief: x standard deviation */
float regression::std_x() {
  return sqrtf((_sum_x2 - _sum_x * _sum_x / _n) / (_n - 1));
}

/*! @brief: y standard deviation */
float regression::std_y() {
  return sqrtf((_sum_y2 - _sum_y * _sum_y / _n) / (_n - 1));
}

/*! @brief: x mean */
float regression::mean_x() {
  return _sum_x / _n;
}

/*! @brief: y mean */
float regression::mean_y() {
  return _sum_y / _n;
}

/*! @brief: y(x) */
float regression::y(float x) {
  return a() * x + b();
}

/*! @brief: x(y) */
float regression::x(float y) {
  return (y - b()) / a();
}
