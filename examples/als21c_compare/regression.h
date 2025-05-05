#ifndef _REGRESSION_H
#define _REGRESSION_H

/*
 small linear regression library - requires 24 bytes ram

 Does curve fitting for straight lines, exponential, logarithmic and power curves.

 To fit data to straight lines: y = a * x + b
   add(x, y)

 To fit data to exponential curves: y = exp(b) * exp(a * x)
   add(x, log(y))

 To fit data to logarithmic curves: y = a * log(x) + b
   add(log(x), y)

 To fit data to power curves: y = exp(b) * pow(x, a)
   add(log(x), log(y))

 */

class regression {
public:
  regression();
  /* clear all samples */
  void clear();
  /* add sample to linear regression */
  void add(float x, float y);
  /* remove previously added sample from linear regression */
  void remove(float x, float y);
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
