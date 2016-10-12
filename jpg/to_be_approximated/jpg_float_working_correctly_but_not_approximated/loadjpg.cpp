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

//#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>		// sprintf(..), fopen(..)
#include <stdlib.h>		// sprintf(..), fopen(..)
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <string.h>		// memset(..)
#include <math.h>		// sqrt(..), cos(..)

//#include "ac_types.h"
#include "loadjpg.h"
//#include "loadjpg_ac.h"
#include "jpeg_helper.h"

using namespace std;
extern ofstream resultFile;
#define JPG_SPEEDUP

//Saving debug information to a log file
void dprintf(const char *fmt, ...)
{

}// End dbool ADD_UPPER;printf(..)

/*
__forceinline void dprintf(const char *fmt, ...)
{
	va_list parms;
	char buf[256];

	// Try to print in the allocated space.
	va_start(parms, fmt);
	vsprintf (buf, fmt, parms);
	va_end(parms);

	// Write the information out to a txt file
	FILE *fp = fopen("output.txt", "a+");
	fprintf(fp, "%s",  buf);
	fclose(fp);

}// End dprintf(..)
*/

/***************************************************************************/

#define DQT 	 0xDB	// Define Quantization Table
#define SOF 	 0xC0	// Start of Frame (size information)
#define DHT 	 0xC4	// Huffman Table
#define SOI 	 0xD8	// Start of Image
#define SOS 	 0xDA	// Start of Scan
#define EOI 	 0xD9	// End of Image, or End of File
#define APP0	 0xE0

#define BYTE_TO_WORD(x) (((x)[0]<<8)|(x)[1])


const int HUFFMAN_TABLES	=	4;
const int COMPONENTS		=	4;

const int  cY	 = 1;
const int  cCb	 = 2;
const int  cCr	 = 3;


static int ZigZagArray[64] =
{
	0,   1,   5,  6,   14,  15,  27,  28,
	2,   4,   7,  13,  16,  26,  29,  42,
	3,   8,  12,  17,  25,  30,  41,  43,
	9,   11, 18,  24,  31,  40,  44,  53,
	10,  19, 23,  32,  39,  45,  52,  54,
	20,  22, 33,  38,  46,  51,  55,  60,
	21,  34, 37,  47,  50,  56,  59,  61,
	35,  36, 48,  49,  57,  58,  62,  63,
};

/***************************************************************************/


struct stBlock
{
     int value;					// Decodes to.
     int length;				// Length in bits.
     unsigned short int code;	// 2 byte code (variable length)
};

/***************************************************************************/


struct stHuffmanTable
{
	unsigned char	m_length[17];		// 17 values from jpg file,
										// k =1-16 ; L[k] indicates the number of Huffman codes of length k
	unsigned char	m_hufVal[257];		// 256 codes read in from the jpeg file

	int				m_numBlocks;
	stBlock			m_blocks[1024];
};


struct stComponent
{
  unsigned int			m_hFactor;
  unsigned int			m_vFactor;
  float *				m_qTable;			// Pointer to the quantisation table to use
  stHuffmanTable*		m_acTable;
  stHuffmanTable*		m_dcTable;
  short int				m_DCT[65];			// DCT coef
  int					m_previousDC;
};


struct stJpegData
{
	unsigned char*		m_rgb;				// Final Red Green Blue pixel data
	unsigned int		m_width;			// Width of image
	unsigned int		m_height;			// Height of image

	const unsigned char*m_stream;			// Pointer to the current stream
	int					m_restart_interval;

	stComponent			m_component_info[COMPONENTS];

	float				m_Q_tables[COMPONENTS][64];	// quantization tables
	stHuffmanTable		m_HTDC[HUFFMAN_TABLES];		// DC huffman tables
	stHuffmanTable		m_HTAC[HUFFMAN_TABLES];		// AC huffman tables

	// Temp space used after the IDCT to store each components
	unsigned char		m_Y[64*4];
	unsigned char		m_Cr[64];
	unsigned char		m_Cb[64];

	// Internal Pointer use for colorspace conversion, do not modify it !!!
	unsigned char *		m_colourspace;
};


/***************************************************************************/
//
//  Returns the size of the file in bytes
//
/***************************************************************************/
inline int FileSize(FILE *fp)
{
	long pos;
	fseek(fp, 0, SEEK_END);
	pos = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return pos;
}

/***************************************************************************/

// Clamp our integer between 0 and 255
inline unsigned char Clamp(int i)
{
	if (i<0)
		return 0;
	else if (i>255)
		return 255;
	else
		return i;
}

/***************************************************************************/

#ifdef JPG_SPEEDUP

#define GenHuffCodes( num_codes, arr, huffVal )				\
{															\
     int hufcounter = 0;									\
     int codelengthcounter = 1;								\
															\
     for(int cc=0; cc< num_codes; cc++)						\
     {														\
		 while ( arr[cc].length > codelengthcounter )		\
		 {													\
			hufcounter = hufcounter << 1;					\
			codelengthcounter++;							\
		 }													\
															\
		 arr[cc].code = hufcounter;							\
		 arr[cc].value = huffVal[cc];						\
		 hufcounter = hufcounter + 1;						\
	}														\
}															\

#else

void GenHuffCodes( int num_codes, stBlock* arr, unsigned char* huffVal )
{
     int hufcounter = 0;
     int codelengthcounter = 1;


     for(int cc=0; cc< num_codes; cc++)
     {
		 while ( arr[cc].length > codelengthcounter )
		 {
			hufcounter = hufcounter << 1;
			codelengthcounter++;
		 }

		 arr[cc].code = hufcounter;
		 arr[cc].value = huffVal[cc];
		 hufcounter = hufcounter + 1;
	}
}

#endif //JPG_SPEEDUP


float C(int u)
{
    if (u == 0)
         return (1.0f/sqrtf(2));
    else
         return 1.0f;
}


// sglee: this is the core of IDCT
int func(int x, int y, const int block[8][8])
{
	float PI = 3.14f;
  float sum=0;
  float sum_op_1 = 0;
  float sum_op_2 = 0;
    for( int u=0; u<8; u++)
    {
         for(int v=0; v<8; v++)
         {
             sum_op_1 = sum;
             sum_op_2 = ( C(u) * C(v) ) * block[u][v] * cosf( ((2*x+1) * u * PI) / 16)  * cosf( ((2*y+1) * v * PI) / 16);
              
             resultFile<<"--"<<block[u][v]<<", "<<( C(u) * C(v) ) * cosf( ((2*x+1) * u * PI) / 16)  * cosf( ((2*y+1) * v * PI) / 16)<<endl;
             resultFile<<sum_op_1<<","<<sum_op_2<<endl; 
             sum = sum_op_1 + sum_op_2;
         }
    }

    return (int) ((1.0/4.0) * sum);
}

int func_fixed(int x, int y, const int block[8][8], float &cos_pwr, float &block_pwr)
{

  int f1, f2, f3, f4;
  f1 = 0;
  f2 = 10;
  f3 = 10;
  f4 = 10;
  float PI = 3.14f;

//  const float PI = 3.14f;
  float cos1;
  float cos2;
  float cos_fx1;
  float cos_fx2;
  float cos_fl;
  float tmp1;
  float tmp1_fx;
  float tmp2;
  int block_bf = 8-f1; // actually, integer bits, should be smaller than 8
  int cos_bi1 = 1;
  int cos_bf1 = f2;
  int tmp_bf = f3;
  int cos_bi2 = 1;
  int cos_bf2 = f4;
  float block_fl;
  int block_fixed;
  float sum_fl = 0;

  float pwr_cos = 0;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      cos1 = 0.5*C(u)*cosf( ((2*x+1) * u * PI) / 16);
      cos2 = 0.5*C(v)*cosf( ((2*y+1) * v * PI) / 16);
      cos_fx1 = float2fix_idct (cos1, cos_bi1, cos_bf1);
      cos_fx2 = float2fix_idct (cos2, cos_bi2, cos_bf2);

      block_fl = (double)(block[u][v])/256.0;
      block_fl = float2fix_idct (block_fl, 8, block_bf);
      block_fixed = block_fl * 256;

      tmp1 = block_fixed * cos_fx1;
      tmp1_fx = float2fix_idct (tmp1, 12, tmp_bf);
      tmp2 = tmp1_fx * cos_fx2;

      sum_fl += tmp2;

    }
  }

  block_pwr = 0.0; // measure_power(block);
  cos_pwr = 0.0;

  return (int) (sum_fl);
}

/*
int func_ac(int x, int y, const int block[8][8], float &cos_pwr, float &block_pwr)
{
//  const float PI = 3.14f;
  float cos1;
  float cos2;
  float cos_fx1;
  float cos_fx2;
  float cos_fl;
  float tmp1;
  float tmp1_fx;
  float tmp2;
  int block_bf = 8-f1; // actually, integer bits, should be smaller than 8
  int cos_bi1 = 1;
  int cos_bf1 = f2;
  int tmp_bf = f3;
  int cos_bi2 = 1;
  int cos_bf2 = f4;
  float block_fl;
  int block_fixed;
  float sum_fl = 0;

  float pwr_cos = 0;
  for( int u=0; u<8; u++)
  {
    for(int v=0; v<8; v++)
    {
      cos1 = 0.5*C(u)*cosf( ((2*x+1) * u * PI) / 16);
      cos2 = 0.5*C(v)*cosf( ((2*y+1) * v * PI) / 16);
      cos_fx1 = float2fix_idct (cos1, cos_bi1, cos_bf1);
      cos_fx2 = float2fix_idct (cos2, cos_bi2, cos_bf2);

      block_fl = (double)(block[u][v])/256.0;
      block_fl = float2fix_idct (block_fl, 8, block_bf);
      block_fixed = block_fl * 256;

      tmp1 = block_fixed * cos_fx1;
      tmp1_fx = float2fix_idct (tmp1, 12, tmp_bf);
      tmp2 = tmp1_fx * cos_fx2;

      sum_fl += tmp2;

    }
  }

  block_pwr = 0.0; // measure_power(block);
  cos_pwr = 0.0;

  return (int) (sum_fl);
}
*/

void PerformIDCT(int outBlock[8][8], const int inBlock[8][8])
{
	for(int y=0; y<8; y++)
	{
		for(int x=0; x<8; x++)
		{
			outBlock[x][y]  =  func( x, y, inBlock);
		}
	}
}

