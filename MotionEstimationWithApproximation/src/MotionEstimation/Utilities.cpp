/**
* @file Utilities.cpp
* @author Muhammad Usman Karim Khan, Muhammad Shafique, Lars Bauer
* Group: Prof. Joerg Henkel at Chair for Embedded Systems (CES), KIT
* @brief This file contains the general utility functions
*/

#define _WIN32 1
#include "Utilities.h"
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#if defined _WIN32 || defined _WIN64 
// sglee #include <intrin.h>
// sglee #include <Windows.h>
#else
#include <unistd.h>
#endif
u32 GetTimeInMiliSec()
{
//#ifdef _MSC_VER
//	return GetTickCount();
//#else
	// http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ((u32)ts.tv_sec * 1000LL + (u32)ts.tv_nsec / 1000000LL);
//#endif
}

u64 GetTimeInMicroSec()
{
#ifdef _MSC_VER
	LARGE_INTEGER uiFreq;        // ticks per second
	LARGE_INTEGER uiTick;	     // ticks
	f64 fTime;

	QueryPerformanceFrequency(&uiFreq);
	QueryPerformanceCounter(&uiTick);
	fTime = (uiTick.QuadPart) * 1000000.0 / uiFreq.QuadPart;
	return u64(fTime);
	return 0;
#else
	// http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ((u64)ts.tv_sec * 1000000LL + (u64)ts.tv_nsec / 1000LL);
#endif
}

/* sglee 
u64 ReadTSC()
{
#if defined _WIN32 || defined _WIN64
	return __rdtsc();
#else
	// Modified from: http://stackoverflow.com/questions/13772567/get-cpu-cycle-count
	u32 lo,hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((u64)hi << 32) | lo;
#endif
}
*/

void BubbleSortValIndex(i32 *piVals, i32 *piIdx, u32 uiSize)
{
	// Modified from: http://rosettacode.org/wiki/Sorting_algorithms/Bubble_sort#C
	int j, t = 1;
	while (uiSize-- && t)
		for (j = t = 0; j < i32(uiSize); j++) 
		{
			if (piVals[j] > piVals[j+1]) continue;
			t = piVals[j], piVals[j] = piVals[j+1], piVals[j+1] = t;
			t = piIdx[j], piIdx[j] = piIdx[j+1], piIdx[j+1] = t;
			t=1;
		}
}

void GetCurrentDateTime(i8 *piBuff, u32 uiBuffSize)
{
	// http://stackoverflow.com/questions/997946/c-get-current-time-and-date
	time_t now = time(0);
	struct tm tstruct;
	tstruct = *localtime(&now);
	// Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
	// for more information about date/time format
	strftime(piBuff, uiBuffSize, "%d-%m-%Y @ %X", &tstruct);
}

void MatMulFloat64(f64 *m_pfA, f64 *m_pfB, f64 *m_pfC, u32 uiNumRowsB, u32 uiNumColsB, u32 uiNumColsC)
{
	for(u32 i=0;i<uiNumRowsB;i++)
	{
		for(u32 j=0;j<uiNumColsC;j++)
		{
			m_pfA[i*uiNumColsC+j] = 0.0;
			for(u32 k=0;k<uiNumColsB;k++)
				m_pfA[i*uiNumColsC+j] += m_pfB[i*uiNumColsB+k]*m_pfC[k*uiNumColsC+j];
		}
	}
}

/*
void GetMachineName(i8 *bMachineName, u32 uiStrSize)
{
	strcpy(bMachineName,"Unknown_Machine");
#if defined _WIN32 || defined _WIN64
	// help from: http://stackoverflow.com/questions/504810/how-do-i-find-the-current-machines-full-hostname-in-c-hostname-and-domain-info
	WCHAR uMachineName[150];
	DWORD uLength = 150;
	GetComputerName(uMachineName,&uLength);
	for(u32 i=0;i<uiStrSize && i<150;i++)
		bMachineName[i] = i8(uMachineName[i]);
#else
	gethostname(bMachineName,uiStrSize);
#endif
}
*/

bit IsFileExist(const i8 *pbFileName)
{
	// help from: http://stackoverflow.com/questions/4316442/stdofstream-check-if-file-exists-before-writing
	struct stat buf;
	if (stat(pbFileName, &buf) != -1)
		return true;
	return false;
}
