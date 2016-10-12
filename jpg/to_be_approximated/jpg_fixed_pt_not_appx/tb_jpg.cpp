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

#include <fstream>

#include <sstream>
#include <string>
#include <stdio.h>		// sprintf(..), fopen(..)
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <math.h>
#include <vector>

#include "loadjpg.h"	// ConvertJpgFile(..)

#include "savejpg.h"    // SaveJpgFile(..)
#include "compare_bmp.h"    // SaveJpgFile(..)

/***************************************************************************/
/*                                                                         */
/* Entry Point                                                             */
/*                                                                         */
/***************************************************************************/

using namespace std;
ofstream op_resultFile;
ofstream sum_resultFile;
ifstream sum_sampleFile;

bool GEN_OP=false;
bool GEN_SUM=false;
bool SAMPLE_SUM=true;
// sglee int __stdcall WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
int main ()
{

    if (GEN_OP) { 
        op_resultFile.open("operand_results.txt");
    }

    if (GEN_SUM) { 
        sum_resultFile.open("sum_results.txt");
    }
    if (SAMPLE_SUM) { 
        sum_sampleFile.open("sum_as_input.txt");
    }
     
    /*
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
*/

    // Create a jpg from a bmp
	  SaveJpgFile("image/lena_ref.bmp", "image/lena_ref.jpg");

	  // Create a bmp from a jpg
	  ConvertJpgFile("image/lena_ref.jpg", "image/lena_float.bmp");
	  ConvertJpgFile_fixed("image/lena_ref.jpg", "image/lena_fixed.bmp");

    comparebmp("image/lena_ref.bmp", "image/lena_float.bmp");
    comparebmp("image/lena_ref.bmp", "image/lena_fixed.bmp");

    
	
    if (GEN_OP) { 
        op_resultFile.close();
    }
    if (GEN_SUM) { 
        sum_resultFile.close();
    }
    if (SAMPLE_SUM) { 
        sum_sampleFile.close();
    }
    
    
    return 0;
}