/*
void PerformIDCT2(int outBlock[8][8], const int inBlock[8][8])
{
//  const float PI = 3.14f;
  float sum=0;

  float coef1[8][8];
  float coef2[8][8];
  // generate first coefficient matrix: C(v)*cos(((2*y+1) * v * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  coef1[i1][i2] = C(i2)*cosf(((2*i1+1) * i2 * PI) / 16);
	  }
  }
  // generate second coefficient matrix: C(u)*cos(((2*x+1) * u * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  coef2[i1][i2] = C(i2)*cosf(((2*i1+1) * i2 * PI) / 16);
	  }
  }

  float temp1[8][8];
  float temp2[8][8];

  // first matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp1[i0][i1]=0;
    	  for (int i2=0;i2<8;i2++) {
		      temp1[i0][i1] += coef1[i0][i2]*inBlock[i1][i2];
	      }
      }
  }

  // second matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp2[i0][i1]=0;
	      for (int i2=0;i2<8;i2++) {
	    	  temp2[i0][i1] += coef2[i0][i2]*temp1[i1][i2];
	      }
      }
  }

  for (int i1=0;i1<8;i1++) {
      for (int i2=0;i2<8;i2++) {
    	  outBlock[i1][i2]= (int)(1.0/4.0*temp2[i1][i2]);
      }
  }


}

void PerformIDCT2_REF(int outBlock[8][8], const int inBlock[8][8])
{
  const float PI = 3.14f;
  float sum=0;

  int cos_bi1 = 1;
  int cos_bf1 = 15;
  int cos_bi2 = 1;
  int cos_bf2 = 15;
  int s0_bi = 10;
  int s0_bf = 6;
  int s1_bi = 10;
  int s1_bf = 6;

  float coef1[8][8];
  float coef2[8][8];
  // generate first coefficient matrix: C(v)*cos(((2*y+1) * v * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  coef1[i1][i2] = float2fix_idct(C(i2)*cosf(((2*i1+1) * i2 * PI) / 16), cos_bi1, cos_bf1);
	  }
  }
  // generate second coefficient matrix: C(u)*cos(((2*x+1) * u * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  coef2[i1][i2] = float2fix_idct(C(i2)*cosf(((2*i1+1) * i2 * PI) / 16), cos_bi2, cos_bf2);
	  }
  }

  float temp1[8][8];
  float temp2[8][8];
  float tmp;

  // first matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp1[i0][i1]=0;
    	  float tmp2 = 0;
    	  for (int i2=0;i2<8;i2++) {
		      tmp = float2fix_idct(coef1[i0][i2]*inBlock[i1][i2], s0_bi, s0_bf);
		      temp1[i0][i1] += tmp;
 //   		  cout << "SGLEE PRE: " << tmp << ", " << temp1[i0][i1];
 //   		  tmp2 += coef1[i0][i2]*inBlock[i1][i2];
 //   		  cout << ", POST: " << coef1[i0][i2]*inBlock[i1][i2] << ", " << tmp2 << endl;
	      }
      }
  }

  // second matrix multiplication
  for (int i0=0;i0<8;i0++) {
      for (int i1=0;i1<8;i1++) {
    	  temp2[i0][i1]=0;
	      for (int i2=0;i2<8;i2++) {
	    	  tmp = float2fix_idct(coef2[i0][i2]*temp1[i1][i2], s1_bi, s1_bf);
//	    	  tmp = coef2[i0][i2]*temp1[i1][i2];
	    	  temp2[i0][i1] = temp2[i0][i1] + tmp;
	      }
      }
  }

  for (int i1=0;i1<8;i1++) {
      for (int i2=0;i2<8;i2++) {
    	  outBlock[i1][i2]= (int)(1.0/4.0*temp2[i1][i2]);
      }
  }


}
*/

void PerformIDCT_fixed(int outBlock[8][8], const int inBlock[8][8], float &cos_pwr, float &block_pwr, float &block_mean, float &block_var)
{
	for(int y=0; y<8; y++)
	{
		for(int x=0; x<8; x++)
		{
//#ifdef AC
//			outBlock[x][y]  =  func_ac( x, y, inBlock, cos_pwr, block_pwr);
//#else
			outBlock[x][y]  =  func_fixed( x, y, inBlock, cos_pwr, block_pwr);
//#endif
		}
	}

  block_pwr = measure_power(inBlock);
  float pwr = measure_mean_var (inBlock, block_mean, block_var);

}

/***************************************************************************/

void DequantizeBlock( int block[64], const float quantBlock[64] )
{
	for( int c=0; c<64; c++)
	{
		block[c] = (int)(block[c] * quantBlock[c]);
	}
}

/***************************************************************************/

void DeZigZag(int outBlock[64], const int inBlock[64])
{
	for(int i=0; i<64; i++)
	{
		outBlock[ i ] = inBlock[ZigZagArray[i]];
	}
}

/***************************************************************************/

void TransformArray(int outArray[8][8], const int inArray[64])
{
	int cc = 0;
	for( int y=0; y<8; y++)
	{
		for( int x=0; x<8; x++)
		{
			outArray[x][y]  =  inArray[cc];
			cc++;
		}
	}
}

/***************************************************************************/

void DumpDecodedBlock(int val[8][8])
{
	dprintf("# Decoded 8x8 Block#\n");
	for( int y=0; y<8; y++)
	{
		for( int x=0; x<8; x++)
		{
			dprintf("%2x ", val[x][y]);
		}
		dprintf("\n");
	}
}

/***************************************************************************/
int another_counter = 0;
inline void DecodeSingleBlock(stComponent *comp, unsigned char *outputBuf, int stride)
{
	
    another_counter++; 
    short* inptr    = comp->m_DCT;
	float* quantptr = comp->m_qTable;


	// Create a temp 8x8, i.e. 64 array for the data
	int data[64] = {0};

	// Copy our data into the temp array
	for (int i=0; i<64; i++)
	{
		data[i] = inptr[i];
	}

	// De-Quantize
	DequantizeBlock(data, quantptr);

	// De-Zig-Zag
	int block[64] = {0};
	DeZigZag(block, data);

	// Create an 8x8 array
	int arrayBlock[8][8]={0};
	TransformArray(arrayBlock, block);

	// Inverse DCT
	int val[8][8]={0};
	int val_fixed[8][8]={0};

	PerformIDCT(val, arrayBlock);
	// for 2 level matrix multiplication: PerformIDCT2(val, arrayBlock);

	// Level Shift each element (i.e. add 128), and copy to our
	// output
	unsigned char *outptr = outputBuf;
	for (int y = 0; y < 8; y++)
	{
		for (int x=0; x<8; x++)
		{
			val[x][y] += 128;

			outptr[x] = Clamp(val[x][y]);
		}

		outptr += stride;
	}

	#ifndef JPG_SPEEDUP
	DumpDecodedBlock(val);
	#endif
}

inline void DecodeSingleBlock_fixed(stComponent *comp, unsigned char *outputBuf, int stride, float &cos_pwr, float &block_pwr, float &block_mean, float &block_var)
{
	short* inptr    = comp->m_DCT;
	float* quantptr = comp->m_qTable;


	// Create a temp 8x8, i.e. 64 array for the data
	int data[64] = {0};

	// Copy our data into the temp array
	for (int i=0; i<64; i++)
	{
		data[i] = inptr[i];
	}

	// De-Quantize
	DequantizeBlock(data, quantptr);

	// De-Zig-Zag
	int block[64] = {0};
	DeZigZag(block, data);

	// Create an 8x8 array
	int arrayBlock[8][8]={0};
	TransformArray(arrayBlock, block);

	// Inverse DCT
	int val[8][8]={0};
	int val_fixed[8][8]={0};
	PerformIDCT_fixed(val, arrayBlock, cos_pwr, block_pwr, block_mean, block_var);

	// Level Shift each element (i.e. add 128), and copy to our
	// output
	unsigned char *outptr = outputBuf;
	for (int y = 0; y < 8; y++)
	{
		for (int x=0; x<8; x++)
		{
			val[x][y] += 128;

			outptr[x] = Clamp(val[x][y]);
		}

		outptr += stride;
	}

	#ifndef JPG_SPEEDUP
	DumpDecodedBlock(val);
	#endif
}

inline void DecodeSingleBlock2_fixed(stComponent *comp, unsigned char *outputBuf, int stride, float &cos_pwr, float &block_pwr, float &block_mean, float &block_var, float *elem, double *elem_pwr)
{
	short* inptr    = comp->m_DCT;
	float* quantptr = comp->m_qTable;


	// Create a temp 8x8, i.e. 64 array for the data
	int data[64] = {0};

	// Copy our data into the temp array
	for (int i=0; i<64; i++)
	{
		data[i] = inptr[i];
	}

	// De-Quantize
	DequantizeBlock(data, quantptr);

	// De-Zig-Zag
	int block[64] = {0};
	DeZigZag(block, data);

	// Create an 8x8 array
	int arrayBlock[8][8]={0};
	TransformArray(arrayBlock, block);

	// Inverse DCT
	int val[8][8]={0};
	int val_fixed[8][8]={0};
	PerformIDCT_fixed(val, arrayBlock, cos_pwr, block_pwr, block_mean, block_var);

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem[8*k+m] = (float)arrayBlock[k][m];

      elem_pwr[8*k+m] = (double)block[8*k+m];
    }
  }

	// Level Shift each element (i.e. add 128), and copy to our
	// output
	unsigned char *outptr = outputBuf;
	for (int y = 0; y < 8; y++)
	{
		for (int x=0; x<8; x++)
		{
			val[x][y] += 128;

			outptr[x] = Clamp(val[x][y]);
		}

		outptr += stride;
	}

	#ifndef JPG_SPEEDUP
	DumpDecodedBlock(val);
	#endif
}

