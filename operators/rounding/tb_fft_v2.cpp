//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <random>

#include "sim_config.h"
#include "hw_ac.h"
#include "ac_types.h"
#include "eta1.h"
#include "eta2.h"
#include "loa.h"
#include "bam.h"
#include "etm.h"
#include "operand_gen.h"
#include "io_stat.h"
#include "error_model.h"
#include "analysis.h"
#include "fft.h"

using namespace std;

//////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////

// Simulator
int main_fft2()
{
	int num_pac = 400;
    int N = 256;
	int radix = 4;
    int i;
	int gen_success;
	int Nt = 16;
    vector<cmplx> data_fix(N);
    vector<cmplx> data_fix2(N);
    vector<cmplx2> data_float(N);

	int mode;
	mode = 4; // '1' for unit level analysis, '2' for evaluation

	ACVEC x1_vec(N*num_pac);
	ACVEC x2_vec(N*num_pac);

	float mean_x1 = 0;
	float var_x1 = 1000;
	float mean_x2 = 0;
	float var_x2 = 1000;

	// input generation
	gen_success = operand_gen(Nt, mean_x1, var_x1, mean_x2, var_x2, x1_vec, x2_vec, 1);

	int bi[5] = {3, 3, 3, 3, 3};
	int bf[5] = {13, 13, 13, 13, 13};

	// FFT input 
	int bi_fft_inp = bi[0];
	int bf_fft_inp = bf[0];

	// FFT first stage output 
	int bi_fft_stg0 = bi[1];
	int bf_fft_stg0 = bf[1];

	// FFT second stage output 
	int bi_fft_stg1 = bi[2];
	int bf_fft_stg1 = bf[2];

	// FFT third stage output 
	int bi_fft_stg2 = bi[3];
	int bf_fft_stg2 = bf[3];

	// FFT last stage output 
	int bi_fft_stg3 = bi[4];
	int bf_fft_stg3 = bf[4];

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

	int    n2, k1, N1, N2;
	cmplx W, bfly[4];
	cmplx2 W_float;

	///////////////////////////////////////////////////////////
	// AC related parameters
	///////////////////////////////////////////////////////////
	double corr_th = 0.95;
	ACVEC s0i0_vec;
	ACVEC s0i1_vec;
	ACVEC s0i2_vec;
	ACVEC s0i3_vec;
	ACVEC s0b0_vec;
	ACVEC s0b1_vec;
	ACVEC s0t0_vec;
	ACVEC s0t1_vec;
	ACVEC s0c0_vec;
	ACVEC s0c1_vec;
	ACVEC s0c2_vec;
	ACVEC s0c3_vec;

	ACVEC s1i0_vec;
	ACVEC s1i1_vec;
	ACVEC s1i2_vec;
	ACVEC s1i3_vec;
	ACVEC s1b0_vec;
	ACVEC s1b1_vec;
	ACVEC s1t0_vec;
	ACVEC s1t1_vec;
	ACVEC s1c0_vec;
	ACVEC s1c1_vec;
	ACVEC s1c2_vec;
	ACVEC s1c3_vec;

	ACVEC s2i0_vec;
	ACVEC s2i1_vec;
	ACVEC s2i2_vec;
	ACVEC s2i3_vec;
	ACVEC s2b0_vec;
	ACVEC s2b1_vec;
	ACVEC s2t0_vec;
	ACVEC s2t1_vec;
	ACVEC s2c0_vec;
	ACVEC s2c1_vec;

	ACVEC s3i0_vec;
	ACVEC s3i1_vec;
	ACVEC s3i2_vec;
	ACVEC s3i3_vec;
	ACVEC s3b0_vec;
	ACVEC s3b1_vec;
	ACVEC s3t0_vec;
	ACVEC s3t1_vec;
	ACVEC s3c0_vec;
	ACVEC s3c1_vec;

	//////////////////////////////////////////////////////////
	// inaccurate parameters for stage0
	int Nia_s0i0 = 4;
	int Nia_s0i1 = 4;
	int Nia_s0m0 = 4;
	int Nia_s0m1 = 5;
	int Nia_s0o0 = 3;
	int Nia_s0m2 = Nia_s0m0;
	int Nia_s0m3 = Nia_s0m1;

	// inaccurate parameters for stage1
	int Nia_s1i0 = 3;
	int Nia_s1i1 = 3;
	int Nia_s1m0 = 4;
	int Nia_s1m1 = 5;
	int Nia_s1o0 = 3;
	int Nia_s1m2 = Nia_s1m0;
	int Nia_s1m3 = Nia_s1m1;

	// inaccurate parameters for stage2
	int Nia_s2i0 = 0;
	int Nia_s2i1 = 0;
	int Nia_s2m0 = 0;
	int Nia_s2m1 = 0;
	int Nia_s2m2 = Nia_s2m0;
	int Nia_s2m3 = Nia_s2m1;
	int Nia_s2o0 = 0;

	// inaccurate parameters for stage3
	int Nia_s3i0 = 0;
	int Nia_s3i1 = 0;
	int Nia_s3m0 = 0;
	int Nia_s3m1 = 0;
	int Nia_s3m2 = Nia_s3m0;
	int Nia_s3m3 = Nia_s3m1;
	int Nia_s3o0 = 0;
	//////////////////////////////////////////////////////////

	// inaccurate units for stage0
	loa s0i0 (Nt, Nia_s0i0, Nia_s0i0-1, 0, 1);
	loa s0i1 (Nt, Nia_s0i1, Nia_s0i1-1, 0, 1);
	bam s0m0 (Nt, Nia_s0m0, Nia_s0m0,  Nia_s0m0-1,  0, 1);
	bam s0m1 (Nt, Nia_s0m1, Nia_s0m1,  Nia_s0m1-1,  0, 1);
	bam s0m2 (Nt, Nia_s0m2, Nia_s0m2,  Nia_s0m2-1,  0, 1);
	bam s0m3 (Nt, Nia_s0m3, Nia_s0m3,  Nia_s0m3-1,  0, 1);
	loa s0o0 (Nt, Nia_s0o0, Nia_s0o0-1, 0, 1);

	// inaccurate units for stage1
	loa s1i0 (Nt, Nia_s1i0, Nia_s1i0-1, 0, 1);
	loa s1i1 (Nt, Nia_s1i1, Nia_s1i1-1, 0, 1);
	bam s1m0 (Nt, Nia_s1m0, Nia_s1m0,  Nia_s1m0-1,  0, 1);
	bam s1m1 (Nt, Nia_s1m1, Nia_s1m1,  Nia_s1m1-1,  0, 1);
	bam s1m2 (Nt, Nia_s1m2, Nia_s1m2,  Nia_s1m2-1,  0, 1);
	bam s1m3 (Nt, Nia_s1m3, Nia_s1m3,  Nia_s1m3-1,  0, 1);
	loa s1o0 (Nt, Nia_s1o0, Nia_s1o0-1, 0, 1);

	// inaccurate units for stage2
	loa s2i0 (Nt, Nia_s2i0, Nia_s2i0-1, 0, 1);
	loa s2i1 (Nt, Nia_s2i1, Nia_s2i1-1, 0, 1);
	bam s2m0 (Nt, Nia_s2m0, Nia_s2m0,  Nia_s2m0-1,  0, 1);
	bam s2m1 (Nt, Nia_s2m1, Nia_s2m1,  Nia_s2m1-1,  0, 1);
	bam s2m2 (Nt, Nia_s2m2, Nia_s2m2,  Nia_s2m2-1,  0, 1);
	bam s2m3 (Nt, Nia_s2m3, Nia_s2m3,  Nia_s2m3-1,  0, 1);
	loa s2o0 (Nt, Nia_s2o0, Nia_s2o0-1, 0, 1);

	// inaccurate units for stage1
	loa s3i0 (Nt, Nia_s3i0, Nia_s3i0-1, 0, 1);
	loa s3i1 (Nt, Nia_s3i1, Nia_s3i1-1, 0, 1);
	bam s3m0 (Nt, Nia_s3m0, Nia_s3m0,  Nia_s3m0-1,  0, 1);
	bam s3m1 (Nt, Nia_s3m1, Nia_s3m1,  Nia_s3m1-1,  0, 1);
	bam s3m2 (Nt, Nia_s3m2, Nia_s3m2,  Nia_s3m2-1,  0, 1);
	bam s3m3 (Nt, Nia_s3m3, Nia_s3m3,  Nia_s3m3-1,  0, 1);
	loa s3o0 (Nt, Nia_s3o0, Nia_s3o0-1, 0, 1);

	double snr_packet;
	double snr_sum = 0;
	double snr;

		// packet loop
	for (i=0;i<num_pac;i++ ) {

    	for (int j=0;j<N;j++) {
	    	 data_fix[j].r = x1_vec[i*N+j];
		     data_fix[j].i = x2_vec[i*N+j];
		     data_float[j].r = (double) x1_vec[i*N+j];
		     data_float[j].i = (double) x2_vec[i*N+j];
	    }

		vector<cmplx>::iterator its = data_fix.begin();

		int p0;
		int p1;
		int p2;
		int p3;

//		fft256_fixed(data_fix2, N, &bi[0], &bf[0]);

		////////////////////////////////////////////////////////////
		// Fixed point FFT
		////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////////
		// Stage0
		///////////////////////////////////////////////////////////////////////////////

		int Nstg = N;
		int stage_num = 0;

		N1=4;
		N2=Nstg/4;
  
		q_format x_r; 
		q_format x_i;

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

			data_fix2[s].r = x_r.val;
			data_fix2[s].i = x_i.val;

		}

		its = data_fix.begin();

		// Do 4 Point DFT // 
		for (n2=0; n2<N2; n2++)
		{

			if (mode == 1) {
				s0i0_vec.push_back((*(its+n2)).r);
				s0i1_vec.push_back((*(its+N2+n2)).r);
				s0i2_vec.push_back((*(its+n2)).r + (*(its+N2+n2)).r);
				s0i3_vec.push_back((*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r);
			}

			// Don't hurt the butterfly //
			bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
			bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
			bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
			bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
			bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
			bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
			bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
			bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

			if (mode == 1) {
				s0b0_vec.push_back(bfly[0].r);
				s0b1_vec.push_back(bfly[0].i);
			}

			// Twiddle multiply
			for (k1=0; k1<N1; k1++)
			{
				twiddle(W_float, Nstg, (double)k1*(double)n2);

				W = float2fix (W_float, bi_fft_twd0, bf_fft_twd0);

				if (mode == 1) {
					s0t0_vec.push_back(W.r);
					s0t1_vec.push_back(W.i);
				}

				p0 = (bfly[k1].r*W.r)>>bf_fft_stg0;
				p1 = (-bfly[k1].i*W.i)>>bf_fft_stg0;
				p2 = (bfly[k1].i*W.r)>>bf_fft_stg0;
				p3 = (bfly[k1].r*W.i)>>bf_fft_stg0;

				if (mode == 1) {
					s0c0_vec.push_back(p0);
					s0c1_vec.push_back(p1);
					s0c2_vec.push_back(p2);
					s0c3_vec.push_back(p3);
				}

				(*(its+n2+N2*k1)).r = p0 + p1;
				(*(its+n2+N2*k1)).i = p2 + p3;
			}
		} // for stage0

		if (mode == 2 || mode == 4) {

			its = data_fix2.begin();

			// Do 4 Point DFT // 
			for (n2=0; n2<N2; n2++)
			{

				// Don't hurt the butterfly //
				bfly[0].r = s0i1.calc(s0i0.calc((*(its+n2)).r, (*(its+N2+n2)).r), s0i0.calc((*(its+2*N2+n2)).r, (*(its+3*N2+n2)).r));
				bfly[0].i = s0i1.calc(s0i0.calc((*(its+n2)).i, (*(its+N2+n2)).i), s0i0.calc((*(its+2*N2+n2)).i, (*(its+3*N2+n2)).i));
			
				bfly[1].r = s0i1.calc(s0i0.calc((*(its+n2)).r, (*(its+N2+n2)).i), s0i0.calc(-(*(its+2*N2+n2)).r, -(*(its+3*N2+n2)).i));
				bfly[1].i = s0i1.calc(s0i0.calc((*(its+n2)).i, -(*(its+N2+n2)).r), s0i0.calc(-(*(its+2*N2+n2)).i, (*(its+3*N2+n2)).r));
	
				bfly[2].r = s0i1.calc(s0i0.calc((*(its+n2)).r, -(*(its+N2+n2)).r), s0i0.calc((*(its+2*N2+n2)).r, -(*(its+3*N2+n2)).r));
				bfly[2].i = s0i1.calc(s0i0.calc((*(its+n2)).i, -(*(its+N2+n2)).i), s0i0.calc((*(its+2*N2+n2)).i, -(*(its+3*N2+n2)).i));
	
				bfly[3].r = s0i1.calc(s0i0.calc((*(its+n2)).r, -(*(its+N2+n2)).i), s0i0.calc(-(*(its+2*N2+n2)).r, (*(its+3*N2+n2)).i));
				bfly[3].i = s0i1.calc(s0i0.calc((*(its+n2)).i, (*(its+N2+n2)).r), s0i0.calc(-(*(its+2*N2+n2)).i, -(*(its+3*N2+n2)).r));

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{
					twiddle(W_float, Nstg, (double)k1*(double)n2);

					W = float2fix (W_float, bi_fft_twd0, bf_fft_twd0);
						
					p0 = (s0m0.calc(bfly[k1].r, W.r))>>bf_fft_stg0;
					p1 = (s0m1.calc(-bfly[k1].i, W.i))>>bf_fft_stg0;
					p2 = (s0m2.calc(bfly[k1].i, W.r))>>bf_fft_stg0;
					p3 = (s0m3.calc(bfly[k1].r, W.i))>>bf_fft_stg0;

					(*(its+n2+N2*k1)).r = s0o0.calc(p0, p1);
					(*(its+n2+N2*k1)).i = s0o0.calc(p2, p3);
				}
			} // for stage0

		} // end of mode == 2



		////////////////////////////////////////////////////////////////////////////////
		// Stage1
		////////////////////////////////////////////////////////////////////////////////

		Nstg = N/4;
		stage_num = 1;
		N2=Nstg/4;
		int R2 = 4;

		// for accurate reference: data_fix
		for (int s=0;s<N;s++) {

			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_inp+3;
			x_r.bf = bf_fft_inp;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_inp+3;
			x_i.bf = bf_fft_inp;

			x_r = rnd_clp (x_r, bi_fft_stg0, bf_fft_stg0); 
			x_i = rnd_clp (x_i, bi_fft_stg0, bf_fft_stg0);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;
		}

		// for inaccurate computation: data_fix2
		for (int s=0;s<N;s++) {

			x_r.val = data_fix2[s].r;
			x_r.bi = bi_fft_inp+3;
			x_r.bf = bf_fft_inp;

			x_i.val = data_fix2[s].i;
			x_i.bi = bi_fft_inp+3;
			x_i.bf = bf_fft_inp;

			x_r = rnd_clp (x_r, bi_fft_stg0, bf_fft_stg0); 
			x_i = rnd_clp (x_i, bi_fft_stg0, bf_fft_stg0);

			data_fix2[s].r = x_r.val;
			data_fix2[s].i = x_i.val;
		}


		its = data_fix.begin();

		for (int k2=0; k2<R2; k2++)
		{

			// Do 4 Point DFT // 
			for (n2=0; n2<N2; n2++)
			{
				if (mode == 3) {
					s1i0_vec.push_back((*(its+n2)).r);
					s1i1_vec.push_back((*(its+N2+n2)).r);
					s1i2_vec.push_back((*(its+n2)).r + (*(its+N2+n2)).r);
					s1i3_vec.push_back((*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r);
				}

				// Don't hurt the butterfly //
				bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
				bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
				bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
				bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
				bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
				bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
				bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
				bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

				if (mode == 3) {
					s1b0_vec.push_back(bfly[0].r);
					s1b1_vec.push_back(bfly[0].i);
				}

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{
					twiddle(W_float, Nstg, (double)k1*(double)n2);
					W = float2fix (W_float, bi_fft_twd1, bf_fft_twd1);

					if (mode == 3) {
						s1t0_vec.push_back(W.r);
						s1t1_vec.push_back(W.i);
					}

					p0 = (bfly[k1].r*W.r)>>bf_fft_stg1;
					p1 = (-bfly[k1].i*W.i)>>bf_fft_stg1;
					p2 = (bfly[k1].i*W.r)>>bf_fft_stg1;
					p3 = (bfly[k1].r*W.i)>>bf_fft_stg1;

					if (mode == 3) {
						s1c0_vec.push_back(p0);
						s1c1_vec.push_back(p1);
						s1c2_vec.push_back(p2);
						s1c3_vec.push_back(p3);
					}

					(*(its+n2+N2*k1)).r = p0 + p1;
					(*(its+n2+N2*k1)).i = p2 + p3;

				}
			}

			its = its + Nstg;

		}

		if (mode == 4) {

			its = data_fix2.begin();

			for (int k2=0; k2<R2; k2++)
			{

				// Do 4 Point DFT // 
				for (n2=0; n2<N2; n2++)
				{

					// Don't hurt the butterfly //
					bfly[0].r = s1i1.calc(s1i0.calc((*(its+n2)).r, (*(its+N2+n2)).r), s1i0.calc((*(its+2*N2+n2)).r, (*(its+3*N2+n2)).r));
					bfly[0].i = s1i1.calc(s1i0.calc((*(its+n2)).i, (*(its+N2+n2)).i), s1i0.calc((*(its+2*N2+n2)).i, (*(its+3*N2+n2)).i));
				
					bfly[1].r = s1i1.calc(s1i0.calc((*(its+n2)).r, (*(its+N2+n2)).i), s1i0.calc(-(*(its+2*N2+n2)).r, -(*(its+3*N2+n2)).i));
					bfly[1].i = s1i1.calc(s1i0.calc((*(its+n2)).i, -(*(its+N2+n2)).r), s1i0.calc(-(*(its+2*N2+n2)).i, (*(its+3*N2+n2)).r));
	
					bfly[2].r = s1i1.calc(s1i0.calc((*(its+n2)).r, -(*(its+N2+n2)).r), s1i0.calc((*(its+2*N2+n2)).r, -(*(its+3*N2+n2)).r));
					bfly[2].i = s1i1.calc(s1i0.calc((*(its+n2)).i, -(*(its+N2+n2)).i), s1i0.calc((*(its+2*N2+n2)).i, -(*(its+3*N2+n2)).i));
	
					bfly[3].r = s1i1.calc(s1i0.calc((*(its+n2)).r, -(*(its+N2+n2)).i), s1i0.calc(-(*(its+2*N2+n2)).r, (*(its+3*N2+n2)).i));
					bfly[3].i = s1i1.calc(s1i0.calc((*(its+n2)).i, (*(its+N2+n2)).r), s1i0.calc(-(*(its+2*N2+n2)).i, -(*(its+3*N2+n2)).r));


					// Twiddle multiply
					for (k1=0; k1<N1; k1++)
					{
						twiddle(W_float, Nstg, (double)k1*(double)n2);
						W = float2fix (W_float, bi_fft_twd1, bf_fft_twd1);

						p0 = (s1m0.calc(bfly[k1].r, W.r))>>bf_fft_stg1;
						p1 = (s1m1.calc(-bfly[k1].i, W.i))>>bf_fft_stg1;
						p2 = (s1m2.calc(bfly[k1].i, W.r))>>bf_fft_stg1;
						p3 = (s1m3.calc(bfly[k1].r, W.i))>>bf_fft_stg1;

						(*(its+n2+N2*k1)).r = s1o0.calc(p0, p1);
						(*(its+n2+N2*k1)).i = s1o0.calc(p2, p3);

					}
				}

				its = its + Nstg;

			}
		} // mode == 4



		its = data_fix.begin();

/*
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

				s2i0_vec.push_back((*(its+n2)).r);
				s2i1_vec.push_back((*(its+N2+n2)).r);
				s2i2_vec.push_back((*(its+n2)).r + (*(its+N2+n2)).r);
				s2i3_vec.push_back((*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r);

				// Don't hurt the butterfly //
				bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
				bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
				bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
				bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
				bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
				bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
				bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
				bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

				s2b0_vec.push_back(bfly[0].r);
				s2b1_vec.push_back(bfly[0].i);

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{

					twiddle(W_float, Nstg, (double)k1*(double)n2);
					W = float2fix (W_float, bi_fft_twd2, bf_fft_twd2);

					s2t0_vec.push_back(W.r);
					s2t1_vec.push_back(W.i);
					s2c0_vec.push_back(bfly[k1].i*W.r);
					s2c1_vec.push_back(bfly[k1].r*W.i);

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

				s3i0_vec.push_back((*(its+n2)).r);
				s3i1_vec.push_back((*(its+N2+n2)).r);
				s3i2_vec.push_back((*(its+n2)).r + (*(its+N2+n2)).r);
				s3i3_vec.push_back((*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r);

				// Don't hurt the butterfly //
				bfly[0].r = (*(its+n2)).r + (*(its+N2+n2)).r + (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).r;
				bfly[0].i = (*(its+n2)).i + (*(its+N2+n2)).i + (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).i;
  
				bfly[1].r = (*(its+n2)).r + (*(its+N2+n2)).i - (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).i;
				bfly[1].i = (*(its+n2)).i - (*(its+N2+n2)).r - (*(its+2*N2+n2)).i + (*(its+3*N2+n2)).r;
  
				bfly[2].r = (*(its+n2)).r - (*(its+N2+n2)).r + (*(its+2*N2+n2)).r - (*(its+3*N2+n2)).r;
				bfly[2].i = (*(its+n2)).i - (*(its+N2+n2)).i + (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).i;
  
				bfly[3].r = (*(its+n2)).r - (*(its+N2+n2)).i - (*(its+2*N2+n2)).r + (*(its+3*N2+n2)).i;
				bfly[3].i = (*(its+n2)).i + (*(its+N2+n2)).r - (*(its+2*N2+n2)).i - (*(its+3*N2+n2)).r;

				s3b0_vec.push_back(bfly[0].r);
				s3b1_vec.push_back(bfly[0].i);

				// Twiddle multiply
				for (k1=0; k1<N1; k1++)
				{
					twiddle(W_float, Nstg, (double)k1*(double)n2);
					W = float2fix (W_float, bi_fft_twd3, bf_fft_twd3);

					s3t0_vec.push_back(W.r);
					s3t1_vec.push_back(W.i);
					s3c0_vec.push_back(bfly[k1].i*W.r);
					s3c1_vec.push_back(bfly[k1].r*W.i);

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
		*/
		////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////

//		fft256_radix4 (data_float, N);
		
		/*
		for (int s=0;s<N;s++) {
			cout << "Fixed real(" << s << "): " << data_fix[s].r << ", Floating real: " << data_fix2[s].r << endl;
			cout << "Fixed imag(" << s << "): " << data_fix[s].i << ", Floating imag: " << data_fix2[s].i << endl;
		}
		*/
		
		// error measure here

		cout << "Packet: " << i << endl;

		// packet SNR calculation
		double sig_pwr = 0;
		double err_pwr = 0;
		
		for (int s=0;s<N;s++) {
			sig_pwr += data_fix[s].r*data_fix[s].r+data_fix[s].i*data_fix[s].i;
			err_pwr += (data_fix[s].r-data_fix2[s].r)*(data_fix[s].r-data_fix2[s].r)+(data_fix[s].i-data_fix2[s].i)*(data_fix[s].i-data_fix2[s].i);

//			cout << "sglee acc real: " << data_fix[s].r << ", inacc real: " << data_fix2[s].r << endl;
//			cout << "sglee acc imag: " << data_fix[s].i << ", inacc imag: " << data_fix2[s].i << endl;

		}


		snr_packet = sig_pwr/err_pwr;
		snr_sum += snr_packet;

	} // packet loop

	snr = snr_sum / num_pac;
	cout << "SNR = " << 10*log10(snr) << endl;

	if (mode == 2 || mode == 4) {
		// SNR estimation
		double errs0t0[8] = {0, 0.43, 0.96, 1.97, 3.95, 7.95, 15.86, 27.62};
		double errs0t1[8] = {0, 0.43, 0.97, 1.98, 4.00, 8.00, 15.98, 31.79};
		double errs0t2[8] = {0, 2478.9, 6433.09, 14215.6, 29542.4, 58743.3, 112241, 192195};
		double errs0t3[8] = {0, 1787.75, 4584.07, 10068.3, 20818.7, 41747.9, 80091.2, 136587};
		double errs0t4[8] = {0, 2475.48, 6430.8, 14199, 29506.4, 59002.5, 112964, 191515};
		double errs0t5[8] = {0, 1789.97, 4585.58, 10079.3, 20902.2, 41573.5, 79602.9, 137081};
		double errs0t6[8] = {0, 0.39, 0.86, 1.77, 3.56, 7.14, 14.29, 20.23};

		double errs1t0[8] = {0, 0.43, 0.97, 1.98, 3.98, 8.03, 16.01, 28.05};
		double errs1t1[8] = {0, 0.43, 0.97, 1.99, 4.00, 7.98, 16.03, 32.06};
		double errs1t2[8] = {0, 2476.5, 6447.11, 14229.3, 29515.8, 58703.3, 112897, 192655};
		double errs1t3[8] = {0, 1750.22, 4513.31, 9880.23, 20553.4, 40760.9, 77881.4, 134372};
		double errs1t4[8] = {0, 2474.09, 6462.05, 14213.2, 29630.8, 58740.6, 111998, 192210};
		double errs1t5[8] = {0, 1751.77, 4503.63, 9890.8, 20478, 40737, 78481.9, 134656};
		double errs1t6[8] = {0, 0.38, 0.84, 1.72, 3.47, 6.97, 13.9, 19.7};

		double exp = 13;

		double evars0t0 = errs0t0[(size_t)Nia_s0i0]*errs0t0[(size_t)Nia_s0i0];
		double evars0t1 = errs0t1[(size_t)Nia_s0i1]*errs0t1[(size_t)Nia_s0i1];
		double evars0t2 = (errs0t2[(size_t)Nia_s0m0]/pow(2,exp))*(errs0t2[(size_t)Nia_s0m0]/pow(2,exp));
		double evars0t3 = (errs0t3[(size_t)Nia_s0m1]/pow(2,exp))*(errs0t3[(size_t)Nia_s0m1]/pow(2,exp));
		double evars0t4 = (errs0t4[(size_t)Nia_s0m2]/pow(2,exp))*(errs0t4[(size_t)Nia_s0m2]/pow(2,exp));
		double evars0t5 = (errs0t5[(size_t)Nia_s0m3]/pow(2,exp))*(errs0t5[(size_t)Nia_s0m3]/pow(2,exp));
		double evars0t6 = errs0t6[(size_t)Nia_s0o0]*errs0t6[(size_t)Nia_s0o0];

		double evars1t0 = errs1t0[(size_t)Nia_s1i0]*errs1t0[(size_t)Nia_s1i0];
		double evars1t1 = errs1t1[(size_t)Nia_s1i1]*errs1t1[(size_t)Nia_s1i1];
		double evars1t2 = (errs1t2[(size_t)Nia_s1m0]/pow(2,exp))*(errs1t2[(size_t)Nia_s1m0]/pow(2,exp));
		double evars1t3 = (errs1t3[(size_t)Nia_s1m1]/pow(2,exp))*(errs1t3[(size_t)Nia_s1m1]/pow(2,exp));
		double evars1t4 = (errs1t4[(size_t)Nia_s1m2]/pow(2,exp))*(errs1t4[(size_t)Nia_s1m2]/pow(2,exp));
		double evars1t5 = (errs1t5[(size_t)Nia_s1m3]/pow(2,exp))*(errs1t5[(size_t)Nia_s1m3]/pow(2,exp));
		double evars1t6 = errs1t6[(size_t)Nia_s1o0]*errs1t6[(size_t)Nia_s1o0];

		double evars0r = 0.5*evars0t0+0.25*evars0t1+evars0t2+evars0t3+evars0t6;
		double evars0i = 0.5*evars0t0+0.25*evars0t1+evars0t4+evars0t5+evars0t6;

		double evars1r = 0.5*evars1t0+0.25*evars1t1+evars1t2+evars1t3+evars1t6;
		double evars1i = 0.5*evars1t0+0.25*evars1t1+evars1t4+evars1t5+evars1t6;

		double snr_est = (var_x1+var_x2)/(evars0r+evars0i+evars1r+evars1i);

		cout << "SNR estimation = " << 10*log10(snr_est) << endl;
	}

	if (mode == 1) {

		cout << "stage0 input correlation --------------------------------------" << endl;
		io_stat in_corr_s0i(Nt, 1, s0i0_vec, s0i1_vec, corr_th, corr_th);
		in_corr_s0i.print_io_stat();
		cout << "stage0 butterfly intermdeidate --------------------------------------" << endl;
		io_stat in_corr_s0b(Nt+1, 1, s0i2_vec, s0i3_vec, corr_th, corr_th);
		in_corr_s0b.print_io_stat();

		cout << "stage0 twiddle multiplication0 --------------------------------------" << endl;
		io_stat in_corr_s0t0(Nt+2, 2, s0b0_vec, s0t0_vec, corr_th, corr_th);
		in_corr_s0t0.print_io_stat();

		cout << "stage0 twiddle multiplication1 --------------------------------------" << endl;
		io_stat in_corr_s0t1(Nt+2, 2, s0b1_vec, s0t1_vec, corr_th, corr_th);
		in_corr_s0t1.print_io_stat();
		cout << "stage0 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s0t2(Nt+2, 2, s0b1_vec, s0t0_vec, corr_th, corr_th);
		in_corr_s0t2.print_io_stat();
		cout << "stage0 twiddle multiplication3 --------------------------------------" << endl;
		io_stat in_corr_s0t3(Nt+2, 2, s0b0_vec, s0t1_vec, corr_th, corr_th);
		in_corr_s0t3.print_io_stat();
		cout << "stage0 output adder0 --------------------------------------" << endl;
		io_stat in_corr_s0o0(2*Nt, 1, s0c0_vec, s0c1_vec, corr_th, corr_th);
		in_corr_s0o0.print_io_stat();
		cout << "stage0 output adder1 --------------------------------------" << endl;
		io_stat in_corr_s0o1(2*Nt, 1, s0c2_vec, s0c3_vec, corr_th, corr_th);
		in_corr_s0o1.print_io_stat();

	///////////////////////////////////////////////////////////////////////////////
	// Analysis
	///////////////////////////////////////////////////////////////////////////////

	for (int Nia_add_s0i0 = 1; Nia_add_s0i0 < 8; Nia_add_s0i0++) {
		loa add_s0i0 (Nt, Nia_add_s0i0, Nia_add_s0i0-1, 0, 1);
		error_model err_add_s0i0;
		err_add_s0i0.error_est(add_s0i0.err_tbl, in_corr_s0i.io, Nia_add_s0i0-1, 0, 1);
		err_add_s0i0.print_err_stat(err_add_s0i0.est_err, 0);
	}

	for (int Nia_add_s0i1 = 1; Nia_add_s0i1 < 8; Nia_add_s0i1++) {
		loa add_s0i1 (Nt, Nia_add_s0i1, Nia_add_s0i1-1, 0, 1);
		error_model err_add_s0i1;
		err_add_s0i1.error_est(add_s0i1.err_tbl, in_corr_s0b.io, Nia_add_s0i1-1, 0, 1);
		err_add_s0i1.print_err_stat(err_add_s0i1.est_err, 0);
	}

	for (int Nia_mul_s0t0 = 1; Nia_mul_s0t0 < 8; Nia_mul_s0t0++) {
		bam mul_s0t0 (2*Nt, Nia_mul_s0t0, Nia_mul_s0t0, Nia_mul_s0t0-1, 0, 1);
		error_model err_mul_s0t0;
		err_mul_s0t0.error_est(mul_s0t0.err_tbl, in_corr_s0t0.io, Nia_mul_s0t0-1, 0, 1);
		err_mul_s0t0.print_err_stat(err_mul_s0t0.est_err, 0);
	}

	for (int Nia_mul_s0t1 = 1; Nia_mul_s0t1 < 8; Nia_mul_s0t1++) {
		bam mul_s0t1 (2*Nt, Nia_mul_s0t1, Nia_mul_s0t1, Nia_mul_s0t1-1, 0, 1);
		error_model err_mul_s0t1;
		err_mul_s0t1.error_est(mul_s0t1.err_tbl, in_corr_s0t1.io, Nia_mul_s0t1-1, 0, 1);
		err_mul_s0t1.print_err_stat(err_mul_s0t1.est_err, 0);
	}

	for (int Nia_mul_s0t2 = 1; Nia_mul_s0t2 < 8; Nia_mul_s0t2++) {
		bam mul_s0t2 (2*Nt, Nia_mul_s0t2, Nia_mul_s0t2, Nia_mul_s0t2-1, 0, 1);
		error_model err_mul_s0t2;
		err_mul_s0t2.error_est(mul_s0t2.err_tbl, in_corr_s0t2.io, Nia_mul_s0t2-1, 0, 1);
		err_mul_s0t2.print_err_stat(err_mul_s0t2.est_err, 0);
	}

	for (int Nia_mul_s0t3 = 1; Nia_mul_s0t3 < 8; Nia_mul_s0t3++) {
		bam mul_s0t3 (2*Nt, Nia_mul_s0t3, Nia_mul_s0t3, Nia_mul_s0t3-1, 0, 1);
		error_model err_mul_s0t3;
		err_mul_s0t3.error_est(mul_s0t3.err_tbl, in_corr_s0t3.io, Nia_mul_s0t3-1, 0, 1);
		err_mul_s0t3.print_err_stat(err_mul_s0t3.est_err, 0);
	}

	for (int Nia_add_s0o0 = 1; Nia_add_s0o0 < 8; Nia_add_s0o0++) {
		loa add_s0o0 (Nt, Nia_add_s0o0, Nia_add_s0o0-1, 0, 1);
		error_model err_add_s0o0;
		err_add_s0o0.error_est(add_s0o0.err_tbl, in_corr_s0o0.io, Nia_add_s0o0-1, 0, 1);
		err_add_s0o0.print_err_stat(err_add_s0o0.est_err, 0);
	}

	for (int Nia_add_s0o1 = 1; Nia_add_s0o1 < 8; Nia_add_s0o1++) {
		loa add_s0o1 (Nt, Nia_add_s0o1, Nia_add_s0o1-1, 0, 1);
		error_model err_add_s0o1;
		err_add_s0o1.error_est(add_s0o1.err_tbl, in_corr_s0o1.io, Nia_add_s0o1-1, 0, 1);
		err_add_s0o1.print_err_stat(err_add_s0o1.est_err, 0);
	}

	} // for mode == 1


	if (mode == 3) {

		cout << "stage1 input correlation --------------------------------------" << endl;
		io_stat in_corr_s1i(Nt, 1, s1i0_vec, s1i1_vec, corr_th, corr_th);
		in_corr_s1i.print_io_stat();

		cout << "stage1 butterfly intermdeidate --------------------------------------" << endl;
		io_stat in_corr_s1b(Nt+1, 1, s1i2_vec, s1i3_vec, corr_th, corr_th);
		in_corr_s1b.print_io_stat();
		cout << "stage1 twiddle multiplication1 --------------------------------------" << endl;
		io_stat in_corr_s1t0(Nt+2, 2, s1b0_vec, s1t0_vec, corr_th, corr_th);
		in_corr_s1t0.print_io_stat();
		cout << "stage1 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s1t1(Nt+2, 2, s1b1_vec, s1t1_vec, corr_th, corr_th);
		in_corr_s1t1.print_io_stat();
		cout << "stage1 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s1t2(Nt+2, 2, s1b1_vec, s1t0_vec, corr_th, corr_th);
		in_corr_s1t2.print_io_stat();
		cout << "stage1 twiddle multiplication3 --------------------------------------" << endl;
		io_stat in_corr_s1t3(Nt+2, 2, s1b0_vec, s1t1_vec, corr_th, corr_th);
		in_corr_s1t3.print_io_stat();
		cout << "stage1 output adder0 --------------------------------------" << endl;
		io_stat in_corr_s1o0(2*Nt, 1, s1c0_vec, s1c1_vec, corr_th, corr_th);
		in_corr_s1o0.print_io_stat();
		cout << "stage1 output adder1 --------------------------------------" << endl;
		io_stat in_corr_s1o1(2*Nt, 1, s1c2_vec, s1c3_vec, corr_th, corr_th);
		in_corr_s1o1.print_io_stat();
		
	///////////////////////////////////////////////////////////////////////////////
	// Analysis
	///////////////////////////////////////////////////////////////////////////////

	for (int Nia_add_s1i0 = 1; Nia_add_s1i0 < 8; Nia_add_s1i0++) {
		loa add_s1i0 (Nt, Nia_add_s1i0, Nia_add_s1i0-1, 0, 1);
		error_model err_add_s1i0;
		err_add_s1i0.error_est(add_s1i0.err_tbl, in_corr_s1i.io, Nia_add_s1i0-1, 0, 1);
		err_add_s1i0.print_err_stat(err_add_s1i0.est_err, 0);
		cout << "sglee: " << Nia_add_s1i0 << endl;
	}

	for (int Nia_add_s1i1 = 1; Nia_add_s1i1 < 8; Nia_add_s1i1++) {
		loa add_s1i1 (Nt, Nia_add_s1i1, Nia_add_s1i1-1, 0, 1);
		error_model err_add_s1i1;
		err_add_s1i1.error_est(add_s1i1.err_tbl, in_corr_s1b.io, Nia_add_s1i1-1, 0, 1);
		err_add_s1i1.print_err_stat(err_add_s1i1.est_err, 0);
	}

	for (int Nia_mul_s1t0 = 1; Nia_mul_s1t0 < 8; Nia_mul_s1t0++) {
		bam mul_s1t0 (2*Nt, Nia_mul_s1t0, Nia_mul_s1t0, Nia_mul_s1t0-1, 0, 1);
		error_model err_mul_s1t0;
		err_mul_s1t0.error_est(mul_s1t0.err_tbl, in_corr_s1t0.io, Nia_mul_s1t0-1, 0, 1);
		err_mul_s1t0.print_err_stat(err_mul_s1t0.est_err, 0);
	}

	for (int Nia_mul_s1t1 = 1; Nia_mul_s1t1 < 8; Nia_mul_s1t1++) {
		bam mul_s1t1 (2*Nt, Nia_mul_s1t1, Nia_mul_s1t1, Nia_mul_s1t1-1, 0, 1);
		error_model err_mul_s1t1;
		err_mul_s1t1.error_est(mul_s1t1.err_tbl, in_corr_s1t1.io, Nia_mul_s1t1-1, 0, 1);
		err_mul_s1t1.print_err_stat(err_mul_s1t1.est_err, 0);
	}

	for (int Nia_mul_s1t2 = 1; Nia_mul_s1t2 < 8; Nia_mul_s1t2++) {
		bam mul_s1t2 (2*Nt, Nia_mul_s1t2, Nia_mul_s1t2, Nia_mul_s1t2-1, 0, 1);
		error_model err_mul_s1t2;
		err_mul_s1t2.error_est(mul_s1t2.err_tbl, in_corr_s1t2.io, Nia_mul_s1t2-1, 0, 1);
		err_mul_s1t2.print_err_stat(err_mul_s1t2.est_err, 0);
	}

	for (int Nia_mul_s1t3 = 1; Nia_mul_s1t3 < 8; Nia_mul_s1t3++) {
		bam mul_s1t3 (2*Nt, Nia_mul_s1t3, Nia_mul_s1t3, Nia_mul_s1t3-1, 0, 1);
		error_model err_mul_s1t3;
		err_mul_s1t3.error_est(mul_s1t3.err_tbl, in_corr_s1t3.io, Nia_mul_s1t3-1, 0, 1);
		err_mul_s1t3.print_err_stat(err_mul_s1t3.est_err, 0);
	}

	for (int Nia_add_s1o0 = 1; Nia_add_s1o0 < 8; Nia_add_s1o0++) {
		loa add_s1o0 (Nt, Nia_add_s1o0, Nia_add_s1o0-1, 0, 1);
		error_model err_add_s1o0;
		err_add_s1o0.error_est(add_s1o0.err_tbl, in_corr_s1o0.io, Nia_add_s1o0-1, 0, 1);
		err_add_s1o0.print_err_stat(err_add_s1o0.est_err, 0);
	}

	for (int Nia_add_s1o1 = 1; Nia_add_s1o1 < 8; Nia_add_s1o1++) {
		loa add_s1o1 (Nt, Nia_add_s1o1, Nia_add_s1o1-1, 0, 1);
		error_model err_add_s1o1;
		err_add_s1o1.error_est(add_s1o1.err_tbl, in_corr_s1o1.io, Nia_add_s1o1-1, 0, 1);
		err_add_s1o1.print_err_stat(err_add_s1o1.est_err, 0);
	}

	} // for mode == 3

	cout << " sglee " << endl;

	getchar();
	return 1;

}
