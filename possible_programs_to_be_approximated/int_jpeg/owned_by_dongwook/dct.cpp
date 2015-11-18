/*!
  \file dct.cpp
  DCT transform from JPEG example.
!*/
#include "jpegEncoder.h"
#include <stdio.h>

#include <stdlib.h>


#define NO_MULTIPLY
#ifdef NO_MULTIPLY
#define LS(r,s) ((r) << (s))
#define RS(r,s) ((r) >> (s))       /* Caution with rounding... */
#else
#define LS(r,s) ((r) * (1 << (s)))
#define RS(r,s) ((r) / (1 << (s))) /* Correct rounding */
#endif

#define MSCALE(expr)  RS((expr),9)

/* Cos constants */

#define c1d4 362L
#define c1d8 473L
#define c3d8 196L
#define c1d16 502L
#define c3d16 426L
#define c5d16 284L
#define c7d16 100L

freq_t CLIP(int tmp)
{
   freq_t tval=tmp;
   tval = (((tval < 0) ? (tval - 4) : (tval + 4)) / 8);
   if (tval < -1023)
   {
     tval = -1023;
   }
   else if (tval > 1023)
   {
     tval = 1023;
   }
	 return tval;
}


#define __CHANDCT__
#ifdef __CHANDCT__
void dct(
	bool dcten, 
	yuv_t inqueue[64], 
	freq_t outqueue[64], 
	bool&qen)
{


#pragma HLS function_instantiate variable=color

  int i;// aptr;
  short a0,  a1,  a2,  a3;
  short b0,  b1,  b2,  b3;
  short tb0,  tb1,  ta0, ta1,  ta2,  ta3;
  short c0,  c1,  c2,  c3;
  short v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7;
  //yuv_t in_block[64];
  short tmp[64];
	qen=dcten;
  if(dcten==false) return;
  for (i = 0; i < 8; i++)
  {

	#pragma HLS PIPELINE
    //aptr = i;
    v0 = inqueue[i*8+0];
    v1 = inqueue[i*8+1];
    v2 = inqueue[i*8+2];
    v3 = inqueue[i*8+3];
    v4 = inqueue[i*8+4];
    v5 = inqueue[i*8+5];
    v6 = inqueue[i*8+6];
    v7 = inqueue[i*8+7];
    a0 = LS((v0 + v7),  2); c3 = LS((v0 - v7),  2);
    a1 = LS((v1 + v6),  2); c2 = LS((v1 - v6),  2);
    a2 = LS((v2 + v5),  2); c1 = LS((v2 - v5),  2);
    a3 = LS((v3 + v4),  2); c0 = LS((v3 - v4),  2);
    b0 = a0 + a3;
    b1 = a1 + a2;
    b2 = a1 - a2;
    b3 = a0 - a3;
    tmp[i] = MSCALE(c1d4 * (b0 + b1));
    tmp[i + 32] = MSCALE(c1d4 * (b0 - b1));
    tmp[i + 16] = MSCALE((c3d8 * b2) + (c1d8 * b3));
    tmp[i + 48] = MSCALE((c3d8 * b3) - (c1d8 * b2));
    b0 = MSCALE(c1d4 * (c2 - c1));
    b1 = MSCALE(c1d4 * (c2 + c1));
    a0 = c0 + b0;
    a1 = c0 - b0;
    a2 = c3 - b1;
    a3 = c3 + b1;
    tmp[i + 8] = MSCALE((c7d16 * a0) + (c1d16 * a3));
    tmp[i + 24] = MSCALE((c3d16 * a2) - (c5d16 * a1));
    tmp[i + 40] = MSCALE((c3d16 * a1) + (c5d16 * a2));
    tmp[i + 56] = MSCALE((c7d16 * a3) - (c1d16 * a0));
  }
  for (i = 0; i < 8; i++)
  {

	#pragma HLS PIPELINE
    //aptr = LS(i,  3);
    v0 = tmp[i*8+0];// aptr++;
    v1 = tmp[i*8+1]; //aptr++;
    v2 = tmp[i*8+2]; //aptr++;
    v3 = tmp[i*8+3]; //aptr++;
    v4 = tmp[i*8+4];// aptr++;
    v5 = tmp[i*8+5]; //aptr++;
    v6 = tmp[i*8+6]; //aptr++;
    v7 = tmp[i*8+7];
    c3 = RS((v0 - v7),  1); a0 = RS((v0 + v7),  1);
    c2 = RS((v1 - v6),  1); a1 = RS((v1 + v6),  1);
    c1 = RS((v2 - v5),  1); a2 = RS((v2 + v5),  1);
    c0 = RS((v3 - v4),  1); a3 = RS((v3 + v4),  1);
    b0 = a0 + a3; b1 = a1 + a2; b2 = a1 - a2; b3 = a0 - a3;
    tb0 = MSCALE(c1d4 * (c2 - c1));
    tb1 = MSCALE(c1d4 * (c2 + c1));
    ta0 = c0 + tb0;
    ta1 = c0 - tb0;
    ta2 = c3 - tb1;
    ta3 = c3 + tb1;
    outqueue[i*8] 		=CLIP(MSCALE(c1d4 * (b0 + b1)));
    outqueue[i*8 + 4] =CLIP(MSCALE(c1d4 * (b0 - b1)));
    outqueue[i*8 + 2] =CLIP(MSCALE((c3d8 * b2) + (c1d8 * b3)));
    outqueue[i*8 + 6] =CLIP(MSCALE((c3d8 * b3) - (c1d8 * b2)));
    outqueue[i*8 + 1] =CLIP(MSCALE((c7d16 * ta0) + (c1d16 * ta3)));
    outqueue[i*8 + 3] =CLIP(MSCALE((c3d16 * ta2) - (c5d16 * ta1)));
    outqueue[i*8 + 5] =CLIP(MSCALE((c3d16 * ta1) + (c5d16 * ta2)));
    outqueue[i*8 + 7] =CLIP(MSCALE((c7d16 * ta3) - (c1d16 * ta0)));
	}
}
#else