/*
inline void DecodeSingleBlock2_ac(stComponent *comp, unsigned char *outputBuf, int stride, int *elem, double *elem_pwr)
{
	short* inptr    = comp->m_DCT;
	float* quantptr = comp->m_qTable;


	// Create a temp 8x8, i.e. 64 array for the data
	int data[64] = {0};

	// Copy our data into the temp array
	for (int i=0; i<64; i++)
	{
		data[i] = inptr[i];
	}

	// De-Quantize
	DequantizeBlock(data, quantptr);

	// De-Zig-Zag
	int block[64] = {0};
	DeZigZag(block, data);

	// Create an 8x8 array
	int arrayBlock[8][8]={0};
	TransformArray(arrayBlock, block);

	// Inverse DCT
	int val[8][8]={0};
	int val_fixed[8][8]={0};
	PerformIDCT2_AC(val, arrayBlock);


  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem[8*k+m] = arrayBlock[k][m];

      elem_pwr[8*k+m] = (double)block[8*k+m];
    }
  }

	// Level Shift each element (i.e. add 128), and copy to our
	// output
	unsigned char *outptr = outputBuf;
	for (int y = 0; y < 8; y++)
	{
		for (int x=0; x<8; x++)
		{
			val[x][y] += 128;

			outptr[x] = Clamp(val[x][y]);
		}

		outptr += stride;
	}

	#ifndef JPG_SPEEDUP
	DumpDecodedBlock(val);
	#endif
}

inline void DecodeSingleBlock3_ac(stComponent *comp, unsigned char *outputBuf, int stride, int *elem, double *elem_pwr, const int (*coef1)[8], const int (*coef2)[8], const int block_cnt)
{
	short* inptr    = comp->m_DCT;
	float* quantptr = comp->m_qTable;


	// Create a temp 8x8, i.e. 64 array for the data
	int data[64] = {0};

	// Copy our data into the temp array
	for (int i=0; i<64; i++)
	{
		data[i] = inptr[i];
	}

	// De-Quantize
	DequantizeBlock(data, quantptr);

	// De-Zig-Zag
	int block[64] = {0};
	DeZigZag(block, data);

	// Create an 8x8 array
	int arrayBlock[8][8]={0};
	TransformArray(arrayBlock, block);

	// Inverse DCT
	int val[8][8]={0};
	int val_fixed[8][8]={0};
//	PerformIDCT3_AC(val, arrayBlock, coef1, coef2);
	// for lumped model PerformIDCT1_AC(val, arrayBlock);
//	PerformIDCT4cbcr_AC(val, arrayBlock, coef1, coef2);
	PerformIDCT6_AC(val, arrayBlock, coef1, coef2, false);

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem[8*k+m] = arrayBlock[k][m];

      elem_pwr[8*k+m] = (double)block[8*k+m];
    }
  }

	// Level Shift each element (i.e. add 128), and copy to our
	// output
	unsigned char *outptr = outputBuf;
	for (int y = 0; y < 8; y++)
	{
		for (int x=0; x<8; x++)
		{
			val[x][y] += 128;

			outptr[x] = Clamp(val[x][y]);
		}

		outptr += stride;
	}

	#ifndef JPG_SPEEDUP
	DumpDecodedBlock(val);
	#endif
}
*/

/*
inline void DecodeSingleBlock3y_ac(stComponent *comp, unsigned char *outputBuf, int stride, int *elem, double *elem_pwr, const int (*coef1)[8], const int (*coef2)[8], const int block_cnt)
{
	short* inptr    = comp->m_DCT;
	float* quantptr = comp->m_qTable;


	// Create a temp 8x8, i.e. 64 array for the data
	int data[64] = {0};

	// Copy our data into the temp array
	for (int i=0; i<64; i++)
	{
		data[i] = inptr[i];
	}

	// De-Quantize
	DequantizeBlock(data, quantptr);

	// De-Zig-Zag
	int block[64] = {0};
	DeZigZag(block, data);

	// Create an 8x8 array
	int arrayBlock[8][8]={0};
	TransformArray(arrayBlock, block);

	// Inverse DCT
	int val[8][8]={0};
	int val_fixed[8][8]={0};
//	PerformIDCT3_AC(val, arrayBlock, coef1, coef2);
//	PerformIDCT4y_AC(val, arrayBlock, coef1, coef2);
	PerformIDCT6_AC(val, arrayBlock, coef1, coef2, true);
	// for lumped model PerformIDCT1y_AC(val, arrayBlock, block_cnt);

    for (int k=0;k<8;k++) {
      for (int m=0;m<8;m++) {
        elem[8*k+m] = arrayBlock[k][m];
        elem_pwr[8*k+m] = (double)block[8*k+m];
      }
    }

	// Level Shift each element (i.e. add 128), and copy to our
	// output
	unsigned char *outptr = outputBuf;
	for (int y = 0; y < 8; y++)
	{
		for (int x=0; x<8; x++)
		{
			val[x][y] += 128;

			outptr[x] = Clamp(val[x][y]);
		}

		outptr += stride;
	}

	#ifndef JPG_SPEEDUP
	DumpDecodedBlock(val);
	#endif
}
*/

/***************************************************************************/

/***************************************************************************/
//
// Save a buffer in 24bits Bitmap (.bmp) format
//
/***************************************************************************/
inline void WriteBMP24(const char* szBmpFileName, int Width, int Height, unsigned char* RGB)
{
	#pragma pack(1)
	struct stBMFH // BitmapFileHeader & BitmapInfoHeader
	{
		// BitmapFileHeader
		char         bmtype[2];     // 2 bytes - 'B' 'M'
		unsigned int iFileSize;     // 4 bytes
		short int    reserved1;     // 2 bytes
		short int    reserved2;     // 2 bytes
		unsigned int iOffsetBits;   // 4 bytes
		// End of stBMFH structure - size of 14 bytes
		// BitmapInfoHeader
		unsigned int iSizeHeader;    // 4 bytes - 40
		unsigned int iWidth;         // 4 bytes
		unsigned int iHeight;        // 4 bytes
		short int    iPlanes;        // 2 bytes
		short int    iBitCount;      // 2 bytes
		unsigned int Compression;    // 4 bytes
		unsigned int iSizeImage;     // 4 bytes
		unsigned int iXPelsPerMeter; // 4 bytes
		unsigned int iYPelsPerMeter; // 4 bytes
		unsigned int iClrUsed;       // 4 bytes
		unsigned int iClrImportant;  // 4 bytes
		// End of stBMIF structure - size 40 bytes
		// Total size - 54 bytes
	};
	#pragma pack()

	// Round up the width to the nearest DWORD boundary
	int iNumPaddedBytes = 4 - (Width * 3) % 4;
	iNumPaddedBytes = iNumPaddedBytes % 4;

	stBMFH bh;
	memset(&bh, 0, sizeof(bh));
	bh.bmtype[0]='B';
	bh.bmtype[1]='M';
	bh.iFileSize = (Width*Height*3) + (Height*iNumPaddedBytes) + sizeof(bh);
	bh.iOffsetBits = sizeof(stBMFH);
	bh.iSizeHeader = 40;
	bh.iPlanes = 1;
	bh.iWidth = Width;
	bh.iHeight = Height;
	bh.iBitCount = 24;


	char temp[2048]={0};
	sprintf(temp, "%s", szBmpFileName);
	FILE* fp = fopen(temp, "wb");
	// sglee DBG_ASSERT( fp ); // Error creating file - valid file name?  valid location?
	fwrite(&bh, sizeof(bh), 1, fp);
	for (int y=Height-1; y>=0; y--)
	{
		for (int x=0; x<Width; x++)
		{
			int i = (x + (Width)*y) * 3;
			unsigned int rgbpix = (RGB[i]<<16)|(RGB[i+1]<<8)|(RGB[i+2]<<0);
			fwrite(&rgbpix, 3, 1, fp);
		}
		if (iNumPaddedBytes>0)
		{
			unsigned char pad = 0;
			fwrite(&pad, iNumPaddedBytes, 1, fp);
		}
	}
	fclose(fp);
}

/***************************************************************************/

// Takes two array of bits, and build the huffman table for size, and code

/***************************************************************************/
inline void BuildHuffmanTable(const unsigned char *bits, const unsigned char *stream, stHuffmanTable *HT)
{
	for (int j=1; j<=16; j++)
	{
		HT->m_length[j] = bits[j];
	}

	// Work out the total number of codes
	int numBlocks = 0;
	for (int i=1; i<=16; i++)
	{
		numBlocks += HT->m_length[i];
	}
	HT->m_numBlocks = numBlocks;

	// Fill in the data our our blocks, so we know how many bits each
	// one is
	int c=0;
	for (int i=1; i<=16; i++)
	{
		for (int j=0; j<HT->m_length[i]; j++)
		{
			HT->m_blocks[c].length = i;
			c++;
		}

	}

	GenHuffCodes(HT->m_numBlocks, HT->m_blocks, HT->m_hufVal);
}

/***************************************************************************/

inline void PrintSOF(const unsigned char *stream)
{
	int width;
	int height;
	int nr_components;
	int precision;

	const char *nr_components_to_string[] =	{	"????",
												"Grayscale",
												"????",
												"YCbCr",
												"CYMK" };

	precision = stream[2];
	height = BYTE_TO_WORD(stream+3);
	width  = BYTE_TO_WORD(stream+5);
	nr_components = stream[7];

	dprintf("> SOF marker\n");
	dprintf("Size:%dx%d nr_components:%d (%s)  precision:%d\n",
							width, height,
							nr_components,
							nr_components_to_string[nr_components],
							precision);
}

/***************************************************************************/

inline int ParseSOF(stJpegData *jdata, const unsigned char *stream)
{
	/*
	SOF		16		0xffc0		Start Of Frame
	Lf		16		3Nf+8		Frame header length
	P		8		8			Sample precision
	Y		16		0-65535		Number of lines
	X		16		1-65535		Samples per line
	Nf		8		1-255		Number of image components (e.g. Y, U and V).

	---------Repeats for the number of components (e.g. Nf)-----------------
	Ci		8		0-255		Component identifier
	Hi		4		1-4			Horizontal Sampling Factor
	Vi		4		1-4			Vertical Sampling Factor
	Tqi		8		0-3			Quantization Table Selector.
	*/

	PrintSOF(stream);

	int height = BYTE_TO_WORD(stream+3);
	int width  = BYTE_TO_WORD(stream+5);
	int nr_components = stream[7];

	stream += 8;
	for (int i=0; i<nr_components; i++)
	{
		int cid				= *stream++;
		int sampling_factor = *stream++;
		int Q_table			= *stream++;

		stComponent *c = &jdata->m_component_info[cid];
		c->m_vFactor = sampling_factor&0xf;
		c->m_hFactor = sampling_factor>>4;
		c->m_qTable = jdata->m_Q_tables[Q_table];

		dprintf("Component:%d  factor:%dx%d  Quantization table:%d\n",
				cid,
				c->m_vFactor,
				c->m_hFactor,
				Q_table );
	}
	jdata->m_width = width;
	jdata->m_height = height;

	return 0;
}

/***************************************************************************/

inline void BuildQuantizationTable(float *qtable, const unsigned char *ref_table)
{
	int c = 0;

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			unsigned char val = ref_table[c];

			qtable[c] = val;
			c++;
		}
	}
}


/***************************************************************************/

