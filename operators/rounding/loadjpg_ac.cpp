/*
 * loadjpg_ac.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: sglee
 */

/*
 * loadjpg.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: sglee
 */

/***************************************************************************/
/*                                                                         */
/*  File: loadjpg.cpp                                                      */
/*  Author: bkenwright@xbdev.net                                           */
/*  Date: 19-01-06                                                         */
/*                                                                         */
/*  Revised: 26-07-07                                                      */
/*                                                                         */
/***************************************************************************/
/*
    About:
	Simplified jpg/jpeg decoder image loader - so we can take a .jpg file
	either from memory or file, and convert it either to a .bmp or directly
	to its rgb pixel data information.

	Simplified, and only deals with basic jpgs, but it covers all the
	information of how the jpg format works :)

	Can be used to convert a jpg in memory to rgb pixels in memory.

	Or you can pass it a jpg file name and an output bmp filename, and it
	loads and writes out a bmp file.

	i.e.
	ConvertJpgFile("cross.jpg", "cross.bmp")
*/
/***************************************************************************/

#pragma once

#include <stdio.h>		// sprintf(..), fopen(..)
#include <stdlib.h>		// sprintf(..), fopen(..)
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <string.h>		// memset(..)
#include <math.h>		// sqrt(..), cos(..)

#include "loadjpg.h"
#include "jpeg_helper.h"

#include "loa.h"
#include "bam.h"
#include "loadjpg_ac.h"
const float PI = 3.14f;
#include "fft.h"

void PerformIDCT2_AC(int outBlock[8][8], const int inBlock[8][8])
{
  float sum=0;

  // this is for the baseline hardware
  int cos_bi1 = 1;
  int cos_bf1 = 15;
  int cos_bi2 = 1;
  int cos_bf2 = 15;
  int s0_bi = 10;
  int s0_bf = 6;
  int s1_bi = 10;
  int s1_bf = 6;

  // approximation setup
  loa s0a(16, 0, 0, 0, 0);
  loa s1a(16, 0, 0, 0, 0);
  bam s0m(16, 0, 0, 0, 0, 0);
  bam s1m(16, 0, 0, 0, 0, 0);

  int coef1_fxp[8][8];
  int coef2_fxp[8][8];

  // generate first coefficient matrix: C(v)*cos(((2*y+1) * v * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  coef1_fxp[i1][i2] = float2int_idct (C(i2)*cosf(((2*i1+1) * i2 * PI) / 16), cos_bi1, cos_bf1);
		  printf ("SGLEE COEF1: %d, %d, %d", i1, i2, coef1_fxp[i1][i2]);
	  }
  }
  // generate second coefficient matrix: C(u)*cos(((2*x+1) * u * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  coef2_fxp[i1][i2] = float2int_idct(C(i2)*cosf(((2*i1+1) * i2 * PI) / 16), cos_bi2, cos_bf2);
		  printf ("SGLEE COEF2: %d, %d, %d", i1, i2, coef2_fxp[i1][i2]);
	  }
  }

  int temp1_fxp[8][8];
  int temp2_fxp[8][8];
  int tmp_fxp;

  // first matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp1_fxp[i0][i1]=0;
    	  for (int i2=0;i2<8;i2++) {
//		      tmp_fxp = coef1_fxp[i0][i2]*inBlock[i1][i2];
		      tmp_fxp = s0m.calc(coef1_fxp[i0][i2], (short)inBlock[i1][i2]);

		      struct q_format r_tmp;
		      struct q_format r_int;
		      r_tmp.val = tmp_fxp*2;
		      r_tmp.bi = 32-cos_bf1-1;
		      r_tmp.bf = cos_bf1+1;
		      r_int = rnd_clp (r_tmp, s0_bi, s0_bf);
		      tmp_fxp = r_int.val;
		      temp1_fxp[i0][i1] = s0a.calc(temp1_fxp[i0][i1], tmp_fxp);
 	      }
      }
  }

  // second matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp2_fxp[i0][i1]=0;
	      for (int i2=0;i2<8;i2++) {
	    	  tmp_fxp = s1m.calc(coef2_fxp[i0][i2], temp1_fxp[i1][i2]);

		      struct q_format r_tmp;
		      struct q_format r_int;
		      r_tmp.val = tmp_fxp*2;
		      r_tmp.bi = 32-s0_bf-cos_bf2-1;
		      r_tmp.bf = s0_bf+cos_bf2+1;
		      r_int = rnd_clp (r_tmp, s1_bi, s1_bf);
		      tmp_fxp = r_int.val;
		      temp2_fxp[i0][i1] = s1a.calc(temp2_fxp[i0][i1], tmp_fxp);
	      }

	      struct q_format r_tmp;
	      struct q_format r_int;
	      r_tmp.val = temp2_fxp[i0][i1]*2;
	      r_tmp.bi = 32-s1_bf-1;
	      r_tmp.bf = s1_bf+1;
	      r_int = rnd_clp (r_tmp, 16, -2);
	      tmp_fxp = r_int.val;
	      temp2_fxp[i0][i1] = tmp_fxp;
      }
  }

  for (int i1=0;i1<8;i1++) {
      for (int i2=0;i2<8;i2++) {
    	  outBlock[i1][i2]= (short)temp2_fxp[i1][i2];
      }
  }

}

