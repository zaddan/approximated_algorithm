#include "fft.h"

// Rounding and Clipping of integer number
// Rounding first, clipping later
// e.g. if bf = 2 & bi = 2, the range of integer number is -7 ~ 7 
// There's no -8. 
q_format rnd_clp (const q_format &x, const int &bi, const int &bf)
{
  q_format tmp_rnd;
  q_format result;

	// Rounding first
	if (x.bf < bf)
		cout << "Rounding Error!\n" << endl;
	// No rounding
	else if (x.bf == bf)
	{
		tmp_rnd.val = x.val;
		tmp_rnd.bi = x.bi;
		tmp_rnd.bf = x.bf;
	}
	else 
	{
		tmp_rnd.bi = x.bi;
		tmp_rnd.bf = bf;
		// Actual rounding
		if ((x.val & (0x1 << (x.bf - bf - 1)) != 0) && ((x.val & 0x80000000) == 0))
			tmp_rnd.val = ((signed)(x.val & (0xffffffff << (x.bf - bf))) >> (x.bf-bf)) + 1;
		else 
			tmp_rnd.val = ((signed)(x.val & (0xffffffff << (x.bf - bf))) >> (x.bf-bf));
	}

	// Then Clipping
	if (tmp_rnd.bi < bi)
		cout  << "Clipping Error!\n" << endl;
	// No Clipping
	else if (tmp_rnd.bi == bi)
	{
		result.val = tmp_rnd.val;
		result.bi = tmp_rnd.bi;
		result.bf = tmp_rnd.bf;
	}
	else 
	{
		result.bi = tmp_rnd.bi;
		result.bf = tmp_rnd.bf;
		// Actual Clipping
		// Range is 2^(bi+bf)-1 >= result.val >= -2^(bi+bf)+1  
		// For example, if bi+bf = 10, 511 >= result.val >= -511
		if ((tmp_rnd.val & (0x1 << (tmp_rnd.bi + tmp_rnd.bf))) != 0) // negative number
		{
			if (tmp_rnd.val <= (-1)*pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1))
			{
				result.val = (-1)*pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1) + 1;
				//printf ("Clipping! %d, %d \n", tmp_rnd.val, result.val);
			}
			else 
				result.val = tmp_rnd.val;
		}
		else // positive number 
		{
			if (tmp_rnd.val >= pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1))
			{
				result.val = pow(2.0, tmp_rnd.bi + tmp_rnd.bf - 1) - 1;
				//printf ("Clipping! %d, %d \n", tmp_rnd.val, result.val);
			}
			else 
				result.val = tmp_rnd.val;
		}
	}

	return result;

}


/*
// Floating to fixed point number
// Both input & output are 'double' but
// First, convert double to int. Input is rounded and clipped with bi and bf and
// then converted to double again to make the following calculation unchanged.
cmplx float2fix (const cmplx2 &x, const int &bi, const int &bf)
{
	q_format r_tmp;
	q_format i_tmp;
	q_format r_int;
	q_format i_int;

	cmplx result;
	
	// Convertion to integer
	// For accurate rounding, need one more bit 
	r_tmp.val = (int) (x.r * pow (2.0, bf+1));
	i_tmp.val = (int) (x.i * pow (2.0, bf+1));

	r_tmp.bi = bi+1;
	i_tmp.bi = bi+1;
	r_tmp.bf = bf+1;
	i_tmp.bf = bf+1;

	// Rounding and Clipping
	r_int = rnd_clp (r_tmp, bi, bf);
	i_int = rnd_clp (i_tmp, bi, bf);

	// Convertion to double again
	result.r = ((double) r_int.val) / pow(2.0, bf);
	result.i = ((double) i_int.val) / pow(2.0, bf);
	
	return result;

} 
*/

cmplx float2fix (const cmplx2 &x, const int &bi, const int &bf)
{
	q_format r_tmp;
	q_format i_tmp;
	q_format r_int;
	q_format i_int;

	cmplx result;
	
	// Convertion to integer
	// For accurate rounding, need one more bit 
	r_tmp.val = (int) (x.r * pow (2.0, bf+1));
	i_tmp.val = (int) (x.i * pow (2.0, bf+1));

	r_tmp.bi = bi+1;
	i_tmp.bi = bi+1;
	r_tmp.bf = bf+1;
	i_tmp.bf = bf+1;

	// Rounding and Clipping
	r_int = rnd_clp (r_tmp, bi, bf);
	i_int = rnd_clp (i_tmp, bi, bf);

	// Convertion to double again
	result.r = r_int.val;
	result.i = i_int.val;
	
	return result;

} 

