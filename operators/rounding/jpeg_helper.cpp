/*
 * jpeg_helper.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: sglee
 */


#include <stdio.h>		// sprintf(..), fopen(..)
#include <stdlib.h>		// sprintf(..), fopen(..)
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <string.h>		// memset(..)
#include <math.h>		// sqrt(..), cos(..)

#include "jpeg_helper.h"
#include "fft.h"

/***************************************************************************/

// Floating to fixed point number
// Both input & output are 'double' but
// First, convert double to int. Input is rounded and clipped with bi and bf and
// then converted to double again to make the following calculation unchanged.
float float2fix_idct (float x, int bi, int bf)
{
	struct q_format r_tmp;
	struct q_format r_int;

	float result;

	// Convertion to integer
	// For accurate rounding, need one more bit
	r_tmp.val = (int) (x * pow (2, bf+1));

	r_tmp.bi = bi+1;
	r_tmp.bf = bf+1;

	// Rounding and Clipping
	r_int = rnd_clp (r_tmp, bi, bf);

	// Convertion to double again
	result = ((double) r_int.val) / pow(2, bf);

	return result;

}

int float2int_idct (float x, int bi, int bf)
{
	struct q_format r_tmp;
	struct q_format r_int;

	float result;

	// Convertion to integer
	// For accurate rounding, need one more bit
	r_tmp.val = (int) (x * pow (2, bf+1));

	r_tmp.bi = bi+1;
	r_tmp.bf = bf+1;

	// Rounding and Clipping
	r_int = rnd_clp (r_tmp, bi, bf);

	return r_int.val;

}

float measure_power (const int val[8][8]) {
  double pwr = 0;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      pwr += (float) (val[u][v]*val[u][v]);
      // printf ("%d\n", val[u][v]);
    }
  }
  return (float) pwr/64.0;

}

double measure_power (signed char *val) {
  double pwr = 0;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      pwr += (float) ((val[8*u+v])*(val[8*u+v]));
      // printf ("%d\n", val[u][v]);
    }
  }
  return (float) pwr/64.0;

}

float measure_power (const float val[8][8]) {
  double pwr = 0;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      pwr += val[u][v]*val[u][v];
    }
  }
  return (float) pwr/64.0;

}

float measure_mean_var (const int val[8][8], float &mean, float &var) {
  double pwr = 0;
  double var3 = 0;
  double mean1 = 0;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      pwr += (float) (val[u][v]*val[u][v]);
      mean1 += val[u][v];
    }
  }
  mean = mean1 / 64.0;

  double diff;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      diff = mean - val[u][v];
      var3 += diff * diff;
    }
  }

  var = var3 / 64.0;

  return (float) pwr/64.0;

}

int C_fixed(int u, int nbit)
{
    if (u == 0)
         return (int)((1.0f/sqrtf(2))*pow(2,nbit));
    else
         return (int)(1.0f*pow(2,nbit));
}



