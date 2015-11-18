//
//  quant.cpp
//  jpegEncoder
//
//  Created by Dongwook Lee on 10/26/13.
//  Copyright (c) 2013 Dongwook Lee. All rights reserved.
//

#include "jpegEncoder.h"
void quant_y(bool qen, unsigned char qtable[64], freq_t inqueue[64], freq_t outqueue[64], bool& hen)
{
  int i,  m,  q,  o;
  freq_t buffer[64];
  unsigned char ZigzagIndex[] =
	{0,  1,  5,  6, 14, 15, 27, 28,
    2,  4,  7, 13, 16, 26, 29, 42,
    3,  8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63};
	hen=qen;
  if(qen==false) return;

  for (i=0; i<64; i++)    {
#pragma HLS UNROLL factor=4
	#pragma HLS pipeline
    m = inqueue[i];

    q=qtable[i];

    if (m > 0)
    {
      o = (m + q/2) / q;
    }
    else
    {
      o = (m - q/2) / q;
    }
    buffer[ZigzagIndex[i]]=o;
  }
	for(int i=0; i<64; i++)
	{
		#pragma HLS pipeline
		outqueue[i]=buffer[i];
	}
}
void quant_u(bool qen, unsigned char qtable[64], freq_t inqueue[64], freq_t outqueue[64],bool& hen)
{
  int i,  m,  q,  o;
  freq_t buffer[64];
  unsigned char ZigzagIndex[] =
	{0,  1,  5,  6, 14, 15, 27, 28,
    2,  4,  7, 13, 16, 26, 29, 42,
    3,  8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63};
	hen=qen;
  if(qen==false) return;
  for (i=0; i<64; i++)    {
	#pragma HLS pipeline
#pragma HLS UNROLL factor=4
    m = inqueue[i];

    q=qtable[i];

    if (m > 0)
    {
      o = (m + q/2) / q;
    }
    else
    {
      o = (m - q/2) / q;
    }
    buffer[ZigzagIndex[i]]=o;
  }
	for(int i=0; i<64; i++)
	{
#pragma HLS pipeline
		outqueue[i]=buffer[i];
	}
}
void quant_v(bool qen, unsigned char qtable[64], freq_t inqueue[64], freq_t outqueue[64], bool& hen)
{
  int i,  m,  q,  o;
  freq_t buffer[64];
  unsigned char ZigzagIndex[] =
	{0,  1,  5,  6, 14, 15, 27, 28,
    2,  4,  7, 13, 16, 26, 29, 42,
    3,  8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63};
	hen=qen;
  if(qen==false) return;
  for (i=0; i<64; i++)    {
	#pragma HLS pipeline
#pragma HLS UNROLL factor=4
    m = inqueue[i];

    q=qtable[i];

    if (m > 0)
    {
      o = (m + q/2) / q;
    }
    else
    {
      o = (m - q/2) / q;
    }
    buffer[ZigzagIndex[i]]=o;
  }
	for(int i=0; i<64; i++)
	{
#pragma HLS pipeline
		outqueue[i]=buffer[i];
	}
}