inline int ParseDQT(stJpegData *jdata, const unsigned char *stream)
{
	int length, qi;
	float *table;

	dprintf("> DQT marker\n");
	length = BYTE_TO_WORD(stream) - 2;
	stream += 2;	// Skip length

	while (length>0)
	{
		qi = *stream++;

		int qprecision = qi>>4;	 // upper 4 bits specify the precision
		int qindex     = qi&0xf; // index is lower 4 bits

		if (qprecision)
		{
			// precision in this case is either 0 or 1 and indicates the precision
			// of the quantized values;
			// 8-bit (baseline) for 0 and  up to 16-bit for 1

			dprintf("Error - 16 bits quantization table is not supported\n");
			// sglee DBG_HALT;
		}

		if (qindex>=4)
		{
			dprintf("Error - No more 4 quantization table is supported (got %d)\n", qi);
			// sglee DBG_HALT;
		}

		// The quantization table is the next 64 bytes
		table = jdata->m_Q_tables[qindex];

		// the quantization tables are stored in zigzag format, so we
		// use this functino to read them all in and de-zig zag them
		BuildQuantizationTable(table, stream);
		stream += 64;
		length -= 65;
	}
	return 0;
}

/***************************************************************************/

inline int ParseSOS(stJpegData *jdata, const unsigned char *stream)
{
	/*
	SOS		16		0xffd8			Start Of Scan
	Ls		16		2Ns + 6			Scan header length
	Ns		8		1-4				Number of image components
	Csj		8		0-255			Scan Component Selector
	Tdj		4		0-1				DC Coding Table Selector
	Taj		4		0-1				AC Coding Table Selector
	Ss		8		0				Start of spectral selection
	Se		8		63				End of spectral selection
	Ah		4		0				Successive Approximation Bit High
	Ai		4		0				Successive Approximation Bit Low
	*/

	unsigned int nr_components = stream[2];

	dprintf("> SOS marker\n");

	if (nr_components != 3)
	{
		dprintf("Error - We only support YCbCr image\n");
		// sglee DBG_HALT;
	}


	stream += 3;
	for (unsigned int i=0;i<nr_components;i++)
	{
		unsigned int cid   = *stream++;
		unsigned int table = *stream++;

		if ((table&0xf)>=4)
		{
			dprintf("Error - We do not support more than 2 AC Huffman table\n");
			// sglee DBG_HALT;
		}
		if ((table>>4)>=4)
		{
			dprintf("Error - We do not support more than 2 DC Huffman table\n");
			// sglee DBG_HALT;
		}
		dprintf("ComponentId:%d  tableAC:%d tableDC:%d\n", cid, table&0xf, table>>4);

		jdata->m_component_info[cid].m_acTable = &jdata->m_HTAC[table&0xf];
		jdata->m_component_info[cid].m_dcTable = &jdata->m_HTDC[table>>4];
	}
	jdata->m_stream = stream+3;
	return 0;
}

/***************************************************************************/

inline int ParseDHT(stJpegData *jdata, const unsigned char *stream)
{
	/*
	u8 0xff
	u8 0xc4 (type of segment)
	u16 be length of segment
	4-bits class (0 is DC, 1 is AC, more on this later)
	4-bits table id
	array of 16 u8 number of elements for each of 16 depths
	array of u8 elements, in order of depth
	*/

	unsigned int count, i;
	unsigned char huff_bits[17];
	int length, index;

	length = BYTE_TO_WORD(stream) - 2;
	stream += 2;	// Skip length

	dprintf("> DHT marker (length=%d)\n", length);

	while (length>0)
	{
		index = *stream++;

		// We need to calculate the number of bytes 'vals' will takes
		huff_bits[0] = 0;
		count = 0;
		for (i=1; i<17; i++)
		{
			huff_bits[i] = *stream++;
			count += huff_bits[i];
		}

		if (count > 256)
		{
			dprintf("Error - No more than 1024 bytes is allowed to describe a huffman table");
			// sglee DBG_HALT;
		}
		if ( (index &0xf) >= HUFFMAN_TABLES)
		{
			dprintf("Error - No mode than %d Huffman tables is supported\n", HUFFMAN_TABLES);
			// sglee DBG_HALT;
		}
		dprintf("Huffman table %s n%d\n", (index&0xf0)?"AC":"DC", index&0xf);
		dprintf("Length of the table: %d\n", count);

		if (index & 0xf0 )
		{
			unsigned char* huffval = jdata->m_HTAC[index&0xf].m_hufVal;
			for (i = 0; i < count; i++)
				huffval[i] = *stream++;

			BuildHuffmanTable(huff_bits, stream, &jdata->m_HTAC[index&0xf]); // AC
		}
		else
		{
			unsigned char* huffval = jdata->m_HTDC[index&0xf].m_hufVal;
			for (i = 0; i < count; i++)
				huffval[i] = *stream++;

			BuildHuffmanTable(huff_bits, stream, &jdata->m_HTDC[index&0xf]); // DC
		}

		length -= 1;
		length -= 16;
		length -= count;
	}
	dprintf("< DHT marker\n");
	return 0;
}

/***************************************************************************/

inline int ParseJFIF(stJpegData *jdata, const unsigned char *stream)
{
	int chuck_len;
	int marker;
	int sos_marker_found = 0;
	int dht_marker_found = 0;

	// Parse marker
	while (!sos_marker_found)
	{
		if (*stream++ != 0xff)
		{
			goto bogus_jpeg_format;
		}

		// Skip any padding ff byte (this is normal)
		while (*stream == 0xff)
		{
			stream++;
		}

		marker = *stream++;
		chuck_len = BYTE_TO_WORD(stream);

		switch (marker)
		{
			case SOF:
			{
				if (ParseSOF(jdata, stream) < 0)
					return -1;
			}
			break;

			case DQT:
			{
				if (ParseDQT(jdata, stream) < 0)
					return -1;
			}
			break;

			case SOS:
			{
				if (ParseSOS(jdata, stream) < 0)
					return -1;
				sos_marker_found = 1;
			}
			break;

			case DHT:
			{
				if (ParseDHT(jdata, stream) < 0)
					return -1;
				dht_marker_found = 1;
			}
			break;

			// The reason I added these additional skips here, is because for
			// certain jpg compressions, like swf, it splits the encoding
			// and image data with SOI & EOI extra tags, so we need to skip
			// over them here and decode the whole image
			case SOI:
			case EOI:
			{
				chuck_len = 0;
				break;
			}
			break;

			case 0xDD: //DRI: Restart_markers=1;
			{
				jdata->m_restart_interval = BYTE_TO_WORD(stream);
				dprintf("DRI - Restart_marker\n");
			}
			break;

			case APP0:
			{
				dprintf("APP0 Chunk ('txt' information) skipping\n");
			}
			break;

			default:
			{
				dprintf("ERROR> Unknown marker %2.2x\n", marker);
			}
			break;
		}

		stream += chuck_len;
	}

	if (!dht_marker_found)
	{
		dprintf("ERROR> No Huffman table loaded\n");
		// sglee DBG_HALT;
	}

	return 0;

	bogus_jpeg_format:
	dprintf("ERROR> Bogus jpeg format\n");
	// sglee DBG_HALT;
	return -1;
}

/***************************************************************************/

inline int JpegParseHeader(stJpegData *jdata, const unsigned char *buf, unsigned int size)
{
	// Identify the file
	if ((buf[0] != 0xFF) || (buf[1] != SOI))
	{
		dprintf("Not a JPG file ?\n");
		// sglee DBG_HALT;
		return -1;
	}

	const unsigned char* startStream = buf+2;
	const int fileSize = size-2;

	dprintf("-|- File thinks its size is: %d bytes\n", fileSize);

	int ret = ParseJFIF(jdata, startStream);

	return ret;
}

/***************************************************************************/

inline void JpegGetImageSize(stJpegData *jdata, unsigned int *width, unsigned int *height)
{
	*width  = jdata->m_width;
	*height = jdata->m_height;
}

/***************************************************************************/

unsigned int g_reservoir = 0;
unsigned int g_nbits_in_reservoir = 0;

#ifdef JPG_SPEEDUP

#define FillNBits(stream, nbits_wanted)						\
{															\
	while ((int)g_nbits_in_reservoir<nbits_wanted)			\
	{														\
		unsigned char c = *(*stream)++;						\
		g_reservoir <<= 8;									\
		if (c == 0xff && (**stream) == 0x00)				\
		{													\
			(*stream)++;									\
		}													\
		g_reservoir |= c;									\
		g_nbits_in_reservoir+=8;							\
	}														\
}															\

inline short GetNBits(const unsigned char** stream, int nbits_wanted)
{
	FillNBits(stream, nbits_wanted);

	short result = ((g_reservoir)>>(g_nbits_in_reservoir-(nbits_wanted)));

	g_nbits_in_reservoir -= (nbits_wanted);
	g_reservoir &= ((1U<<g_nbits_in_reservoir)-1);

	return result;
}

inline int LookNBits(const unsigned char** stream, int nbits_wanted)
{
	FillNBits(stream, nbits_wanted);

	int result = ((g_reservoir)>>(g_nbits_in_reservoir-(nbits_wanted)));
	return result;
}

#define SkipNBits(stream, nbits_wanted)						\
{															\
	FillNBits(stream, nbits_wanted);						\
															\
	g_nbits_in_reservoir -= (nbits_wanted);					\
	g_reservoir &= ((1U<<g_nbits_in_reservoir)-1);			\
}															\


#else

inline void FillNBits(const unsigned char** stream, int& nbits_wanted)
{
	while ((int)g_nbits_in_reservoir<nbits_wanted)
	{
		/*const */unsigned char c = *(*stream)++;
		g_reservoir <<= 8;
		//if (c == 0xff && (**stream) != 0x00)
		//{
		//	//// sglee DBG_HALT;
		//	*(*stream)++;
		//	*(*stream)++;
		//	c = *(*stream)++;

		//	g_reservoir = 0;
		//	g_nbits_in_reservoir = 0;
		//}

		if (c == 0xff && (**stream) == 0x00)
		{
			(*stream)++;
		}
		g_reservoir |= c;
		g_nbits_in_reservoir+=8;
	}
}

inline short GetNBits(const unsigned char** stream, int nbits_wanted)
{
	FillNBits(stream, nbits_wanted);

	short result = ((g_reservoir)>>(g_nbits_in_reservoir-(nbits_wanted)));

	g_nbits_in_reservoir -= (nbits_wanted);
	g_reservoir &= ((1U<<g_nbits_in_reservoir)-1);

	/*
	// Could do the sign conversion here!
	if (result < (short)(1UL<<((nbits_wanted)-1)))
	{
		result = result + (short)(0xFFFFFFFFUL<<(nbits_wanted))+1;
	}
	*/
	return result;
}

