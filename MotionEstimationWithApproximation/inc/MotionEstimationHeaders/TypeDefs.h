/**
* @file TypeDefs.h
* @author Muhammad Usman Karim Khan, Muhammad Shafique, Lars Bauer
* Group: Prof. Joerg Henkel at Chair for Embedded Systems (CES), KIT
* @brief This file contains the m_iType definitions and macros used in this project.
* @details Help is taken from http://msdn.microsoft.com/en-us/library/s3f49ktz(v=vs.80).aspx
*/


#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdio.h>
#include <assert.h>
#include <math.h>

#ifdef _MSC_VER
#include <intrin.h>	// @todo For linux, I think I can use x86intrin.h
#endif

#define			I32_MIN						-(1<<31)
#define			I32_MAX						((1<<30)-1)
#define			U32_MAX						((1<<30)-1)
#define			I8_MIN						-(1<<7)
#define			I8_MAX						((1<<7)-1)
#define			U8_MAX						((1<<8)-1)
#define			I16_MIN						-(1<<15)
#define			I16_MAX						((1<<15)-1)
#define			U16_MAX						((1<<16)-1)

typedef signed int i32;
typedef unsigned int u32;
typedef char i8;
typedef unsigned char u8;
typedef unsigned char byte;
typedef signed short i16;
typedef unsigned short u16;
typedef bool bit;
typedef float f32;
typedef double f64;
#if defined WIN32 || defined WIN64
typedef __int64 i64;
typedef unsigned __int64 u64;
#define			I64_MIN 					(-9223372036854775807i64 - 1i64)
#define			I64_MAX						(9223372036854775807i64)
#define			U64_MAX						(18446744073709551615u64)	// Check this
#else
typedef long long i64;
typedef unsigned long long u64;
#define			I64_MIN 					(-9223372036854775807LL - 1LL)
#endif

typedef struct _pixel
{
	u32	x;
	u32	y;
}pixel;

// General macros
#ifndef max
#define 		max(a, b) 					(((a) > (b)) 	? 	(a) 		: 	(b))
#endif

#ifndef min
#define 		min(a, b) 					(((a) < (b)) 	? 	(a) 		: 	(b))
#endif

// Before using SWAP, define a variable tmp of the same class as A and B
#define			SWAP(A,B)	((A)^=(B),(B)^=(A),(A)^=(B))
#define 		MEDIAN_MV(a,b,c)			(min( max( min(a,b), c), max(a,b)))
#define 		ABS(A) 						((A)<(0) ? (-(A)):(A)) 			// ABS macro, faster than procedure
#define 		Clip1(a)            		((a)>255?255:((a)<0?0:(a)))
#define 		Clip3(min,max,val) 			(((val)<(min))?(min):(((val)>(max))?(max):(val)))
#define 		MUX(condition,a,b)        	(condition?a:b)
#define 		SIGN(A, B) 					((B >= 0) ? (A): (-(A))) 			// SIGN macro
#define			ROUND(x)					floor((x)+0.5)
#define			ADD_MOD_2(A, B)				(( (A) + (B) ) & 0x1)	// This is equal to XOR macro.
#ifndef			PI
#define			PI							3.141592654
#endif
#define			WITHIN(A,B,C)				(ABS(i32(A)-i32(B)) <= C? 1 : 0)	// Test if A and B are within a distance C from eachother
#define			RET_1_IF_TRUE(A)			( (A) != 0 ? 1 : 0 )	

// log2, help from http://stackoverflow.com/questions/6834868/macro-to-compute-number-of-bits-needed-to-store-a-number-n
#define			NBITS2(n)					(((n)&2)?1:0)
#define			NBITS4(n)					(((n)&0xC)?(2+NBITS2((n)>>2)):(NBITS2(n)))
#define			NBITS8(n)					(((n)&0xF0)?(4+NBITS4((n)>>4)):(NBITS4(n)))
#define			NBITS16(n)					(((n)&0xFF00)?(8+NBITS8((n)>>8)):(NBITS8(n)))
#define			NBITS32(n)					(((n)&0xFFFF0000)?(16+NBITS16((n)>>16)):(NBITS16(n)))
#define			LOG2(n)						((n)==0?0:NBITS32(n)+1)

// Assertion
#if defined _MSC_VER && defined _DEBUG
#define			MY_DEBUGBREAK()				__debugbreak()
#else
#define			MY_DEBUGBREAK()				assert(0)			// @todo I need to change this
#endif

#define			MAKE_SURE(cond, str)							\
	do{															\
		if(!(cond))												\
		{														\
			printf("%s\n",(str));								\
			printf("%s: line %d\n",__FILE__, __LINE__);			\
			fflush(NULL);										\
			MY_DEBUGBREAK();									\
		}														\
	}while(0)													\

// Sleep 
#ifdef _MSC_VER
#include <Windows.h>
#define		sleep(A)		Sleep(A)
#endif

// Saturation
#define			SATURATE_HIGH(A,B)			((A)>(B)?(B):(A))
#define			SATURATE_LOW(A,B)			((A)<(B)?(B):(A))
#define			SATURATE(A,B,C)				((A)<(B)?(B):((A)>(C)?(C):(A)))	// Saturate A between B (low) and C (high)
#define			QUANTIZE_HIGH(A,B,C)		((B)-floor(((B)-(A))/(C))*(C))	// Quantize A towards high, with B the maximum value and C the quantization step size

#endif	// __TYPEDEFS_H__