void dct(yuv_queue_t& inqueue, freq_queue_t& outqueue, yuv_t color)
{
#pragma HLS PIPELINE
#pragma HLS function_instantiate variable=color

  
  int coeff[64] = {
    c1d4,  c1d4,  c1d4,  c1d4,  c1d4,  c1d4,  c1d4,  c1d4,
    c1d16,  c3d16,  c5d16,   c7d16,  -c7d16, -c5d16, -c3d16, -c1d16,
    c1d8,  c3d8, -c3d8, -c1d8, -c1d8, -c3d8,  c3d8,  c1d8,
    c3d16,  -c7d16, -c1d16, -c5d16,  c5d16,  c1d16,   c7d16, -c3d16,
    c1d4, -c1d4, -c1d4,  c1d4,  c1d4, -c1d4, -c1d4,  c1d4,
    c5d16, -c1d16,   c7d16,  c3d16, -c3d16,  -c7d16,  c1d16, -c5d16,
    c3d8, -c1d8,  c1d8, -c3d8, -c3d8,  c1d8, -c1d8,  c3d8,
    c7d16, -c5d16,  c3d16, -c1d16,  c1d16, -c3d16,  c5d16,  -c7d16
  };
  yuv_t input_buf[8];
  //int output[64];
  int temp[64];
  int tmp;
  int dct_value;
  int i, ii, j,k;
  /*for (i=0; i<64;i++)
  {
    input[i]=inqueue.read();
    FILE_LOG(logDEBUG)<<"DCT_INPUT["<<i<<"]: "<<(int)input[i]<<endl;
    //printf("in %d\n",in_block[i]);
  }*/

  for ( i=0; i < 8; ++i )
  {
    for (j=0; j < 8; j++ )
    {
      input_buf[j]=inqueue.read();
      //FILE_LOG(logDEBUG)<<"DCT_INPUT["<<i*8+j<<"]: "
     //   <<(int)input_buf[i*8+j]<<endl;
    }

    for (j=0; j < 8; ++j )
    {
      tmp = 0;
	  #pragma HLS PIPELINE
      for (int k=0; k < 8; ++k )
      {
        tmp = tmp + (input_buf[k] * coeff[j*8+k]);
      }
      temp[j*8+i] = (tmp);
    }
  }

  for ( ii=0 ; ii < 8; ++ii )
  {
    dct_label2:for ( j=0; j < 8; ++j )
    {
      dct_value = 0;
		#pragma HLS PIPELINE
      dct_label1:for ( k=0 ; k < 8 ; ++k )
      {
        dct_value = dct_value + (coeff[ii*8+k] * temp[j*8+k]);
      }
      //output[ii*8+j] = RS(dct_value,17);
      //freq_t tmp=output[ii*8+j] ;
      outqueue.write(tval);
 //     FILE_LOG(logDEBUG)<<"DCT_OUTPUT["<<ii*8+j<<"]: "<<tmp<<endl;
      //printf("out %d\n",out_block[i]);
    }
  }
  
}
#endif
//! unit test for dct block
/*
void test_dct()
{
  yuv_queue_t inqueue;
  freq_queue_t outqueue;

  for(int i=0; i<64; i++)
  {
    yuv_t tmp=i;
    inqueue.write(tmp);
  }
  dct(inqueue,outqueue,Y_DATA);
}*/
