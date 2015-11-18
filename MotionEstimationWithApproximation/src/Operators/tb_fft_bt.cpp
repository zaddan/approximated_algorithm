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
int main_fft_bt()
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
	mode = 2; // '1' for unit level analysis, '2' for evaluation

	ACVEC x1_vec(N*num_pac);
	ACVEC x2_vec(N*num_pac);

	float mean_x1 = 0;
	float var_x1 = 1000;
	float mean_x2 = 0;
	float var_x2 = 1000;

	// input generation
	gen_success = operand_gen(Nt, mean_x1, var_x1, mean_x2, var_x2, x1_vec, x2_vec, 1);

	int bi[5] = { 3, 3, 3, 3, 3 };
	int bf[5] = { 13, 13, 13, 13, 13 };

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
	int Nia_s0i0 = 3;
	int Nia_s0i1 = 2;
	int Nia_s0m0 = 3;
	int Nia_s0m1 = 4;
	int Nia_s0m2 = 3;
	int Nia_s0m3 = 3;
	int Nia_s0o0 = 2;
	//////////////////////////////////////////////////////////

	loa s0i0(Nt, Nia_s0i0, Nia_s0i0 - 1, 0, 1);
	loa s0i1(Nt, Nia_s0i1, Nia_s0i1 - 1, 0, 1);
	bam s0m0(Nt, Nia_s0m0, Nia_s0m0, Nia_s0m0 - 1, 0, 1);
	bam s0m1(Nt, Nia_s0m1, Nia_s0m1, Nia_s0m1 - 1, 0, 1);
	bam s0m2(Nt, Nia_s0m2, Nia_s0m2, Nia_s0m2 - 1, 0, 1);
	bam s0m3(Nt, Nia_s0m3, Nia_s0m3, Nia_s0m3 - 1, 0, 1);
	loa s0o0(Nt, Nia_s0o0, Nia_s0o0 - 1, 0, 1);

	double snr_packet;
	double snr_sum = 0;
	double snr;

	// packet loop
	for (i = 0; i<num_pac; i++) {

		for (int j = 0; j<N; j++) {
			data_fix[j].r = x1_vec[i*N + j];
			data_fix[j].i = x2_vec[i*N + j];
			data_float[j].r = (double)x1_vec[i*N + j];
			data_float[j].i = (double)x2_vec[i*N + j];
		}

		//		fft256_fixed(data_fix2, N, &bi[0], &bf[0]);

		////////////////////////////////////////////////////////////
		// Fixed point FFT
		////////////////////////////////////////////////////////////

		///////////////////////////////////////
		// Stage0
		///////////////////////////////////////

		int Nstg = N;
		int stage_num = 0;

		N1 = 4;
		N2 = Nstg / 4;

		q_format x_r;
		q_format x_i;

		for (int s = 0; s<N; s++) {
			x_r.val = data_fix[s].r;
			x_r.bi = bi_fft_inp;
			x_r.bf = bf_fft_inp;

			x_i.val = data_fix[s].i;
			x_i.bi = bi_fft_inp;
			x_i.bf = bf_fft_inp;

			x_r = rnd_clp(x_r, bi_fft_inp, bf_fft_inp);
			x_i = rnd_clp(x_i, bi_fft_inp, bf_fft_inp);

			data_fix[s].r = x_r.val;
			data_fix[s].i = x_i.val;

			data_fix2[s].r = x_r.val;
			data_fix2[s].i = x_i.val;

		}

		vector<cmplx>::iterator its = data_fix.begin();

		int p0;
		int p1;
		int p2;
		int p3;

		// Do 4 Point DFT // 
		for (n2 = 0; n2<N2; n2++)
		{

			if (mode == 1) {
				s0i0_vec.push_back((*(its + n2)).r);
				s0i1_vec.push_back((*(its + N2 + n2)).r);
				s0i2_vec.push_back((*(its + n2)).r + (*(its + N2 + n2)).r);
				s0i3_vec.push_back((*(its + 2 * N2 + n2)).r + (*(its + 3 * N2 + n2)).r);
			}

			// Don't hurt the butterfly //
			bfly[0].r = (*(its + n2)).r + (*(its + N2 + n2)).r + (*(its + 2 * N2 + n2)).r + (*(its + 3 * N2 + n2)).r;
			bfly[0].i = (*(its + n2)).i + (*(its + N2 + n2)).i + (*(its + 2 * N2 + n2)).i + (*(its + 3 * N2 + n2)).i;

			bfly[1].r = (*(its + n2)).r + (*(its + N2 + n2)).i - (*(its + 2 * N2 + n2)).r - (*(its + 3 * N2 + n2)).i;
			bfly[1].i = (*(its + n2)).i - (*(its + N2 + n2)).r - (*(its + 2 * N2 + n2)).i + (*(its + 3 * N2 + n2)).r;

			bfly[2].r = (*(its + n2)).r - (*(its + N2 + n2)).r + (*(its + 2 * N2 + n2)).r - (*(its + 3 * N2 + n2)).r;
			bfly[2].i = (*(its + n2)).i - (*(its + N2 + n2)).i + (*(its + 2 * N2 + n2)).i - (*(its + 3 * N2 + n2)).i;

			bfly[3].r = (*(its + n2)).r - (*(its + N2 + n2)).i - (*(its + 2 * N2 + n2)).r + (*(its + 3 * N2 + n2)).i;
			bfly[3].i = (*(its + n2)).i + (*(its + N2 + n2)).r - (*(its + 2 * N2 + n2)).i - (*(its + 3 * N2 + n2)).r;

			if (mode == 1) {
				s0b0_vec.push_back(bfly[0].r);
				s0b1_vec.push_back(bfly[0].i);
			}

			// Twiddle multiply
			for (k1 = 0; k1<N1; k1++)
			{
				twiddle(W_float, Nstg, (double)k1*(double)n2);

				W = float2fix(W_float, bi_fft_twd0, bf_fft_twd0);

				if (mode == 1) {
					s0t0_vec.push_back(W.r);
					s0t1_vec.push_back(W.i);
				}

				p0 = (bfly[k1].r*W.r) >> bf_fft_stg0;
				p1 = (-bfly[k1].i*W.i) >> bf_fft_stg0;
				p2 = (bfly[k1].i*W.r) >> bf_fft_stg0;
				p3 = (bfly[k1].r*W.i) >> bf_fft_stg0;

				if (mode == 1) {
					s0c0_vec.push_back(p0);
					s0c1_vec.push_back(p1);
					s0c2_vec.push_back(p2);
					s0c3_vec.push_back(p3);
				}

				(*(its + n2 + N2*k1)).r = p0 + p1;
				(*(its + n2 + N2*k1)).i = p2 + p3;
			}
		} // for stage0

		its = data_fix.begin();


		if (mode == 2) {

			its = data_fix2.begin();

			// Do 4 Point DFT // 
			for (n2 = 0; n2<N2; n2++)
			{

				// Don't hurt the butterfly //
				bfly[0].r = s0i1.calc(s0i0.calc((*(its + n2)).r, (*(its + N2 + n2)).r), s0i0.calc((*(its + 2 * N2 + n2)).r, (*(its + 3 * N2 + n2)).r));
				bfly[0].i = s0i1.calc(s0i0.calc((*(its + n2)).i, (*(its + N2 + n2)).i), s0i0.calc((*(its + 2 * N2 + n2)).i, (*(its + 3 * N2 + n2)).i));

				bfly[1].r = s0i1.calc(s0i0.calc((*(its + n2)).r, (*(its + N2 + n2)).i), s0i0.calc(-(*(its + 2 * N2 + n2)).r, -(*(its + 3 * N2 + n2)).i));
				bfly[1].i = s0i1.calc(s0i0.calc((*(its + n2)).i, -(*(its + N2 + n2)).r), s0i0.calc(-(*(its + 2 * N2 + n2)).i, (*(its + 3 * N2 + n2)).r));

				bfly[2].r = s0i1.calc(s0i0.calc((*(its + n2)).r, -(*(its + N2 + n2)).r), s0i0.calc((*(its + 2 * N2 + n2)).r, -(*(its + 3 * N2 + n2)).r));
				bfly[2].i = s0i1.calc(s0i0.calc((*(its + n2)).i, -(*(its + N2 + n2)).i), s0i0.calc((*(its + 2 * N2 + n2)).i, -(*(its + 3 * N2 + n2)).i));

				bfly[3].r = s0i1.calc(s0i0.calc((*(its + n2)).r, -(*(its + N2 + n2)).i), s0i0.calc(-(*(its + 2 * N2 + n2)).r, (*(its + 3 * N2 + n2)).i));
				bfly[3].i = s0i1.calc(s0i0.calc((*(its + n2)).i, (*(its + N2 + n2)).r), s0i0.calc(-(*(its + 2 * N2 + n2)).i, -(*(its + 3 * N2 + n2)).r));

				// Twiddle multiply
				for (k1 = 0; k1<N1; k1++)
				{
					twiddle(W_float, Nstg, (double)k1*(double)n2);

					W = float2fix(W_float, bi_fft_twd0, bf_fft_twd0);

					p0 = (s0m0.calc(bfly[k1].r, W.r)) >> bf_fft_stg0;
					p1 = (s0m1.calc(-bfly[k1].i, W.i)) >> bf_fft_stg0;
					p2 = (s0m2.calc(bfly[k1].i, W.r)) >> bf_fft_stg0;
					p3 = (s0m3.calc(bfly[k1].r, W.i)) >> bf_fft_stg0;

					(*(its + n2 + N2*k1)).r = s0o0.calc(p0, p1);
					(*(its + n2 + N2*k1)).i = s0o0.calc(p2, p3);
				}
			} // for stage0

		} // end of mode == 2


		// error measure here

		cout << "Packet: " << i << endl;

		// packet SNR calculation
		double sig_pwr = 0;
		double err_pwr = 0;

		for (int s = 0; s<N; s++) {
			sig_pwr += data_fix[s].r*data_fix[s].r + data_fix[s].i*data_fix[s].i;
			err_pwr += (data_fix[s].r - data_fix2[s].r)*(data_fix[s].r - data_fix2[s].r) + (data_fix[s].i - data_fix2[s].i)*(data_fix[s].i - data_fix2[s].i);

			//			cout << "sglee acc real: " << data_fix[s].r << ", inacc real: " << data_fix2[s].r << endl;
			//			cout << "sglee acc imag: " << data_fix[s].i << ", inacc imag: " << data_fix2[s].i << endl;

		}


		snr_packet = sig_pwr / err_pwr;
		snr_sum += snr_packet;

	} // packet loop


	snr = snr_sum / num_pac;
	cout << "SNR = " << 10 * log10(snr) << endl;

	// SNR estimation
	double err0_tbl[8] = { 0, 0.43, 0.96, 1.97, 3.95, 7.95, 15.86, 27.62 };
	double err1_tbl[8] = { 0, 0.43, 0.97, 1.98, 4.00, 8.00, 15.98, 31.79 };
	double err2_tbl[8] = { 0, 2478.9, 6433.09, 14215.6, 29542.4, 58743.3, 112241, 192195 };
	double err3_tbl[8] = { 0, 1787.75, 4584.07, 10068.3, 20818.7, 41747.9, 80091.2, 136587 };
	double err4_tbl[8] = { 0, 2475.48, 6430.8, 14199, 29506.4, 59002.5, 112964, 191515 };
	double err5_tbl[8] = { 0, 1789.97, 4585.58, 10079.3, 20902.2, 41573.5, 79602.9, 137081 };
	double err6_tbl[8] = { 0, 0.39, 0.86, 1.77, 3.56, 7.14, 14.29, 20.23 };
	double exp = 13;

	double evar0 = err0_tbl[(size_t)Nia_s0i0] * err0_tbl[(size_t)Nia_s0i0];
	double evar1 = err1_tbl[(size_t)Nia_s0i1] * err1_tbl[(size_t)Nia_s0i1];
	double evar2 = (err2_tbl[(size_t)Nia_s0m0] / pow(2, exp))*(err2_tbl[(size_t)Nia_s0m0] / pow(2, exp));
	double evar3 = (err3_tbl[(size_t)Nia_s0m1] / pow(2, exp))*(err3_tbl[(size_t)Nia_s0m1] / pow(2, exp));
	double evar4 = (err4_tbl[(size_t)Nia_s0m2] / pow(2, exp))*(err4_tbl[(size_t)Nia_s0m2] / pow(2, exp));
	double evar5 = (err5_tbl[(size_t)Nia_s0m3] / pow(2, exp))*(err5_tbl[(size_t)Nia_s0m3] / pow(2, exp));
	double evar6 = err6_tbl[(size_t)Nia_s0o0] * err6_tbl[(size_t)Nia_s0o0];

	double snr_est = (var_x1 + var_x2) / (evar0 + 0.5*evar1 + evar2 + evar3 + evar4 + evar5 + 2 * evar6);

	cout << "SNR estimation = " << 10 * log10(snr_est) << endl;

	if (mode == 1) {

		cout << "stage0 input correlation --------------------------------------" << endl;
		io_stat in_corr_s0i(Nt, 1, s0i0_vec, s0i1_vec, corr_th, corr_th);
		in_corr_s0i.print_io_stat();
		cout << "stage0 butterfly intermdeidate --------------------------------------" << endl;
		io_stat in_corr_s0b(Nt + 1, 1, s0i2_vec, s0i3_vec, corr_th, corr_th);
		in_corr_s0b.print_io_stat();

		cout << "stage0 twiddle multiplication0 --------------------------------------" << endl;
		io_stat in_corr_s0t0(Nt + 2, 2, s0b0_vec, s0t0_vec, corr_th, corr_th);
		in_corr_s0t0.print_io_stat();

		cout << "stage0 twiddle multiplication1 --------------------------------------" << endl;
		io_stat in_corr_s0t1(Nt + 2, 2, s0b1_vec, s0t1_vec, corr_th, corr_th);
		in_corr_s0t1.print_io_stat();
		cout << "stage0 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s0t2(Nt + 2, 2, s0b1_vec, s0t0_vec, corr_th, corr_th);
		in_corr_s0t2.print_io_stat();
		cout << "stage0 twiddle multiplication3 --------------------------------------" << endl;
		io_stat in_corr_s0t3(Nt + 2, 2, s0b0_vec, s0t1_vec, corr_th, corr_th);
		in_corr_s0t3.print_io_stat();
		cout << "stage0 output adder0 --------------------------------------" << endl;
		io_stat in_corr_s0o0(2 * Nt, 1, s0c0_vec, s0c1_vec, corr_th, corr_th);
		in_corr_s0o0.print_io_stat();
		cout << "stage0 output adder1 --------------------------------------" << endl;
		io_stat in_corr_s0o1(2 * Nt, 1, s0c2_vec, s0c3_vec, corr_th, corr_th);
		in_corr_s0o1.print_io_stat();

		// sglee comment
		cout << "stage1 input correlation --------------------------------------" << endl;
		io_stat in_corr_s1i(Nt, 1, s1i0_vec, s1i1_vec, corr_th, corr_th);
		in_corr_s1i.print_io_stat();
		cout << "stage1 butterfly intermdeidate --------------------------------------" << endl;
		io_stat in_corr_s1b(Nt + 1, 1, s1i2_vec, s1i3_vec, corr_th, corr_th);
		in_corr_s1b.print_io_stat();
		cout << "stage1 twiddle multiplication1 --------------------------------------" << endl;
		io_stat in_corr_s1t0(Nt + 2, 2, s1b0_vec, s1t0_vec, corr_th, corr_th);
		in_corr_s1t0.print_io_stat();
		cout << "stage1 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s1t1(Nt + 2, 2, s1b1_vec, s1t1_vec, corr_th, corr_th);
		in_corr_s1t1.print_io_stat();
		cout << "stage1 output adder --------------------------------------" << endl;
		io_stat in_corr_s1o(2 * Nt, 1, s1c0_vec, s1c1_vec, corr_th, corr_th);
		in_corr_s1o.print_io_stat();

		cout << "stage2 input correlation --------------------------------------" << endl;
		io_stat in_corr_s2i(Nt, 1, s2i0_vec, s2i1_vec, corr_th, corr_th);
		in_corr_s2i.print_io_stat();
		cout << "stage2 butterfly intermdeidate --------------------------------------" << endl;
		io_stat in_corr_s2b(Nt + 1, 1, s2i2_vec, s2i3_vec, corr_th, corr_th);
		in_corr_s2b.print_io_stat();
		cout << "stage2 twiddle multiplication1 --------------------------------------" << endl;
		io_stat in_corr_s2t0(Nt + 2, 2, s2b0_vec, s2t0_vec, corr_th, corr_th);
		in_corr_s2t0.print_io_stat();
		cout << "stage2 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s2t1(Nt + 2, 2, s2b1_vec, s2t1_vec, corr_th, corr_th);
		in_corr_s2t1.print_io_stat();
		cout << "stage2 output adder --------------------------------------" << endl;
		io_stat in_corr_s2o(2 * Nt, 1, s2c0_vec, s2c1_vec, corr_th, corr_th);
		in_corr_s2o.print_io_stat();

		cout << "stage3 input correlation --------------------------------------" << endl;
		io_stat in_corr_s3i(Nt, 1, s3i0_vec, s3i1_vec, corr_th, corr_th);
		in_corr_s3i.print_io_stat();
		cout << "stage3 butterfly intermdeidate --------------------------------------" << endl;
		io_stat in_corr_s3b(Nt + 1, 1, s3i2_vec, s3i3_vec, corr_th, corr_th);
		in_corr_s3b.print_io_stat();
		cout << "stage3 twiddle multiplication1 --------------------------------------" << endl;
		io_stat in_corr_s3t0(Nt + 2, 2, s3b0_vec, s3t0_vec, corr_th, corr_th);
		in_corr_s3t0.print_io_stat();
		cout << "stage3 twiddle multiplication2 --------------------------------------" << endl;
		io_stat in_corr_s3t1(Nt + 2, 2, s3b1_vec, s3t1_vec, corr_th, corr_th);
		in_corr_s3t1.print_io_stat();
		cout << "stage3 output adder --------------------------------------" << endl;
		io_stat in_corr_s3o(2 * Nt, 1, s3c0_vec, s3c1_vec, corr_th, corr_th);
		in_corr_s3o.print_io_stat();
		// sgle comment end

		///////////////////////////////////////////////////////////////////////////////
		// Analysis
		///////////////////////////////////////////////////////////////////////////////

		for (int Nia_add_s0i0 = 1; Nia_add_s0i0 < 8; Nia_add_s0i0++) {
			loa add_s0i0(Nt, Nia_add_s0i0, Nia_add_s0i0 - 1, 0, 1);
			error_model err_add_s0i0;
			err_add_s0i0.error_est(add_s0i0.err_tbl, in_corr_s0i.io, Nia_add_s0i0 - 1, 0, 1);
			err_add_s0i0.print_err_stat(err_add_s0i0.est_err, 0);
		}

		for (int Nia_add_s0i1 = 1; Nia_add_s0i1 < 8; Nia_add_s0i1++) {
			loa add_s0i1(Nt, Nia_add_s0i1, Nia_add_s0i1 - 1, 0, 1);
			error_model err_add_s0i1;
			err_add_s0i1.error_est(add_s0i1.err_tbl, in_corr_s0b.io, Nia_add_s0i1 - 1, 0, 1);
			err_add_s0i1.print_err_stat(err_add_s0i1.est_err, 0);
		}

		int m1a_cal;
		int m1_cal;

		for (int Nia_mul_s0t0 = 1; Nia_mul_s0t0 < 8; Nia_mul_s0t0++) {
			bam mul_s0t0(2 * Nt, Nia_mul_s0t0, Nia_mul_s0t0, Nia_mul_s0t0 - 1, 0, 1);
			error_model err_mul_s0t0;
			err_mul_s0t0.error_est(mul_s0t0.err_tbl, in_corr_s0t0.io, Nia_mul_s0t0 - 1, 0, 1);


			cout << "Estimated" << endl;
			err_mul_s0t0.print_err_stat(err_mul_s0t0.est_err, 0);
		}

		for (int Nia_mul_s0t1 = 1; Nia_mul_s0t1 < 8; Nia_mul_s0t1++) {
			bam mul_s0t1(2 * Nt, Nia_mul_s0t1, Nia_mul_s0t1, Nia_mul_s0t1 - 1, 0, 1);
			error_model err_mul_s0t1;
			err_mul_s0t1.error_est(mul_s0t1.err_tbl, in_corr_s0t1.io, Nia_mul_s0t1 - 1, 0, 1);
			err_mul_s0t1.print_err_stat(err_mul_s0t1.est_err, 0);
		}

		for (int Nia_mul_s0t2 = 1; Nia_mul_s0t2 < 8; Nia_mul_s0t2++) {
			bam mul_s0t2(2 * Nt, Nia_mul_s0t2, Nia_mul_s0t2, Nia_mul_s0t2 - 1, 0, 1);
			error_model err_mul_s0t2;
			err_mul_s0t2.error_est(mul_s0t2.err_tbl, in_corr_s0t2.io, Nia_mul_s0t2 - 1, 0, 1);
			err_mul_s0t2.print_err_stat(err_mul_s0t2.est_err, 0);
		}

		for (int Nia_mul_s0t3 = 1; Nia_mul_s0t3 < 8; Nia_mul_s0t3++) {
			bam mul_s0t3(2 * Nt, Nia_mul_s0t3, Nia_mul_s0t3, Nia_mul_s0t3 - 1, 0, 1);
			error_model err_mul_s0t3;
			err_mul_s0t3.error_est(mul_s0t3.err_tbl, in_corr_s0t3.io, Nia_mul_s0t3 - 1, 0, 1);
			err_mul_s0t3.print_err_stat(err_mul_s0t3.est_err, 0);
		}

		for (int Nia_add_s0o0 = 1; Nia_add_s0o0 < 8; Nia_add_s0o0++) {
			loa add_s0o0(Nt, Nia_add_s0o0, Nia_add_s0o0 - 1, 0, 1);
			error_model err_add_s0o0;
			err_add_s0o0.error_est(add_s0o0.err_tbl, in_corr_s0o0.io, Nia_add_s0o0 - 1, 0, 1);
			err_add_s0o0.print_err_stat(err_add_s0o0.est_err, 0);
		}

		for (int Nia_add_s0o1 = 1; Nia_add_s0o1 < 8; Nia_add_s0o1++) {
			loa add_s0o1(Nt, Nia_add_s0o1, Nia_add_s0o1 - 1, 0, 1);
			error_model err_add_s0o1;
			err_add_s0o1.error_est(add_s0o1.err_tbl, in_corr_s0o1.io, Nia_add_s0o1 - 1, 0, 1);
			err_add_s0o1.print_err_stat(err_add_s0o1.est_err, 0);
		}

	} // for mode == 1



	cout << " sglee " << endl;

	getchar();
	return 1;
}