void PerformIDCT3_AC(int outBlock[8][8], const int inBlock[8][8], const int (*coef1)[8], const int (*coef2)[8])
{
  float sum=0;

  // this is for the baseline hardware
  int cos_bi1 = 1;
  int cos_bf1 = 15;
  int cos_bi2 = 1;
  int cos_bf2 = 15;
  int s0_bi = 10;
  int s0_bf = 5;
  int s1_bi = 10;
  int s1_bf = 6;

  // approximation setup
  loa s0a(32, 0, 0, 0, 0);
  loa s1a(32, 0, 0, 0, 0);
  bam s0m(16, 0, 0, 0, 0, 0);
  bam s1m(16, 0, 0, 0, 0, 0);

  int temp1_fxp[8][8];
  int temp2_fxp[8][8];
  int tmp_fxp;

  // first matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp1_fxp[i0][i1]=0;
    	  for (int i2=0;i2<8;i2++) {
//		      tmp_fxp = coef1_fxp[i0][i2]*inBlock[i1][i2];
		      tmp_fxp = s0m.calc(coef1[i0][i2], (short)inBlock[i1][i2]);

	    	  vec_y_s0a[i0*8+i1].push_back((ACINT)tmp_fxp);
		      temp1_fxp[i0][i1] = s0a.calc(temp1_fxp[i0][i1], tmp_fxp);
 	      }

    	  struct q_format r_tmp;
    	  struct q_format r_int;
    	  r_tmp.val = temp1_fxp[i0][i1]*2;
    	  r_tmp.bi = 32-cos_bf1-1;
    	  r_tmp.bf = cos_bf1+1;
    	  r_int = rnd_clp (r_tmp, s0_bi, s0_bf);
    	  temp1_fxp[i0][i1] = r_int.val;

    	  vec_y_lump2.push_back((ACINT)temp1_fxp[i0][i1]);
    	  vec_y2[8*i0+i1].push_back((ACINT)temp1_fxp[i0][i1]);
      }
  }

  // second matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp2_fxp[i0][i1]=0;
	      for (int i2=0;i2<8;i2++) {
	    	  tmp_fxp = s1m.calc(coef2[i0][i2], (short)temp1_fxp[i1][i2]);

		      vec_s1a_lump.push_back((ACINT)tmp_fxp);
		      temp2_fxp[i0][i1] = s1a.calc(temp2_fxp[i0][i1], tmp_fxp);
	      }

	      struct q_format r_tmp;
	      struct q_format r_int;
	      r_tmp.val = temp2_fxp[i0][i1]*2;
	      r_tmp.bi = 32-s0_bf-cos_bf2-1;
	      r_tmp.bf = s0_bf+cos_bf2+1;
	      r_int = rnd_clp (r_tmp, 32-s0_bf-cos_bf2-1, -2);