inline int LookNBits(const unsigned char** stream, int nbits_wanted)
{
	FillNBits(stream, nbits_wanted);

	int result = ((g_reservoir)>>(g_nbits_in_reservoir-(nbits_wanted)));
	return result;
}

inline void SkipNBits(const unsigned char** stream, int& nbits_wanted)
{
	FillNBits(stream, nbits_wanted);

	g_nbits_in_reservoir -= (nbits_wanted);
	g_reservoir &= ((1U<<g_nbits_in_reservoir)-1);
}

#endif //JPG_SPEEDUP

/***************************************************************************/


bool IsInHuffmanCodes(int code, int numCodeBits, int numBlocks, stBlock* blocks, int* outValue)
{
	for (int j=0; j<numBlocks; j++)
	{
		int hufhCode		= blocks[j].code;
		int hufCodeLenBits	= blocks[j].length;
		int hufValue		= blocks[j].value;

		// We've got a match!
		if ((code==hufhCode) && (numCodeBits==hufCodeLenBits))
		{
			*outValue = hufValue;
			return true;
		}
	}
	return false;
}

/***************************************************************************/

int DetermineSign(int val, int nBits)
{
	bool negative = val < (1<<(nBits-1));

	if (negative)
	{
		// (-1 << (s)), makes the last bit a 1, so we have 1000,0000 for example for 8 bits

		val = val + (-1 << (nBits)) + 1;
	}

	// Else its unsigned, just return
	return val;
}

/***************************************************************************/

char g_bigBuf[1024] = {0};
char* IntToBinary(int val, int bits)
{
	for (int i=0; i<32; i++) g_bigBuf[i]='\0';

	int c = 0;
	for (int i=bits-1; i>=0; i--)
	{
		bool on = (val & (1<<i)) ? 1 : 0;
		g_bigBuf[c] = on ? '1' : '0';
		c++;
	}

	return &g_bigBuf[0];
}

/***************************************************************************/

void DumpHufCodes(stHuffmanTable* table)
{
	dprintf("HufCodes\n");
	dprintf("Num: %d\n", table->m_numBlocks);
	for (int i = 0; i<table->m_numBlocks; i++)
	{
		dprintf("%03d\t [%s]\n", i, IntToBinary(table->m_blocks[i].code, table->m_blocks[i].length));
	}
	dprintf("\n");

}

/***************************************************************************/

void DumpDCTValues(short dct[64])
{
	dprintf("\n#Extracted DCT values from SOS#\n");
	int c = 0;
	for (int i=0; i<64; i++)
	{
		dprintf("% 4d  ", dct[c++]);

		if ( (c>0) && (c%8==0) ) dprintf("\n");
	}
	dprintf("\n");
}


void ProcessRestart()
{

}

/***************************************************************************/

void ProcessHuffmanDataUnit(stJpegData *jdata, int indx)
{
	// sglee DBG_ASSERT(indx>=0 && indx<4);
	stComponent *c = &jdata->m_component_info[indx];

	// Start Huffman decoding

	// We memset it here, as later on we can just skip along, when we have lots
	// of leading zeros, for our AC run length encoding :)
	short DCT_tcoeff[64];
	memset(DCT_tcoeff, 0, sizeof(DCT_tcoeff)); //Initialize DCT_tcoeff


	bool found = false;
	int decodedValue = 0;


//	DumpHufCodes(c->m_dcTable);
//	DumpHufCodes(c->m_acTable);

	dprintf("\nHuff Block:\n\n");


	// Scan Decode Resync
	if (jdata->m_restart_interval>0)
	if ( jdata->m_stream[0]==0xff && jdata->m_stream[1]!=0x00)
	{
		// Something might be wrong, we should have had an interval marker set
		//// sglee DBG_ASSERT(jdata->m_restart_interval>0);
		jdata->m_stream += 2;
		g_reservoir = 0;
		g_nbits_in_reservoir = 0;

		// The value in the interval marker determines what number it will count
		// upto before looping back...i.e, for an interval marker of 4, we will
		// have 0xFFD0, 0xFFD1, 0xFFD2, 0xFFD3, 0xFFD4, 0xFFD0, 0xFFD1..etc..looping
		// so each time we get a resync, it will count from 0 to 4 then restart :)
	}

	// First thing is get the 1 DC coefficient at the start of our 64 element
	// block
	for (int k=1; k<16; k++)
	{
		// Keep grabbing one bit at a time till we find one thats a huffman code
		int code = LookNBits(&jdata->m_stream, k);

		// Check if its one of our huffman codes
		if (IsInHuffmanCodes(code, k,  c->m_dcTable->m_numBlocks, c->m_dcTable->m_blocks, &decodedValue))
		{
			// Skip over the rest of the bits now.
			SkipNBits(&jdata->m_stream, k);

			found = true;

			// The decoded value is the number of bits we have to read in next
			int numDataBits = decodedValue;

			// We know the next k bits are for the actual data
			if (numDataBits==0)
			{
				DCT_tcoeff[0] = c->m_previousDC;
			}
			else
			{

				if (jdata->m_restart_interval>0)
				if ( jdata->m_stream[0]==0xff && jdata->m_stream[1]!=0x00)
				{
					jdata->m_stream += 2;
					g_reservoir = 0;
					g_nbits_in_reservoir = 0;
				}

				short data = GetNBits(&jdata->m_stream, numDataBits);

				data = DetermineSign(data, numDataBits);

				DCT_tcoeff[0] = data + c->m_previousDC;
				c->m_previousDC = DCT_tcoeff[0];
			}

			// Found so we can exit out
			break;
		}
	}

	if (!found)
	{
		dprintf("-|- ##ERROR## We have a *serious* error, unable to find huffman code\n");
		// sglee DBG_HALT;
	}


	//if ( jdata->m_stream[0]==0xff && jdata->m_stream[1]!=0x00)
	//{
	//	jdata->m_stream += 2;
	//	g_reservoir = 0;
	//	g_nbits_in_reservoir = 0;
	//}


	// Second, the 63 AC coefficient
	int nr=1;
	bool EOB_found=false;
	while ( (nr<=63)&&(!EOB_found) )
	{
		int k = 0;
		for (k=1; k<=16; k++)
		{
			// Keep grabbing one bit at a time till we find one thats a huffman code
			int code = LookNBits(&jdata->m_stream, k);


			// Check if its one of our huffman codes
			if (IsInHuffmanCodes(code, k,  c->m_acTable->m_numBlocks, c->m_acTable->m_blocks, &decodedValue))
			{

				// Skip over k bits, since we found the huffman value
				SkipNBits(&jdata->m_stream, k);


				// Our decoded value is broken down into 2 parts, repeating RLE, and then
				// the number of bits that make up the actual value next
				int valCode = decodedValue;

				unsigned char size_val = valCode&0xF;	// Number of bits for our data
				unsigned char count_0  = valCode>>4;	// Number RunLengthZeros

				if (size_val==0)
				{// RLE
					if (count_0==0)EOB_found=true;	// EOB found, go out
					else if (count_0==0xF) nr+=16;  // skip 16 zeros
				}
				else
				{
					nr+=count_0; //skip count_0 zeroes

					if (nr > 63)
					{
						dprintf("-|- ##ERROR## Huffman Decoding\n");
						// sglee DBG_HALT;
					}

					//if ( jdata->m_stream[0]==0xff && jdata->m_stream[1]!=0x00)
					//{
					//	jdata->m_stream += 2;
					//	g_reservoir = 0;
					//	g_nbits_in_reservoir = 0;
					//}

					short data = GetNBits(&jdata->m_stream, size_val );

					data = DetermineSign(data, size_val);

					DCT_tcoeff[nr++]=data;

				}
				break;
			}
		}

		if (k>16)
		{
			nr++;
		}
	}


	#ifndef JPG_SPEEDUP
	DumpDCTValues(DCT_tcoeff);
	#endif


	// We've decoded a block of data, so copy it across to our buffer
	for (int j = 0; j < 64; j++)
	{
		c->m_DCT[j] = DCT_tcoeff[j];
	}

}

/***************************************************************************/

inline void ConvertYCrCbtoRGB(int y, int cb, int cr,
							  int* r, int* g, int* b)

{
	float red, green, blue;

	red   = y + 1.402f*(cb-128);
	green = y-0.34414f*(cr-128)-0.71414f*(cb-128);
	blue  = y+1.772f*(cr-128);

	*r = (int) Clamp((int)red);
	*g = (int) Clamp((int)green);
	*b = (int) Clamp((int)blue);
}

/***************************************************************************/

inline void YCrCB_to_RGB24_Block8x8(stJpegData *jdata, int w, int h, int imgx, int imgy, int imgw, int imgh)
{
	const unsigned char *Y, *Cb, *Cr;
	unsigned char *pix;

	int r, g, b;

	Y  = jdata->m_Y;
	Cb = jdata->m_Cb;
	Cr = jdata->m_Cr;

	int olw = 0; // overlap
//	if ( imgx > abs(imgw-8*w) )
//	{
//		olw = (imgw-imgx)*w + 1;
//	}

	int olh = 0; // overlap
//	if ( imgy > abs(imgh-8*h) )
//	{
//		olh = (imgh-imgy)*2 + 1;
//	}

//	dprintf("***pix***\n\n");
	for (int y=0; y<(8*h - olh); y++)
	{
		for (int x=0; x<(8*w - olw); x++)
		{

			if (x+imgx >= imgw) continue;
			if (y+imgy >= imgh) continue;

			int poff = x*3 + jdata->m_width*3*y;
			pix = &(jdata->m_colourspace[poff]);

			int yoff = x + y*(w*8);
			int coff = (int)(x*(1.0f/w)) + (int)(y*(1.0f/h))*8;

			int yc =  Y[yoff];
			int cb = Cb[coff];
			int cr = Cr[coff];

			ConvertYCrCbtoRGB(yc,cr,cb,&r,&g,&b);

			pix[0] = Clamp(r);
			pix[1] = Clamp(g);
			pix[2] = Clamp(b);

//			dprintf("-[%d][%d][%d]-\t", poff, yoff, coff);
		}
//		dprintf("\n");
	}
//	dprintf("\n\n");
}

