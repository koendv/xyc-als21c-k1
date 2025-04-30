#include "regression.h"
#include <math.h>

regression::regression() {
  reset();
}

void regression::reset() {
  sum_x = 0.0;
  sum_x2 = 0.0;
  sum_y = 0.0;
  sum_y2 = 0.0;
  sum_xy = 0.0;
  n = 0;
}

void regression::addSample(float x, float y) {
  sum_x += x;
  sum_x2 += x * x;
  sum_y += y;
  sum_y2 += y * y;
  sum_xy += x * y;
  n++;
}

void regression::calculate() {
  if (n * sum_x2 - sum_x * sum_x != 0) {
    m = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    b = (sum_y * sum_x2 - sum_x * sum_xy) / (n * sum_x2 - sum_x * sum_x);
    if (n * sum_y2 - sum_y * sum_y != 0)
      r = (n * sum_xy - sum_x * sum_y) / sqrtf((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));
    else
      r = 0;
  } else {
    m = 0;
    b = 0;
    r = 0;
  }
}
