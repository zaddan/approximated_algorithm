/**
* @file Utilities.h
* @author Muhammad Usman Karim Khan, Muhammad Shafique, Lars Bauer
* Group: Prof. Joerg Henkel at Chair for Embedded Systems (CES), KIT
* @brief This file contains the general utility functions
*/

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "TypeDefs.h"

/**
*	Get time in mili seconds.
*/
u32 GetTimeInMiliSec();

/**
*	Get time in micro seconds.
*/
u64 GetTimeInMicroSec();

/**
*	Get time stamp counter
*/
u64	ReadTSC();

/**
*	Bubble sort the values and their indexes
*/
void BubbleSortValIndex(i32 *piVals, i32 *piIdx, u32 uiSize);

/**
*	Get current date and time
*/
void GetCurrentDateTime(i8 *piBuff, u32 uiBuffSize);

/**
*	Multiple two 64-bit matrices A = BxC.
*/
void MatMulFloat64(f64 *m_pfA, f64 *m_pfB, f64 *m_pfC, u32 uiNumRowsB, u32 uiNumColsB, u32 uiNumColsC);

/**
*	Get the computer name.
*	@param bMachineName Name of the machine which will be filled by this function.
*	@param uiStrSize Size of bMachineName in bytes. 
*/
void GetMachineName(i8 *bMachineName, u32 uiStrSize);

/**
*	Determine if the file exists
*	@param pbFileName Name of the file.
*	@return True if the file exists.
*/
bit IsFileExist(const i8 *pbFileName);

#endif	// __UTILITIES_H__