//	      cout << "SGLEE TEST: " << r_tmp.val << ", " << r_int.val << endl;
	      tmp_fxp = r_int.val;
//	      cout << "SGLEE TEST: " << hex << tmp_fxp << dec << endl;
	      temp2_fxp[i0][i1] = tmp_fxp;
      }
  }

  for (int i1=0;i1<8;i1++) {
      for (int i2=0;i2<8;i2++) {
    	  outBlock[i1][i2]= (short)temp2_fxp[i1][i2];
      }
  }

}

void PerformIDCT4_AC(int outBlock[8][8], const int inBlock[8][8], const int (*coef1)[8], const int (*coef2)[8])
{
  float sum=0;

  // this is for the baseline hardware
  int cos_bi1 = 1;
  int cos_bf1 = 15;
  int cos_bi2 = 1;
  int cos_bf2 = 15;
  int s0_bi = 10;
  int s0_bf = 5;
//  int s1_bi = 10;
//  int s1_bf = 6;

  // approximation setup
  loa s0a(32, 0, 0, 0, 0);
  loa s1a(32, 0, 0, 0, 0);
  bam s0m(16, 0, 0, 0, 0, false);
  bam s1m(16, 0, 0, 0, 0, false);

  int temp1_fxp[8][8];
  int temp2_fxp[8][8];
  int tmp_fxp;

  // first matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp1_fxp[i0][i1]=0;
    	  for (int i2=0;i2<8;i2++) {
//		      tmp_fxp = s0m.calc((short)coef1[i0][i2], (short)inBlock[i1][i2]);
		      tmp_fxp = s0m.calc((short)inBlock[i1][i2], (short)coef1[i0][i2]);
//      cout << "SGLEE: " << tmp_fxp << ", " << s0m.calc_ref((short)coef1[i0][i2], (short)inBlock[i1][i2]) << endl;
	    	  vec_y_s0a[i0*8+i1].push_back((ACINT)tmp_fxp);
		      temp1_fxp[i0][i1] = s0a.calc(temp1_fxp[i0][i1], tmp_fxp);
 	      }

    	  struct q_format r_tmp;
    	  struct q_format r_int;
    	  r_tmp.val = temp1_fxp[i0][i1]*2;
    	  r_tmp.bi = 32-cos_bf1-1;
    	  r_tmp.bf = cos_bf1+1;
    	  r_int = rnd_clp (r_tmp, s0_bi, s0_bf);
    	  temp1_fxp[i0][i1] = r_int.val;

    	  vec_y_lump2.push_back((ACINT)temp1_fxp[i0][i1]);
    	  vec_y2[8*i0+i1].push_back((ACINT)temp1_fxp[i0][i1]);
      }
  }

  // second matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp2_fxp[i0][i1]=0;
	      for (int i2=0;i2<8;i2++) {
	    	  tmp_fxp = s1m.calc((short)temp1_fxp[i1][i2], (short)coef2[i0][i2]);

		      vec_s1a_lump.push_back((ACINT)tmp_fxp);
		      temp2_fxp[i0][i1] = s1a.calc(temp2_fxp[i0][i1], tmp_fxp);
	      }

	      struct q_format r_tmp;
	      struct q_format r_int;
	      r_tmp.val = temp2_fxp[i0][i1]*2;
	      r_tmp.bi = 32-s0_bf-cos_bf2-1;
	      r_tmp.bf = s0_bf+cos_bf2+1;
	      r_int = rnd_clp (r_tmp, 32-s0_bf-cos_bf2-1, -2);
//	      cout << "SGLEE TEST: " << r_tmp.val << ", " << r_int.val << endl;
	      tmp_fxp = r_int.val;
//	      cout << "SGLEE TEST: " << hex << tmp_fxp << dec << endl;
	      temp2_fxp[i0][i1] = tmp_fxp;
      }
  }

  for (int i1=0;i1<8;i1++) {
      for (int i2=0;i2<8;i2++) {
    	  outBlock[i1][i2]= (short)temp2_fxp[i1][i2];
      }
  }

}