/***************************************************************************/
//
//  Decoding
//  .-------.
//  | 1 | 2 |
//  |---+---|
//  | 3 | 4 |
//  `-------'
//
/***************************************************************************/
inline void DecodeMCU(stJpegData *jdata, int w, int h)
{
	// Y
	for (int y=0; y<h; y++)
	{
		for (int x=0; x<w; x++)
		{

			int stride = w*8;
			int offset = x*8 + y*64*w;

			ProcessHuffmanDataUnit(jdata, cY);

			// sglee DBG_ASSERT(cY>0 && cY<COMPONENTS);
			// sglee DBG_ASSERT(offset>=0 && offset<64*4);
			DecodeSingleBlock(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride);
		}
	}
	// Cb
	ProcessHuffmanDataUnit(jdata, cCb);
	DecodeSingleBlock(&jdata->m_component_info[cCb], jdata->m_Cb, 8);

	// Cr
	ProcessHuffmanDataUnit(jdata, cCr);
	DecodeSingleBlock(&jdata->m_component_info[cCr], jdata->m_Cr, 8);
}

inline void DecodeMCU_fixed(stJpegData *jdata, int w, int h, float &cos_pwr, float &block_pwr, float &block_mean, float &block_var)
{
  // sglee
  float cos_pwr1;
  float block_pwr1;
  float block_mean1;
  float block_var1;

	// Y
	for (int y=0; y<h; y++)
	{
		for (int x=0; x<w; x++)
		{

			int stride = w*8;
			int offset = x*8 + y*64*w;

			ProcessHuffmanDataUnit(jdata, cY);

			// sglee DBG_ASSERT(cY>0 && cY<COMPONENTS);
			// sglee DBG_ASSERT(offset>=0 && offset<64*4);
			DecodeSingleBlock_fixed(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, cos_pwr, block_pwr, block_mean, block_var);

		}
	}

	// Cb
	ProcessHuffmanDataUnit(jdata, cCb);
	DecodeSingleBlock_fixed(&jdata->m_component_info[cCb], jdata->m_Cb, 8, cos_pwr1, block_pwr1, block_mean1, block_var1);

	// Cr
	ProcessHuffmanDataUnit(jdata, cCr);
	DecodeSingleBlock_fixed(&jdata->m_component_info[cCr], jdata->m_Cr, 8, cos_pwr1, block_pwr1, block_mean1, block_var1);

}

inline void DecodeMCU2_fixed(stJpegData *jdata, int w, int h, float &cos_pwr, float &block_pwr, float &block_mean, float &block_var, double *elem_pwr, float *elem_y, float *elem_cb, float *elem_cr)
{
  // sglee
  float cos_pwr0;
  float block_pwr0;
  float block_mean0;
  float block_var0;
  float cos_pwr1;
  float block_pwr1;
  float block_mean1;
  float block_var1;
  float cos_pwr2;
  float block_pwr2;
  float block_mean2;
  float block_var2;

  //float elem_y[8*8];
  //float elem_cb[8*8];
  //float elem_cr[8*8];

  double elem_pwr_y[8*8];
  double elem_pwr_cb[8*8];
  double elem_pwr_cr[8*8];

 	// Y
	for (int y=0; y<h; y++)
	{
		for (int x=0; x<w; x++)
		{

			int stride = w*8;
			int offset = x*8 + y*64*w;

			ProcessHuffmanDataUnit(jdata, cY);

			// sglee DBG_ASSERT(cY>0 && cY<COMPONENTS);
			// sglee DBG_ASSERT(offset>=0 && offset<64*4);
			// DecodeSingleBlock_fixed(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, cos_pwr0, block_pwr0, block_mean0, block_var0);
			DecodeSingleBlock2_fixed(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, cos_pwr0, block_pwr0, block_mean0, block_var0, &elem_y[0], &elem_pwr_y[0]);

		}
	}

	// Cb
	ProcessHuffmanDataUnit(jdata, cCb);
	// DecodeSingleBlock_fixed(&jdata->m_component_info[cCb], jdata->m_Cb, 8, cos_pwr1, block_pwr1, block_mean1, block_var1);
	DecodeSingleBlock2_fixed(&jdata->m_component_info[cCb], jdata->m_Cb, 8, cos_pwr1, block_pwr1, block_mean1, block_var1, &elem_cb[0], &elem_pwr_cb[0]);

	// Cr
	ProcessHuffmanDataUnit(jdata, cCr);
	// DecodeSingleBlock_fixed(&jdata->m_component_info[cCr], jdata->m_Cr, 8, cos_pwr2, block_pwr2, block_mean2, block_var2);
	DecodeSingleBlock2_fixed(&jdata->m_component_info[cCr], jdata->m_Cr, 8, cos_pwr2, block_pwr2, block_mean2, block_var2, &elem_cr[0], &elem_pwr_cr[0]);

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      // elem_pwr[8*k+m] = ((elem_y[8*k+m]*elem_y[8*k+m])+(elem_cr[8*k+m]*elem_cr[8*k+m])+(elem_cb[8*k+m]*elem_cb[8*k+m]))/3.0;
      elem_pwr[8*k+m] = ((elem_pwr_y[8*k+m]*elem_pwr_y[8*k+m])+(elem_pwr_cr[8*k+m]*elem_pwr_cr[8*k+m])+(elem_pwr_cb[8*k+m]*elem_pwr_cb[8*k+m]))/3.0;
    }
  }

  cos_pwr = (cos_pwr0+cos_pwr1+cos_pwr2)/3.0;
  block_pwr = (block_pwr0+block_pwr1+block_pwr2)/3.0;
  block_mean = (block_mean0+block_mean1+block_mean2)/3.0;
  // printf ("SGLEE BLK PWR: %f \n", block_pwr);

}

/*
inline void DecodeMCU2_ac(stJpegData *jdata, int w, int h, double *elem_pwr, int *elem_y, int *elem_cb, int *elem_cr)
{

  double elem_pwr_y[8*8];
  double elem_pwr_cb[8*8];
  double elem_pwr_cr[8*8];

 	// Y
	for (int y=0; y<h; y++)
	{
		for (int x=0; x<w; x++)
		{

			int stride = w*8;
			int offset = x*8 + y*64*w;

			ProcessHuffmanDataUnit(jdata, cY);

			// sglee DBG_ASSERT(cY>0 && cY<COMPONENTS);
			// sglee DBG_ASSERT(offset>=0 && offset<64*4);
			// DecodeSingleBlock_fixed(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, cos_pwr0, block_pwr0, block_mean0, block_var0);
			DecodeSingleBlock2_ac(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, &elem_y[0], &elem_pwr_y[0]);

		}
	}

	// Cb
	ProcessHuffmanDataUnit(jdata, cCb);
	// DecodeSingleBlock_fixed(&jdata->m_component_info[cCb], jdata->m_Cb, 8, cos_pwr1, block_pwr1, block_mean1, block_var1);
	DecodeSingleBlock2_ac(&jdata->m_component_info[cCb], jdata->m_Cb, 8, &elem_cb[0], &elem_pwr_cb[0]);

	// Cr
	ProcessHuffmanDataUnit(jdata, cCr);
	// DecodeSingleBlock_fixed(&jdata->m_component_info[cCr], jdata->m_Cr, 8, cos_pwr2, block_pwr2, block_mean2, block_var2);
	DecodeSingleBlock2_ac(&jdata->m_component_info[cCr], jdata->m_Cr, 8, &elem_cr[0], &elem_pwr_cr[0]);

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      // elem_pwr[8*k+m] = ((elem_y[8*k+m]*elem_y[8*k+m])+(elem_cr[8*k+m]*elem_cr[8*k+m])+(elem_cb[8*k+m]*elem_cb[8*k+m]))/3.0;
      elem_pwr[8*k+m] = ((elem_pwr_y[8*k+m]*elem_pwr_y[8*k+m])+(elem_pwr_cr[8*k+m]*elem_pwr_cr[8*k+m])+(elem_pwr_cb[8*k+m]*elem_pwr_cb[8*k+m]))/3.0;
    }
  }

}

inline void DecodeMCU3_ac(stJpegData *jdata, int w, int h, double *elem_pwr, int *elem_y, int *elem_cb, int *elem_cr, const int (*coef1)[8], const int (*coef2)[8], const int block_cnt)
{

  double elem_pwr_y[8*8];
  double elem_pwr_cb[8*8];
  double elem_pwr_cr[8*8];

 	// Y
	for (int y=0; y<h; y++)
	{
		for (int x=0; x<w; x++)
		{

			int stride = w*8;
			int offset = x*8 + y*64*w;

			ProcessHuffmanDataUnit(jdata, cY);

			// sglee DBG_ASSERT(cY>0 && cY<COMPONENTS);
			// sglee DBG_ASSERT(offset>=0 && offset<64*4);
			// DecodeSingleBlock_fixed(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, cos_pwr0, block_pwr0, block_mean0, block_var0);
			DecodeSingleBlock3y_ac(&jdata->m_component_info[cY], &jdata->m_Y[offset], stride, &elem_y[0], &elem_pwr_y[0], coef1, coef2, block_cnt);

		}
	}

	// Cb
	ProcessHuffmanDataUnit(jdata, cCb);
	// DecodeSingleBlock_fixed(&jdata->m_component_info[cCb], jdata->m_Cb, 8, cos_pwr1, block_pwr1, block_mean1, block_var1);
	DecodeSingleBlock3_ac(&jdata->m_component_info[cCb], jdata->m_Cb, 8, &elem_cb[0], &elem_pwr_cb[0], coef1, coef2, block_cnt);

	// Cr
	ProcessHuffmanDataUnit(jdata, cCr);
	// DecodeSingleBlock_fixed(&jdata->m_component_info[cCr], jdata->m_Cr, 8, cos_pwr2, block_pwr2, block_mean2, block_var2);
	DecodeSingleBlock3_ac(&jdata->m_component_info[cCr], jdata->m_Cr, 8, &elem_cr[0], &elem_pwr_cr[0], coef1, coef2, block_cnt);

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      // elem_pwr[8*k+m] = ((elem_y[8*k+m]*elem_y[8*k+m])+(elem_cr[8*k+m]*elem_cr[8*k+m])+(elem_cb[8*k+m]*elem_cb[8*k+m]))/3.0;
      elem_pwr[8*k+m] = ((elem_pwr_y[8*k+m]*elem_pwr_y[8*k+m])+(elem_pwr_cr[8*k+m]*elem_pwr_cr[8*k+m])+(elem_pwr_cb[8*k+m]*elem_pwr_cb[8*k+m]))/3.0;
    }
  }

}
*/

/***************************************************************************/

