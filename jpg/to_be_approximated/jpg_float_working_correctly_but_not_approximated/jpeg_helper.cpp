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
#include <iostream>

#include "jpeg_helper.h"
// #include "fft.h"

// Rounding and Clipping of integer number
// Rounding first, clipping later
// e.g. if bf = 2 & bi = 2, the range of integer number is -7 ~ 7
// There's no -8.

using namespace std;

q_format rnd_clp (const q_format &x, const int &bi, const int &bf)
{
  q_format tmp_rnd;
  q_format result;

	// Rounding first
	if (x.bf < bf)
		cout << "Rounding Error!\n" << endl;
	// No rounding
	else if (x.bf == bf)
	{
		tmp_rnd.val = x.val;
		tmp_rnd.bi = x.bi;
		tmp_rnd.bf = x.bf;
	}
	else
	{
		tmp_rnd.bi = x.bi;
		tmp_rnd.bf = bf;
		int x_abs = abs(x.val);
		// Actual rounding
//		if ((x.val & (0x1 << (x.bf - bf - 1)) != 0) && ((x.val & 0x80000000) == 0))
		if (((x_abs & (0x1 << (x.bf - bf - 1))) != 0) && ((x_abs & 0x80000000) == 0))
//			tmp_rnd.val = ((signed)(x.val & (0xffffffff << (x.bf - bf))) >> (x.bf-bf)) + 1;
			tmp_rnd.val = ((x_abs & (0xffffffff << (x.bf - bf))) >> (x.bf-bf)) + 1;
		else
//			tmp_rnd.val = ((signed)(x.val & (0xffffffff << (x.bf - bf))) >> (x.bf-bf));
			tmp_rnd.val = ((x_abs & (0xffffffff << (x.bf - bf))) >> (x.bf-bf));

		tmp_rnd.val = x.val > 0 ? tmp_rnd.val : -(tmp_rnd.val);
	}

	// Then Clipping
	if (tmp_rnd.bi < bi)
		cout  << "Clipping Error!\n" << endl;
	// No Clipping
	else if (tmp_rnd.bi == bi)
	{
		result.val = tmp_rnd.val;
		result.bi = tmp_rnd.bi;
		result.bf = tmp_rnd.bf;
	}
	else
	{
		result.bi = tmp_rnd.bi;
		result.bf = tmp_rnd.bf;
		// Actual Clipping
		// Range is 2^(bi+bf)-1 >= result.val >= -2^(bi+bf)+1
		// For example, if bi+bf = 10, 511 >= result.val >= -511
		if ((tmp_rnd.val & (0x1 << (tmp_rnd.bi + tmp_rnd.bf))) != 0) // negative number
		{
			if (tmp_rnd.val <= (-1)*pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1))
			{
				result.val = (-1)*pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1) + 1;
				//printf ("Clipping! %d, %d \n", tmp_rnd.val, result.val);
			}
			else
				result.val = tmp_rnd.val;
		}
		else // positive number
		{
			if (tmp_rnd.val >= pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1))
			{
				result.val = pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1) - 1;
				//printf ("Clipping! %d, %d \n", tmp_rnd.val, result.val);
			}
			else
				result.val = tmp_rnd.val;
		}
	}

	return result;

}
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



