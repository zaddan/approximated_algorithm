/*
 * loadjpg.h
 *
 *  Created on: Feb 27, 2015
 *      Author: sglee
 */

#ifndef LOADJPG_H_
#define LOADJPG_H_

#ifndef DBG_HALT

	#define DBG_HALT __asm{ int 0x10 }
	#define DBG_ASSERT(exp) {if ( !(exp) ) {DBG_HALT;}}

#endif //DBG_HALT

#include <vector>
//#include "ac_types.h"

/*
// sglee global variables for PSNR estimation
extern int f1;
extern int f2;
extern int f3;
extern int f4;
extern double elem_pwr_avg[64];
//extern double elem_pwr_avg2[64];
//extern double elem_noise_avg[64];
//extern float elem_pwr_y2[64*4096];
//extern float elem_pwr_cb2[64*4096];
//extern float elem_pwr_cr2[64*4096];

extern vector<ACVEC> vec_y;
extern vector<ACVEC> vec_cb;
extern vector<ACVEC> vec_cr;
extern vector<ACVEC> vec_y2;
extern vector<ACVEC> vec_cb2;
extern vector<ACVEC> vec_cr2;

extern ACVEC vec_y_lump;
extern ACVEC vec_cb_lump;
extern ACVEC vec_cr_lump;

extern vector<ACVEC> vec_coef1;
extern vector<ACVEC> vec_coef2;
extern ACVEC vec_coef1_lump;
extern ACVEC vec_coef2_lump;
extern float coef1_err[8][8];
extern float coef2_err[8][8];

extern ACVEC vec_y2_lump;
extern ACVEC vec_cb2_lump;
extern ACVEC vec_cr2_lump;

extern ACVEC vec_y_s0a_lump;
extern ACVEC vec_y_s1a_lump;
extern ACVEC vec_y_s0a1_lump;
extern ACVEC vec_y_s1a1_lump;

extern vector<ACVEC> vec_y_s0a1;
extern vector<ACVEC> vec_y_s0a2;
extern vector<ACVEC> vec_cb_s0a;
extern vector<ACVEC> vec_cr_s0a;
extern vector<ACVEC> vec_y_s1a1;
extern vector<ACVEC> vec_y_s1a2;
extern vector<ACVEC> vec_cb_s1a;
extern vector<ACVEC> vec_cr_s1a;

extern vector<ACVEC> vec_y_s0a;
extern vector<ACVEC> vec_y_s1a;

extern bool ADD_UPPER;
extern int ADD_TYPE;
extern int mode;
extern bool io_capture2;

extern int s0m_type;
extern int s0a_type;
extern int s1m_type;
extern int s1a_type;

// temporal
extern vector<ACVEC> s0m_err;
*/
// Takes the rgb pixel values and creates a bitmap file
inline void WriteBMP24(const char* szBmpFileName, int Width, int Height, unsigned char* RGB);

// Pass in the whole jpg file from memory, and it decodes it to RGB pixel data
inline int DecodeJpgFileData(const unsigned char* buf, int sizeBuf, unsigned char** rgbpix, unsigned int* width, unsigned int* height);
// Don't forget to delete[] rgbpix if you use DecodeJpgFileData..

int func(int x, int y, const int block[8][8]);

// Pass in the filename and it creates a bitmap
int ConvertJpgFile(char* szJpgFileInName, char * szBmpFileOutName);
int ConvertJpgFile_fixed(char* szJpgFileInName, char * szBmpFileOutName);
int ConvertJpgFile_ac(char* szJpgFileInName, char * szBmpFileOutName);

float C(int u);


#endif /* LOADJPG_H_ */
