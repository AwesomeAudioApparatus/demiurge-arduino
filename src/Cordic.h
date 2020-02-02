
#ifndef _DEMIURGE_CORDIC_H_
#define _DEMIURGE_CORDIC_H_

class Cordic {

public:
   static double angle_shift(double alpha, double beta);

   static double arccos_cordic(double t, int n);

   static void arccos_values(int &n_data, double &x, double &fx);

   static double arcsin_cordic(double t, int n);

   static void arcsin_values(int &n_data, double &x, double &fx);

   static double arctan_cordic(double x, double y, int n);

   static void arctan_values(int &n_data, double &x, double &fx);

   static double cbrt_cordic(double x, int n);

   static void cbrt_values(int &n_data, double &x, double &fx);

   static void cos_values(int &n_data, double &x, double &fx);

   static void cossin_cordic(double beta, int n, double &c, double &s);

   static double exp_cordic(double x, int n);

   static void exp_values(int &n_data, double &x, double &fx);

   static int i4_huge();

   static int i4_min(int i1, int i2);

   static double ln_cordic(double x, int n);

   static void ln_values(int &n_data, double &x, double &fx);

   static double multiply_cordic(double x, double y);

   static double r8_uniform_01(int &seed);

   static void sin_values(int &n_data, double &x, double &fx);

   static double sqrt_cordic(double x, int n);

   static void sqrt_values(int &n_data, double &x, double &fx);

   static double tan_cordic(double beta, int n);

   static void tan_values(int &n_data, double &x, double &fx);

   static void timestamp();

};


#endif
