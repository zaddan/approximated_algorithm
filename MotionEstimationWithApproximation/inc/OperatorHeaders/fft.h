#ifndef _FFT
#define _FFT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <random>

#define PI 3.14159265359
#define MAXPOW 24
#define FIXED
#define VAR_ITER

using namespace std;

// Structure for cmplx number
struct cmplx
{
    int r;
    int i;
};

// Structure for cmplx number
struct cmplx2
{
    double r;
    double i;
};

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

q_format rnd_clp (const q_format &x, const int &bi, const int &bf);
cmplx float2fix (const cmplx2 &x, const int &bi, const int &bf);
float float2fix(const float &x, const int &bi, const int &bf);
void twiddle(cmplx2 &W, const int &N, const double &stuff);
void radix4(vector<cmplx2>::iterator its, const int &N);
void fft256_radix4 (vector<cmplx2> &x, const int &N);
void fft256_fixed(vector<cmplx> &data_fix, const int &N, const int *bi, const int *bf);



#endif