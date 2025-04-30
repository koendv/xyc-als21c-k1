#ifndef _REGRESSION_H
#define _REGRESSION_H

/* calculate linear regression */

class regression {
public:
  regression();
  /* add sample to linear regression */
  void addSample(float x, float y);
  /* clear all samples */
  void reset();
  /* calculate slope, intercept, correlation */
  void calculate();
  /* number of samples */
  int n;
  /* y = m * x + b . */
  float m, /* slope */
    b,     /* intercept */
    r;     /* correlation*/
private:
  float sum_x, sum_x2, sum_y, sum_y2, sum_xy;
};

#endif
