#ifndef _REGRESSION_H
#define _REGRESSION_H

/* small linear regression library - requires 24 bytes ram */

class regression {
public:
  regression();
  /* clear all samples */
  void clear();
  /* add sample to linear regression */
  void add(float x, float y);
  /* remove sample from linear regression */
  void remove(float x, float y);
  /* y = a * x + b */
  int n();          /*!< number of samples */
  float a();        /*!< slope */
  float b();        /*!< intercept */
  float r();        /*!< correlation*/
  float std_x();    /*!< x standard deviation */
  float std_y();    /*!< y standard deviation */
  float mean_x();   /*!< x mean */
  float mean_y();   /*!< y mean */
  float y(float x); /*!< y(x) */
  float x(float y); /*!< x(y) */
private:
  float _sum_x, _sum_x2, _sum_y, _sum_y2, _sum_xy;
  int _n;
};

#endif
