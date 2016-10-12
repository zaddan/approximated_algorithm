/*
 * jpeg_helper.h
 *
 *  Created on: Feb 27, 2015
 *      Author: sglee
 */

/***************************************************************************/
#ifndef FIXED_H
#define FIXED_H

/*
// #include "fft.h"
// Takes the rgb pixel values and creates a bitmap file
inline void WriteBMP24(const char* szBmpFileName, int Width, int Height, unsigned char* RGB);

// Pass in the whole jpg file from memory, and it decodes it to RGB pixel data
inline int DecodeJpgFileData(const unsigned char* buf, int sizeBuf, unsigned char** rgbpix, unsigned int* width, unsigned int* height);
// Don't forget to delete[] rgbpix if you use DecodeJpgFileData..


// Pass in the filename and it creates a bitmap
int ConvertJpgFile(char* szJpgFileInName, char * szBmpFileOutName);
*/

/***************************************************************************/
// for fixed point conversion
// value is integer representation of data
// bi is the number of integer bits 
// bf is the number of fractional bits
struct q_format 
{
	int val;	// value
	int bi;		// # of integer bits (including sign bit)
	int bf; 	// # of fractional bits
};


// Floating to fixed point number
// Both input & output are 'double' but
// First, convert double to int. Input is rounded and clipped with bi and bf and
// then converted to double again to make the following calculation unchanged.
float float2fix_idct (float x, int bi, int bf);

int float2int_idct (float x, int bi, int bf);

float measure_power (const int val[8][8]);

double measure_power (signed char *val);


float measure_power (const float val[8][8]);

float measure_mean_var (const int val[8][8], float &mean, float &var);

int C_fixed(int u, int nbit);
q_format rnd_clp (const q_format &x, const int &bi, const int &bf);
#endif // FIXED_H