float float2fix(const float &x, const int &bi, const int &bf)
{
	q_format r_tmp;
	q_format r_int;

	float result;

	// Convertion to integer
	// For accurate rounding, need one more bit 
	r_tmp.val = (int)(x * pow(2.0, bf + 1));

	r_tmp.bi = bi + 1;
	r_tmp.bf = bf + 1;

	// Rounding and Clipping
	r_int = rnd_clp(r_tmp, bi, bf);

	// Convertion to double again
	result = r_int.val;

	return result;

}

int pow_2[MAXPOW];
int pow_4[MAXPOW];

// Twiddle factor generation 
void twiddle(cmplx2 &W, const int &N, const double &stuff)
{
    W.r=0.5*cos(stuff*2.0*PI/(double)N);
    W.i=-0.5*sin(stuff*2.0*PI/(double)N);
}

/*
// Reordeing of FFT result.
// The order of output of pipelined FFT is bit reversed ordered
// To make it normal order, we need reordering block
void bit_r4_reorder(vector<cmplx> &W, const int &N)
{
    int bits, i, j, k;
    double tempr, tempi;
    
    for (i=0; i<MAXPOW; i++)
	if (pow_2[i]==N) bits=i;

    for (i=0; i<N; i++)
    {
	j=0;
	for (k=0; k<bits; k+=2)
	{
	    if (i&pow_2[k]) j+=pow_2[bits-k-2];
	    if (i&pow_2[k+1]) j+=pow_2[bits-k-1];
	}

	if (j>i)  
	{
	    tempr=W[i].r;
	    tempi=W[i].i;
	    W[i].r=W[j].r;
	    W[i].i=W[j].i;
	    W[j].r=tempr;
	    W[j].i=tempi;
	}
    }
}
*/

//////////////////////////////////////////////////////////////////////////
// Floating point reference
//////////////////////////////////////////////////////////////////////////

// Radix4 module for each pipelined stage 
void radix4(vector<cmplx2>::iterator its, const int &N)
{ 
  int    n2, k1, N1, N2;
  cmplx2 W, bfly[4];

  N1=4;
  N2=N/4;
  
  // Do 4 Point DFT // 
  for (n2=0; n2<N2; n2++)
  {
		// Don't hurt the butterfly //
		bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
		bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
		bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
		bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
		bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
		bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
		bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
		bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;


		// Twiddle multiply
		for (k1=0; k1<N1; k1++)
		{
		  twiddle(W, N, (double)k1*(double)n2);
          (*(its+n2+N2*k1)).r = bfly[k1].r*W.r - bfly[k1].i*W.i;
          (*(its+n2+N2*k1)).i = bfly[k1].i*W.r + bfly[k1].r*W.i;
		}
  }
}

// 256 point FFT
void fft256_radix4 (vector<cmplx2> &x, const int &N)
{
	int k1;

	int N1, N2, N3, N4; // N for each stage
	int R2, R3, R4; // iteration number for each stage

	int i;

	N1 = N;
	N2 = N1/4;
	N3 = N2/4;
	N4 = N3/4;

	R2 = 4;
	R3 = 16;
	R4 = 64;

	/////////////////////////////////////
	// First stage 
	/////////////////////////////////////
	radix4(x.begin(), N1);

	/////////////////////////////////////
	// Second stage 
	/////////////////////////////////////
	for (k1=0; k1<R2; k1++)
	{
    radix4(x.begin()+(N2*k1), N2);
	}

	/////////////////////////////////////
	// Third stage 
	/////////////////////////////////////
	for (k1=0; k1<R3; k1++)
	{
    radix4(x.begin()+(N3*k1), N3);
	}

	/////////////////////////////////////
	// Last stage 
	/////////////////////////////////////
	for (k1=0; k1<R4; k1++)
	{
    radix4(x.begin()+(N4*k1), N4);
	}
} 

