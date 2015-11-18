/*
 * tb_jpg.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: sglee
 */


/***************************************************************************/
/*                                                                         */
/*  File: main.cpp                                                         */
/*  Autor: bkenwright@xbdev.net                                            */
/*  URL: www.xbdev.net                                                     */
/*                                                                         */
/***************************************************************************/
/*
	Jpeg File Format Explained
*/
/***************************************************************************/

// sglee #include <windows.h>

#include <stdio.h>		// sprintf(..), fopen(..)
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <math.h>
#include <vector>

#include "loadjpg.h"	// ConvertJpgFile(..)

#include "savejpg.h"    // SaveJpgFile(..)
#include "compare_bmp.h"    // SaveJpgFile(..)
#include "ac_types.h"
#include "io_stat.h"

/***************************************************************************/
/*                                                                         */
/* FeedBack Data                                                           */
/*                                                                         */
/***************************************************************************/

int f1 = 0;
int f2 = 8;
int f3 = 8;
int f4 = 8;
int fc1 = 0;
int fc2 = 12;
int fc3 = 12;
int fc4 = 12;
BYTE scalefactor = 10;

//double blk_pwr_avg[64];
//double noise_pwr_avg[64];
double elem_pwr_avg[64];
//double elem_pwr_avg2[64];
//double elem_noise_avg[64];

// element-wise IDCT input vectors
vector<ACVEC> vec_y(64, ACVEC(4096));
vector<ACVEC> vec_cb(64, ACVEC(4096));
vector<ACVEC> vec_cr(64, ACVEC(4096));
vector<ACVEC> vec_y2(64);
vector<ACVEC> vec_cb2(64);
vector<ACVEC> vec_cr2(64);

vector<ACVEC> vec_y_s0a(64);
vector<ACVEC> vec_cb_s0a(64);
vector<ACVEC> vec_cr_s0a(64);
vector<ACVEC> vec_y_s1a(64);
vector<ACVEC> vec_cb_s1a(64);
vector<ACVEC> vec_cr_s1a(64);

// lumped IDCT input vectors
ACVEC vec_y_lump(64*4096);
ACVEC vec_cb_lump(64*4096);
ACVEC vec_cr_lump(64*4096);
ACVEC vec_coef1(64);
ACVEC vec_coef2(64);
float coef1_err[8][8];
float coef2_err[8][8];
ACVEC vec_y_lump2;
ACVEC vec_cb_lump2;
ACVEC vec_cr_lump2;
ACVEC vec_s0a_lump;
ACVEC vec_s1a_lump;

//Saving debug information to a log file
void dprintf(const char *fmt, ...)
{
/*
	va_list parms;
	char buf[256];

	// Try to print in the allocated space.
	va_start(parms, fmt);
	vsprintf (buf, fmt, parms);
	va_end(parms);

	// Write the information out to a txt file
	FILE *fp = fopen("output.txt", "a+");
	fprintf(fp, "%s", buf);
	fclose(fp);
*/
}// End dprintf(..)



/***************************************************************************/
/*                                                                         */
/* Entry Point                                                             */
/*                                                                         */
/***************************************************************************/


