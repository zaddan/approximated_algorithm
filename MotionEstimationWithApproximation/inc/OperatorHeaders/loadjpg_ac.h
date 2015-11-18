/*
 * loadjpg_ac.h
 *
 *  Created on: Mar 4, 2015
 *      Author: sglee
 */

#ifndef LOADJPG_AC_H_
#define LOADJPG_AC_H_

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

void PerformIDCT2_AC(int outBlock[8][8], const int inBlock[8][8]);
void PerformIDCT3_AC(int outBlock[8][8], const int inBlock[8][8], const int (*coef1)[8], const int (*coef2)[8]);
void PerformIDCT4_AC(int outBlock[8][8], const int inBlock[8][8], const int (*coef1)[8], const int (*coef2)[8]);

#endif /* LOADJPG_AC_H_ */