inline int JpegDecode(stJpegData *jdata)
{
	g_reservoir = 0;
	g_nbits_in_reservoir = 0;

	int hFactor = jdata->m_component_info[cY].m_hFactor;
	int vFactor = jdata->m_component_info[cY].m_vFactor;

	// RGB24:
	if (jdata->m_rgb == NULL)
	{
		int h = jdata->m_height*3;
		int w = jdata->m_width*3;
		int height = h + (8*hFactor) - (h%(8*hFactor));
		int width  = w + (8*vFactor) - (w%(8*vFactor));
		jdata->m_rgb = new unsigned char[width * height + 100]; // 100 is a safetly // this is actual memory

		#ifndef JPG_SPEEDUP
		memset(jdata->m_rgb, 0, width*height);
		#endif
	}

	jdata->m_component_info[0].m_previousDC = 0;
	jdata->m_component_info[1].m_previousDC = 0;
	jdata->m_component_info[2].m_previousDC = 0;
	jdata->m_component_info[3].m_previousDC = 0;

	int xstride_by_mcu = 8*hFactor;
	int ystride_by_mcu = 8*vFactor;

	// Don't forget to that block can be either 8 or 16 lines
	unsigned int bytes_per_blocklines = jdata->m_width*3 * ystride_by_mcu;

	unsigned int bytes_per_mcu = 3*xstride_by_mcu;

	// Just the decode the image by 'macroblock' (size is 8x8, 8x16, or 16x16)
    int my_counter = 0;	
    for (int y=0 ; y<(int)jdata->m_height; y+=ystride_by_mcu)
	{
		for (int x=0; x<(int)jdata->m_width; x+=xstride_by_mcu)
		{
            my_counter++; 
            jdata->m_colourspace = jdata->m_rgb + x*3 + (y *jdata->m_width*3);

			// Decode MCU Plane
		    another_counter= 0;
            DecodeMCU(jdata, hFactor, vFactor );

            //	if (x>20) continue;
		//	if (y>8) continue;

			YCrCB_to_RGB24_Block8x8(jdata, hFactor, vFactor, x, y, jdata->m_width, jdata->m_height);
		}
	}
    cout<<my_counter<<"ok"<<endl;
    cout<<(int)jdata->m_width*(int)jdata->m_height<<"ok"<<endl;
    //cout<<another_counter<<"another_counter"<<endl;
	return 0;
}

inline int JpegDecode_fixed(stJpegData *jdata) // , double *elem_pwr_avg)
{
	g_reservoir = 0;
	g_nbits_in_reservoir = 0;

	int hFactor = jdata->m_component_info[cY].m_hFactor;
	int vFactor = jdata->m_component_info[cY].m_vFactor;

	// RGB24:
	if (jdata->m_rgb == NULL)
	{
		int h = jdata->m_height*3;
		int w = jdata->m_width*3;
		int height = h + (8*hFactor) - (h%(8*hFactor));
		int width  = w + (8*vFactor) - (w%(8*vFactor));
		jdata->m_rgb = new unsigned char[width * height + 100]; // 100 is a safetly

		#ifndef JPG_SPEEDUP
		memset(jdata->m_rgb, 0, width*height);
		#endif
	}

	jdata->m_component_info[0].m_previousDC = 0;
	jdata->m_component_info[1].m_previousDC = 0;
	jdata->m_component_info[2].m_previousDC = 0;
	jdata->m_component_info[3].m_previousDC = 0;

	int xstride_by_mcu = 8*hFactor;
	int ystride_by_mcu = 8*vFactor;

	// Don't forget to that block can be either 8 or 16 lines
	unsigned int bytes_per_blocklines = jdata->m_width*3 * ystride_by_mcu;

	unsigned int bytes_per_mcu = 3*xstride_by_mcu;

  double avg_cos_pwr = 0;
  double avg_block_pwr = 0;
  int block_cnt = 0;
  float cos_pwr;
  float block_pwr;

  double avg_block_mean = 0;
  double avg_block_var = 0;
  float block_mean;
  float block_var;

  double elem_pwr[8*8];
  // double elem_pwr_avg[8][8];
  double elem_pwr_sum[8][8];

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem_pwr_sum[k][m] = 0;
    }
  }

  float elem_y[8*8];
  float elem_cb[8*8];
  float elem_cr[8*8];

  int blk_cnt = 0;
	// Just the decode the image by 'macroblock' (size is 8x8, 8x16, or 16x16)
	for (int y=0 ; y<(int)jdata->m_height; y+=ystride_by_mcu)
	{
		for (int x=0; x<(int)jdata->m_width; x+=xstride_by_mcu)
		{
			jdata->m_colourspace = jdata->m_rgb + x*3 + (y *jdata->m_width*3);

			// Decode MCU Plane
			DecodeMCU2_fixed(jdata, hFactor, vFactor, cos_pwr, block_pwr, block_mean, block_var, &elem_pwr[0], &elem_y[0], &elem_cb[0], &elem_cr[0]);

	  /*
      for (int k=0;k<8;k++) {
        for (int m=0;m<8;m++) {
          elem_pwr_sum[k][m] += elem_pwr[8*k+m];
          elem_pwr_y2[blk_cnt*64+8*k+m] = elem_y[k+8*m];
          elem_pwr_cb2[blk_cnt*64+8*k+m] = elem_cb[k+8*m];
          elem_pwr_cr2[blk_cnt*64+8*k+m] = elem_cr[k+8*m];

        }
      }
      */

      block_cnt += 1;
      avg_cos_pwr += cos_pwr;
      avg_block_pwr += block_pwr;
      avg_block_mean += block_mean;
      avg_block_var += block_var;

      YCrCB_to_RGB24_Block8x8(jdata, hFactor, vFactor, x, y, jdata->m_width, jdata->m_height);

      blk_cnt++;
		}
	}

  //printf ("SGLEE BLK CNT: %d \n", blk_cnt);

  /*
  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem_pwr_avg[8*k+m] = elem_pwr_sum[k][m]/(float)block_cnt;
      // printf ("SGLEE ELEM AVG PWR: %f at %d,%d \n", elem_pwr_avg[k][m],k,m);
    }
  }

  printf ("SGLEE: %f, %f, %d \n", avg_cos_pwr/(float)block_cnt, avg_block_pwr/(float)block_cnt, block_cnt);
  printf ("SGLEE: %f, %f, %d \n", avg_block_mean/(float)block_cnt, avg_block_var/(float)block_cnt, block_cnt);
  */

	return 0;
}

/*
inline int JpegDecode_ac(stJpegData *jdata, double *elem_pwr_avg)
{
	g_reservoir = 0;
	g_nbits_in_reservoir = 0;

	int hFactor = jdata->m_component_info[cY].m_hFactor;
	int vFactor = jdata->m_component_info[cY].m_vFactor;

	// RGB24:
	if (jdata->m_rgb == NULL)
	{
		int h = jdata->m_height*3;
		int w = jdata->m_width*3;
		int height = h + (8*hFactor) - (h%(8*hFactor));
		int width  = w + (8*vFactor) - (w%(8*vFactor));
		jdata->m_rgb = new unsigned char[width * height + 100]; // 100 is a safetly

		#ifndef JPG_SPEEDUP
		memset(jdata->m_rgb, 0, width*height);
		#endif
	}

	jdata->m_component_info[0].m_previousDC = 0;
	jdata->m_component_info[1].m_previousDC = 0;
	jdata->m_component_info[2].m_previousDC = 0;
	jdata->m_component_info[3].m_previousDC = 0;

	int xstride_by_mcu = 8*hFactor;
	int ystride_by_mcu = 8*vFactor;

	// Don't forget to that block can be either 8 or 16 lines
	unsigned int bytes_per_blocklines = jdata->m_width*3 * ystride_by_mcu;

	unsigned int bytes_per_mcu = 3*xstride_by_mcu;

  int block_cnt = 0;

  double elem_pwr[8*8];
  // double elem_pwr_avg[8][8];
  double elem_pwr_sum[8][8];

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem_pwr_sum[k][m] = 0;
    }
  }

  int elem_y[8*8];
  int elem_cb[8*8];
  int elem_cr[8*8];

  ///////////////////////////////////////////////////////////////////////
  int cos_bi1 = 1;
  int cos_bf1 = 7;
  int cos_bi2 = 1;
  int cos_bf2 = 7;
  int coef1[8][8];
  int coef2[8][8];
//  float PI = 3.14f;

  float tmp_coef;
  // generate first coefficient matrix: C(v)*cos(((2*y+1) * v * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  tmp_coef = C(i2)*cosf(((2*i1+1) * i2 * PI) / 16);
		  coef1[i1][i2] = float2int_idct (tmp_coef, cos_bi1, cos_bf1);
//		  coef1_err[i1][i2] = tmp_coef - ((float)coef1[i1][i2])/pow(2.0, cos_bf1);
//		  printf ("SGLEE COEF1: %f, %f \n", tmp_coef*pow(2.0,cos_bf1), coef1_err[i1][i2]);
		  if (io_capture2) vec_coef1[i1*8+i2].push_back(coef1[i1][i2]);
//		  vec_coef1_lump.push_back(coef1[i1][i2]);
	  }
  }
  // generate second coefficient matrix: C(u)*cos(((2*x+1) * u * PI) / 16)
  for (int i1=0;i1<8;i1++) {
	  for (int i2=0;i2<8;i2++) {
		  tmp_coef = C(i2)*cosf(((2*i1+1) * i2 * PI) / 16);
		  coef2[i1][i2] = float2int_idct(tmp_coef, cos_bi2, cos_bf2);
//		  coef2_err[i1][i2] = tmp_coef - ((float)coef2[i1][i2])/pow(2.0, cos_bf2);
//		  printf ("SGLEE COEF2: %f, %f \n", tmp_coef*pow(2.0,cos_bf2), coef2_err[i1][i2]);
		  if (io_capture2) vec_coef2[i1*8+i2].push_back(coef2[i1][i2]);
//		  vec_coef2_lump.push_back(coef2[i1][i2]);
	  }
  }
  ////////////////////////////////////////////////////////////////////////////

  int blk_cnt = 0;
	// Just the decode the image by 'macroblock' (size is 8x8, 8x16, or 16x16)
	for (int y=0 ; y<(int)jdata->m_height; y+=ystride_by_mcu)
	{
		for (int x=0; x<(int)jdata->m_width; x+=xstride_by_mcu)
		{
			jdata->m_colourspace = jdata->m_rgb + x*3 + (y *jdata->m_width*3);

			// Decode MCU Plane
//			DecodeMCU2_ac(jdata, hFactor, vFactor, &elem_pwr[0], &elem_y[0], &elem_cb[0], &elem_cr[0]);
//			DecodeMCU3_ac(jdata, hFactor, vFactor, &elem_pwr[0], &elem_y[0], &elem_cb[0], &elem_cr[0], coef1, coef2);
			DecodeMCU3_ac(jdata, hFactor, vFactor, &elem_pwr[0], &elem_y[0], &elem_cb[0], &elem_cr[0], coef1, coef2, block_cnt);


      block_cnt += 1;

      YCrCB_to_RGB24_Block8x8(jdata, hFactor, vFactor, x, y, jdata->m_width, jdata->m_height);

      blk_cnt++;
		}
	}

  //printf ("SGLEE BLK CNT: %d \n", blk_cnt);

  for (int k=0;k<8;k++) {
    for (int m=0;m<8;m++) {
      elem_pwr_avg[8*k+m] = elem_pwr_sum[k][m]/(float)block_cnt;
      // printf ("SGLEE ELEM AVG PWR: %f at %d,%d \n", elem_pwr_avg[k][m],k,m);
    }
  }

	return 0;
}
*/