// sglee int __stdcall WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
int main_jpg ()
{
	double corr_th = 0.95;

    double scalefactor_vec[7] = {1, 11, 21, 31, 61, 121, 251};
    double psnr_ref_vec[7] = {48.259735, 45.054540, 42.609611, 41.129973, 38.749041, 36.768027, 34.591238};
    //double wl_set[7][4] = {{0,7,2,7}, {0,7,2,6}, {0,6,1,6}, {0,6,2,5}, {0,5,1,5}, {0,5,0,5}, {0,3,0,5}};
    double wl_set[7][4] = {{0,10,10,10}, {0,7,2,6}, {0,6,1,6}, {0,6,2,5}, {0,5,1,5}, {0,5,0,5}, {0,3,0,5}};

    scalefactor = scalefactor_vec[0];
    f1 = wl_set[0][0];
    f2 = wl_set[0][1];
    f3 = wl_set[0][2];
    f4 = wl_set[0][3];

    printf ("SGLEE LOOP CMP LV: %d, WL: %d,%d,%d,%d \n", scalefactor, f1,f2,f3,f4);

    // Create a jpg from a bmp
	SaveJpgFile("image/lena_ref.bmp", "image/lena_ref.jpg");

	// Create a bmp from a jpg
	ConvertJpgFile("image/lena_ref.jpg", "image/lena_float.bmp");
	ConvertJpgFile_ac("image/lena_ref.jpg", "image/lena_fixed.bmp");

	vector<io_stat> s0m_corr(64, io_stat(16, 2));
	vector<io_stat> s1m_corr(64, io_stat(16, 2));
	vector<io_stat> s0a_corr(64, io_stat(16, 1));

	for (int i=0;i<64;i++) {
		s0m_corr[i].gen_stat(vec_y[i], corr_th);
		//s0m_corr[i].gen_stat(vec_y[i], corr_th);
		s1m_corr[i].gen_stat(vec_y2[i], corr_th);
		//s0a_corr[i].gen_stat(vec_y_s0a[i], vec_y_s0a[i], corr_th, corr_th);
	}

//	io_stat s0a_corr(32, 1, vec_s0a_lump, vec_s0a_lump, corr_th, corr_th);
//	io_stat s1m_corr(16, 2, vec_y_lump2, vec_coef1, corr_th, corr_th);
//	io_stat s1a_corr(32, 1, vec_s1a_lump, vec_s1a_lump, corr_th, corr_th);

	s0m_corr[0].print_io_stat();

    /*
    int m;
    double elem_pwr_sum[64];
    double elem_pwr_sum2[64];
    double elem_noise_sum[64];
    double elem_snr[64];

    for (m=0;m<64;m++) {
      elem_pwr_sum[m] = 0;
      elem_pwr_sum2[m] = 0;
      elem_noise_sum[m] = 0;
    }

    for (m=0;m<64*4096;m++) {
      elem_pwr_sum[m%64] += elem_pwr_y1[m]*elem_pwr_y1[m]+elem_pwr_cr1[m]*elem_pwr_cr1[m]+elem_pwr_cb1[m]*elem_pwr_cb1[m];
      elem_pwr_sum2[m%64] += elem_pwr_y2[m]*elem_pwr_y2[m]+elem_pwr_cr2[m]*elem_pwr_cr2[m]+elem_pwr_cb2[m]*elem_pwr_cb2[m];
      elem_noise_sum[m%64] += (elem_pwr_y1[m]-elem_pwr_y2[m])*(elem_pwr_y1[m]-elem_pwr_y2[m]) +
                             (elem_pwr_cr1[m]-elem_pwr_cr2[m])*(elem_pwr_cr1[m]-elem_pwr_cr2[m]) +
                             (elem_pwr_cb1[m]-elem_pwr_cb2[m])*(elem_pwr_cb1[m]-elem_pwr_cb2[m]);
    }

    for (m=0;m<64;m++) {
      elem_pwr_avg[m] = elem_pwr_sum[m]/(3*4096);
      elem_pwr_avg2[m] = elem_pwr_sum2[m]/(3*4096);
      elem_noise_avg[m] = elem_noise_sum[m]/(3*4096);
      printf ("SGLEE ELEM PWR: %f, ELEM PWR+NOISE: %f, ELEM NOISE: %f, SNR(dB): %f at IDX: %d \n", elem_pwr_avg[m], elem_pwr_avg2[m], elem_noise_avg[m], 10 * log10( 255.0*255.0 / elem_noise_avg[m]), m);
    }
    */

    comparebmp("image/lena_ref.bmp", "image/lena_float.bmp");
    comparebmp("image/lena_ref.bmp", "image/lena_fixed.bmp");

	return 0;
}// End WinMain(..)