//////////////////////////////////////////////////////////////////////////
// Fixed point reference
//////////////////////////////////////////////////////////////////////////
void fft256_fixed(vector<cmplx> &data_fix, const int &N, const int *bi, const int *bf) {
		int Nstg = N;
		int stage_num = 0;

		int N1=4;
		int N2=Nstg/4;

		int n2, k1;
		cmplx W, bfly[4];
		cmplx2 W_float;
  
		q_format x_r; 
		q_format x_i;

		// FFT input 
		int bi_fft_inp = *bi;
		int bf_fft_inp = *bf;

		// FFT first stage output 
		int bi_fft_stg0 = *(bi+1);
		int bf_fft_stg0 = *(bf+1);

		// FFT second stage output 
		int bi_fft_stg1 = *(bi+2);
		int bf_fft_stg1 = *(bf+2);

		// FFT third stage output 
		int bi_fft_stg2 = *(bi+3);
		int bf_fft_stg2 = *(bf+3);

		// FFT last stage output 
		int bi_fft_stg3 = *(bi+4);
		int bf_fft_stg3 = *(bf+4);

		// FFT first stage twiddle
		int bi_fft_twd0 = bi_fft_stg0;
		int bf_fft_twd0 = bf_fft_stg0;

		// FFT second stage twiddle
		int bi_fft_twd1 = bi_fft_stg1;
		int bf_fft_twd1 = bf_fft_stg1;

		// FFT third stage twiddle
		int bi_fft_twd2 = bi_fft_stg2;
		int bf_fft_twd2 = bf_fft_stg2;

		// FFT last stage twiddle
		int bi_fft_twd3 = bi_fft_stg3;
		int bf_fft_twd3 = bf_fft_stg3;

		for (int s=0;s<N;s++) {
			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_inp;
			x_r.bf = bf_fft_inp;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_inp;
			x_i.bf = bf_fft_inp;

			x_r = rnd_clp (x_r, bi_fft_inp, bf_fft_inp); 
			x_i = rnd_clp (x_i, bi_fft_inp, bf_fft_inp);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;
		}

		vector<cmplx>::iterator its = data_fix.begin();

		// Do 4 Point DFT // 
		for (n2=0; n2<N2; n2++)
		{
			// Don't hurt the butterfly //
			bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
			bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
			bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
			bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
			bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
			bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
			bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
			bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

			// Twiddle multiply
			for (k1=0; k1<N1; k1++)
			{
				twiddle(W_float, Nstg, (double)k1*(double)n2);

				W = float2fix (W_float, bi_fft_twd0, bf_fft_twd0);

				(*(its+n2+N2*k1)).r = bfly[k1].r*W.r - bfly[k1].i*W.i;
				(*(its+n2+N2*k1)).i = bfly[k1].i*W.r + bfly[k1].r*W.i;
			}
		}

		its = data_fix.begin();

		///////////////////////////////////////
		// Stage1
		///////////////////////////////////////

		Nstg = N/4;
		stage_num = 1;
		N2=Nstg/4;
		int R2 = 4;
  
		for (int s=0;s<N;s++) {

			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_inp+3;
			x_r.bf = 2*bf_fft_inp;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_inp+3;
			x_i.bf = 2*bf_fft_inp;

			x_r = rnd_clp (x_r, bi_fft_stg0, bf_fft_stg0); 
			x_i = rnd_clp (x_i, bi_fft_stg0, bf_fft_stg0);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;
		}

		for (int k2=0; k2<R2; k2++)
		{

			// Do 4 Point DFT // 
			for (n2=0; n2<N2; n2++)
			{
				// Don't hurt the butterfly //
				bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
				bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
				bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
				bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
				bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
				bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
				bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
				bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{
					twiddle(W_float, Nstg, (double)k1*(double)n2);
					W = float2fix (W_float, bi_fft_twd1, bf_fft_twd1);
					(*(its+n2+N2*k1)).r = bfly[k1].r*W.r - bfly[k1].i*W.i;
					(*(its+n2+N2*k1)).i = bfly[k1].i*W.r + bfly[k1].r*W.i;
				}
			}

			its = its + Nstg;

		}

		its = data_fix.begin();


		///////////////////////////////////////
		// Stage2
		///////////////////////////////////////

		Nstg = N/16;
		stage_num = 2;
		N2=Nstg/4;
		int R3 = 16;
  
		for (int s=0;s<N;s++) {

			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_stg0+3;
			x_r.bf = 2*bf_fft_stg0;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_stg0+3;
			x_i.bf = 2*bf_fft_stg0;

			x_r = rnd_clp (x_r, bi_fft_stg1, bf_fft_stg1); 
			x_i = rnd_clp (x_i, bi_fft_stg1, bf_fft_stg1);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;
		}


		for (int k2=0; k2<R3; k2++)
		{

			// Do 4 Point DFT // 
			for (n2=0; n2<N2; n2++)
			{
				// Don't hurt the butterfly //
				bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
				bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
				bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
				bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
				bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
				bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
				bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
				bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{

					twiddle(W_float, Nstg, (double)k1*(double)n2);
					W = float2fix (W_float, bi_fft_twd2, bf_fft_twd2);
					(*(its+n2+N2*k1)).r = bfly[k1].r*W.r - bfly[k1].i*W.i;
					(*(its+n2+N2*k1)).i = bfly[k1].i*W.r + bfly[k1].r*W.i;
				}
			}

			its = its + Nstg;
		}

		its = data_fix.begin();

		///////////////////////////////////////
		// Stage3
		///////////////////////////////////////

		Nstg = N/64;
		stage_num = 3;
		N2=Nstg/4;
		int R4 = 64;
  
		for (int s=0;s<N;s++) {

			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_stg1+3;
			x_r.bf = 2*bf_fft_stg1;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_stg1+3;
			x_i.bf = 2*bf_fft_stg1;

			x_r = rnd_clp (x_r, bi_fft_stg2, bf_fft_stg2); 
			x_i = rnd_clp (x_i, bi_fft_stg2, bf_fft_stg2);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;
		}

		for (int k2=0; k2<R4; k2++)
		{

			// Do 4 Point DFT // 
			for (n2=0; n2<N2; n2++)
			{
				// Don't hurt the butterfly //
				bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
				bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
				bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
				bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
				bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
				bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
				bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
				bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{
				  twiddle(W_float, Nstg, (double)k1*(double)n2);
				  W = float2fix (W_float, bi_fft_twd3, bf_fft_twd3);
		          (*(its+n2+N2*k1)).r = bfly[k1].r*W.r - bfly[k1].i*W.i;
			      (*(its+n2+N2*k1)).i = bfly[k1].i*W.r + bfly[k1].r*W.i;
				}
			}

			its = its + Nstg;
		}

		its = data_fix.begin();

		for (int s=0;s<N;s++) {

			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_stg2+3;
			x_r.bf = 2*bf_fft_stg2;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_stg2+3;
			x_i.bf = 2*bf_fft_stg2;

			x_r = rnd_clp (x_r, bi_fft_stg3, bf_fft_stg3); 
			x_i = rnd_clp (x_i, bi_fft_stg3, bf_fft_stg3);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;
		}
}
/*
// Radix4 module for each pipelined stage (Fixed point)
void radix4_fixed(vector<cmplx>::iterator its, const int &N)
{ 
  int    n2, k1, N1, N2;
  cmplx W, bfly[4];
  cmplx W2;
  int stage_num = 0;

  N1=4;
  N2=N/4;
  
  // Do 4 Point DFT // 
  for (n2=0; n2<N2; n2++)
  {
		// Don't hurt the butterfly //
		bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
		bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
		bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
		bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
		bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
		bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
		bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
		bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;


		// Twiddle multiply
		for (k1=0; k1<N1; k1++)
		{
		  twiddle(W, N, (double)k1*(double)n2);

		  W = float2fix (W, bi_fft_twd0, bf_fft_twd0);

          (*(its+n2+N2*k1)).r = bfly[k1].r*W.r - bfly[k1].i*W.i;
          (*(its+n2+N2*k1)).i = bfly[k1].i*W.r + bfly[k1].r*W.i;
		}
  }
}

// 256 point pipelined FFT for fixed point
void fft256_radix4_fixed (vector<cmplx> &x, const int &N)
{
	int k1;

	int N1, N2, N3, N4; // N for each stage
	int R2, R3, R4; // iteration number for each stage


	N1 = N;
	N2 = N1/4;
	N3 = N2/4;
	N4 = N3/4;

	R2 = 4;
	R3 = 16;
	R4 = 64;

	////////////////////////////////////////////
	// First stage
	////////////////////////////////////////////

#ifdef FIXED
	int i;
	for (i=0;i<N;i++)
		x[i] = float2fix(x[i],bi_fft_stg0, bf_fft_stg0); 
#endif


	//for (m=0;m<N;m++)
	//	printf ("%f, %f\n", x1[m].r-x[m].r, x1[m].i-x[m].i);

	radix4_fixed(x.begin(), N1, 0);

	

	////////////////////////////////////////////
	// Second stage
	////////////////////////////////////////////

#ifdef FIXED
	for (i=0;i<N;i++)
		x[i] = float2fix(x[i], bi_fft_stg1, bf_fft_stg1);
#endif

	for (k1=0; k1<R2; k1++)
	{
	    radix4_fixed(x.begin()+(N2*k1), N2, 1);

	}

	////////////////////////////////////////////
	// Third stage
	////////////////////////////////////////////

#ifdef FIXED
	for (i=0;i<N;i++)
		x[i] = float2fix(x[i], bi_fft_stg2, bf_fft_stg2);
#endif

	for (k1=0; k1<R3; k1++)
	{
	    radix4_fixed(x.begin()+(N3*k1), N3, 2);
	}

	////////////////////////////////////////////
	// Last stage
	////////////////////////////////////////////

#ifdef FIXED
	for (i=0;i<N;i++)
		x[i] = float2fix(x[i], bi_fft_stg3, bf_fft_stg3);
#endif

	for (k1=0; k1<R4; k1++)
	{
	    radix4_fixed(x.begin()+(N4*k1), N4, 3);
	}

} // fft_radix4_fixed
*/