/***************************************************************************/
//
// Take Jpg data, i.e. jpg file read into memory, and decompress it to an
// array of rgb pixel values.
//
// Note - Memory is allocated for this function, so delete it when finished
//
/***************************************************************************/
int DecodeJpgFileData(const unsigned char* buf, // Jpg file in memory
					  const int sizeBuf,		// Size jpg in bytes in memory
					  unsigned char** rgbpix,	// Output rgb pixels
					  unsigned int* width,		// Output image width
					  unsigned int* height)		// Output image height
{
	// Allocate memory for our decoded jpg structure, all our data will be
	// decompressed and stored in here for the various stages of our jpeg decoding
	stJpegData* jdec = new stJpegData();
	memset(jdec, 0, sizeof(stJpegData));

	if (jdec == NULL)
	{
		dprintf("Not enough memory to alloc the structure need for decompressing\n");
		// sglee DBG_HALT;
		return 0;
	}

	// Start Parsing.....reading & storing data
	if (JpegParseHeader(jdec, buf, sizeBuf)<0)
	{
		dprintf("ERROR > parsing jpg header\n");
		// sglee DBG_HALT;
	}

	// We've read it all in, now start using it, to decompress and create rgb values
	dprintf("Decoding JPEG image...\n");
	JpegDecode(jdec);

	// Get the size of the image
	JpegGetImageSize(jdec, width, height);

	*rgbpix = jdec->m_rgb;

	// Release the memory for our jpeg decoder structure jdec
	delete jdec;

	return 1;
}

int DecodeJpgFileData_fixed(const unsigned char* buf, // Jpg file in memory
					  const int sizeBuf,		// Size jpg in bytes in memory
					  unsigned char** rgbpix,	// Output rgb pixels
					  unsigned int* width,		// Output image width
					  unsigned int* height // ,		// Output image height
            // double *elem_pwr_avg
            )
{

	// Allocate memory for our decoded jpg structure, all our data will be
	// decompressed and stored in here for the various stages of our jpeg decoding
	stJpegData* jdec = new stJpegData();
	memset(jdec, 0, sizeof(stJpegData));

	if (jdec == NULL)
	{
		dprintf("Not enough memory to alloc the structure need for decompressing\n");
		// sglee DBG_HALT;
		return 0;
	}

	// Start Parsing.....reading & storing data
	if (JpegParseHeader(jdec, buf, sizeBuf)<0)
	{
		dprintf("ERROR > parsing jpg header\n");
		// sglee DBG_HALT;
	}

	// We've read it all in, now start using it, to decompress and create rgb values
	dprintf("Decoding JPEG image...\n");
	JpegDecode_fixed(jdec); //, elem_pwr_avg);

	// Get the size of the image
	JpegGetImageSize(jdec, width, height);

	*rgbpix = jdec->m_rgb;

	// Release the memory for our jpeg decoder structure jdec
	delete jdec;

	return 1;
}

/*
int DecodeJpgFileData_ac(const unsigned char* buf, // Jpg file in memory
					  const int sizeBuf,		// Size jpg in bytes in memory
					  unsigned char** rgbpix,	// Output rgb pixels
					  unsigned int* width,		// Output image width
					  unsigned int* height,		// Output image height
            double *elem_pwr_avg
            )
{

	// Allocate memory for our decoded jpg structure, all our data will be
	// decompressed and stored in here for the various stages of our jpeg decoding
	stJpegData* jdec = new stJpegData();
	memset(jdec, 0, sizeof(stJpegData));

	if (jdec == NULL)
	{
		dprintf("Not enough memory to alloc the structure need for decompressing\n");
		// sglee DBG_HALT;
		return 0;
	}

	// Start Parsing.....reading & storing data
	if (JpegParseHeader(jdec, buf, sizeBuf)<0)
	{
		dprintf("ERROR > parsing jpg header\n");
		// sglee DBG_HALT;
	}

	// We've read it all in, now start using it, to decompress and create rgb values
	dprintf("Decoding JPEG image...\n");
	JpegDecode_ac(jdec, elem_pwr_avg);

	// Get the size of the image
	JpegGetImageSize(jdec, width, height);

	*rgbpix = jdec->m_rgb;

	// Release the memory for our jpeg decoder structure jdec
	delete jdec;

	return 1;
}
*/

/***************************************************************************/
//
// Load one jpeg image, and decompress it, and save the result.
//
/***************************************************************************/
int ConvertJpgFile(char* szJpgFileInName, char * szBmpFileOutName)
{
	FILE *fp;
	unsigned int lengthOfFile;
	unsigned char *buf;


	// Load the Jpeg into memory
	fp = fopen(szJpgFileInName, "rb");
	if (fp == NULL)
	{
		dprintf("Cannot open jpg file: %s\n", szJpgFileInName);
		// sglee DBG_HALT;
		return 0;
	}

	lengthOfFile = FileSize(fp);

  //printf ("SGLEE JPG SIZE: %d \n", lengthOfFile);

	buf = new unsigned char[lengthOfFile + 4];// +4 is safety padding
	if (buf == NULL)
	{
		dprintf("Not enough memory for loading file\n");
		// sglee DBG_HALT;
		return 0;
	}
	fread(buf, lengthOfFile, 1, fp);
	fclose(fp);

	unsigned char* rgbpix = NULL;

  double elem_pwr_avg[64];

	unsigned int width  = 0;
	unsigned int height = 0;
	DecodeJpgFileData(buf, lengthOfFile, &rgbpix, &width, &height);

  for (int k=0;k<64;k++) {
    // printf ("SGLEE ELEM AVG PWR: %f at %d \n", elem_pwr_avg[k],k);
  }

	if (rgbpix==NULL)
	{
		dprintf("Failed to decode jpg\n");
		// sglee DBG_HALT;
		return 0;
	}

	// Delete our data we read in from the file
	delete[] buf;

	// Save it
	WriteBMP24(szBmpFileOutName, width, height, rgbpix);

	// Since we don't need the pixel information anymore, we must
	// release this as well
	delete[] rgbpix;

	return 1;
}

int ConvertJpgFile_fixed(char* szJpgFileInName, char * szBmpFileOutName)
{
	FILE *fp;
	unsigned int lengthOfFile;
	unsigned char *buf;


	// Load the Jpeg into memory
	fp = fopen(szJpgFileInName, "rb");
	if (fp == NULL)
	{
		dprintf("Cannot open jpg file: %s\n", szJpgFileInName);
		// sglee DBG_HALT;
		return 0;
	}

	lengthOfFile = FileSize(fp);

  //printf ("SGLEE JPG SIZE: %d \n", lengthOfFile);

	buf = new unsigned char[lengthOfFile + 4];// +4 is safety padding
	if (buf == NULL)
	{
		dprintf("Not enough memory for loading file\n");
		// sglee DBG_HALT;
		return 0;
	}
	fread(buf, lengthOfFile, 1, fp);
	fclose(fp);

	unsigned char* rgbpix = NULL;

	unsigned int width  = 0;
	unsigned int height = 0;
	// sglee DecodeJpgFileData(buf, lengthOfFile, &rgbpix, &width, &height);
	DecodeJpgFileData_fixed(buf, lengthOfFile, &rgbpix, &width, &height); //, &elem_pwr_avg[0]);

  //for (int k=0;k<64;k++) {
  //  printf ("SGLEE ELEM AVG PWR: %f at %d \n", elem_pwr_avg[k],k);
  //}

	if (rgbpix==NULL)
	{
		dprintf("Failed to decode jpg\n");
		// sglee DBG_HALT;
		return 0;
	}

	// Delete our data we read in from the file
	delete[] buf;

	// Save it
	WriteBMP24(szBmpFileOutName, width, height, rgbpix);

	// Since we don't need the pixel information anymore, we must
	// release this as well
	delete[] rgbpix;

	return 1;
}

/*
int ConvertJpgFile_ac(char* szJpgFileInName, char * szBmpFileOutName)
{
	FILE *fp;
	unsigned int lengthOfFile;
	unsigned char *buf;


	// Load the Jpeg into memory
	fp = fopen(szJpgFileInName, "rb");
	if (fp == NULL)
	{
		dprintf("Cannot open jpg file: %s\n", szJpgFileInName);
		// sglee DBG_HALT;
		return 0;
	}

	lengthOfFile = FileSize(fp);

  //printf ("SGLEE JPG SIZE: %d \n", lengthOfFile);

	buf = new unsigned char[lengthOfFile + 4];// +4 is safety padding
	if (buf == NULL)
	{
		dprintf("Not enough memory for loading file\n");
		// sglee DBG_HALT;
		return 0;
	}
	fread(buf, lengthOfFile, 1, fp);
	fclose(fp);

	unsigned char* rgbpix = NULL;

	unsigned int width  = 0;
	unsigned int height = 0;
	// sglee DecodeJpgFileData(buf, lengthOfFile, &rgbpix, &width, &height);
	DecodeJpgFileData_ac(buf, lengthOfFile, &rgbpix, &width, &height, &elem_pwr_avg[0]);

  //for (int k=0;k<64;k++) {
  //  printf ("SGLEE ELEM AVG PWR: %f at %d \n", elem_pwr_avg[k],k);
  //}

	if (rgbpix==NULL)
	{
		dprintf("Failed to decode jpg\n");
		// sglee DBG_HALT;
		return 0;
	}

	// Delete our data we read in from the file
	delete[] buf;

	// Save it
	WriteBMP24(szBmpFileOutName, width, height, rgbpix);

	// Since we don't need the pixel information anymore, we must
	// release this as well
	delete[] rgbpix;

	return 1;
}
*/

