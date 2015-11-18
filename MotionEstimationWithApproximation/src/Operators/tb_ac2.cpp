// This is a project for approximate computing. version 001
// Author : Seogoo Lee
// History :
// Initial creation : 2013.12.17
// 

#include <iostream>
#include <vector>
#include <complex>
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



using namespace std;


int main_ac2(void) {
	ACINT a = -46;
	ACINT b = 38;
	size_t Nt = 16;
	size_t Nia = 1;
	size_t msb = Nia - 1;
	size_t lsb = 0;
	//	size_t K = 4;
	size_t hbl = Nia;
	size_t vbl = hbl;
	size_t op_type = 1; // '1' for adder, '2' for multiplier

	// eta1 x(Nt, Nia);
	//eta1 x0(Nt, Nia, msb, lsb);
	//eta2 x1(Nt, K, msb, lsb);
	loa y0(Nt, Nia, msb, lsb, 1);
	//bam y0 (Nt,  hbl, vbl,  hbl-1,  0);
	//etm y1(Nt, Nia, msb, lsb);

	size_t size = 10000;
	int success = 0;
	ACVEC a_vec(size);
	ACVEC b_vec(size);
	size_t in_mode = 1;

	float mean_a = 0;
	float var_a = 100000;
	float mean_b = 0;
	float var_b = 100000;
	double corr_th = 0.95;

	if (in_mode == 0) { // uniform distribution 
		success = operand_gen(Nt, a_vec, b_vec); // uniform generator
	}
	else if (in_mode == 1) { // gaussian distribution
		success = operand_gen(Nt, mean_a, var_a, mean_b, var_b, a_vec, b_vec, 1, 0); // gaussian generator
	}

#ifdef DEBUG
	print_operand(a_vec, b_vec);
#endif
	size_t op_type2 = 1;
	if (op_type == 1)
		op_type2 = 1;
	else
		op_type2 = 2;

	io_stat in_corr(Nt, op_type2, a_vec, b_vec, corr_th, corr_th);

	//cout << "ETA1 result:" << x0.calc(a,b) << ", correct one: " << a+b << endl;
	//cout << "ETA2 result:" << x1.calc(a,b) << ", correct one: " << a+b << endl;
	//cout << "LOA result:" << x2.calc(a,b) << ", correct one: " << a+b << endl;
	//cout << "BAM result:" << y0.calc(a, b) << ", correct one: " << a*b << endl;
	//cout << "ETM result:" << y1.calc(a,b) << ", correct one: " << a*b << endl;

	//	x2.print_err_tbl();
	in_corr.print_io_stat();

	vector<double>::iterator itc = in_corr.io.bw_pr_b.begin();
	size_t i = 0;
	for (vector<double>::iterator it = in_corr.io.bw_pr_a.begin(); it != in_corr.io.bw_pr_a.end(); it++) {
		cout << "bit[" << i << "] prob.: " << *it << ", " << *itc << endl;
		++itc;
		++i;
	}

	ACVEC c;
	ACVEC c_ori;
	for (Nia = 1; Nia < 9; Nia++) {
		bam y0(Nt, Nia, Nia, Nia - 1, 0, 1);
		ACVEC::iterator itb = b_vec.begin();
		int cal;
		int cal_ref;
		for (ACVEC::iterator ita = a_vec.begin(); ita != a_vec.end(); ita++) {
			cal = y0.calc(*ita, *itb);
#ifdef DEBUG
			cout << hex << "output: " << cal << ", correct: " << *ita + *itb << dec << endl;
#endif
			cal_ref = y0.calc_ref(*ita, *itb);
			c.push_back(cal);
			c_ori.push_back(cal_ref);
			++itb;
		}

		//	y0.print_err_tbl();

		error_model err;

		//err.error_cal(c_ori, c, Nt, 1);
		err.error_est(y0.err_tbl, in_corr.io, hbl - 1, 0, 1);
		//err.error_est2(y0.err_tbl2, y0.err_tbl_h, in_corr.io, Nia - 1, 0, 1);

		//err.print_vld_err_tbl();

		err.print_err_stat(err.est_err, 0);
		//err.print_err_stat(err.est_err2, 0);
		//err.print_err_stat(err.cal_err, 0);
	};
	///////////////////////////////////////////////////////////////////////////////
	// Error - Input covariance calculation
	// Covariance = E[(a-mean_a)*(b-mean_b)] 
	///////////////////////////////////////////////////////////////////////////////
	/*
	double err_sum = 0;
	ACVEC::iterator itc_ac = c.begin();
	ACVEC::iterator itc_ori = c_ori.begin();
	for (ACVEC::iterator ita = b_vec.begin(); ita != b_vec.end(); ita++) {
		//cout << "INPUT: " << (*ita) << ", " << (*itc_ac) << ", " << (*itc_ori) << endl;
		err_sum += ((*ita - 0) * (*itc_ac - *itc_ori - err.cal_err.err_mean)) / (sqrt(var_b)*err.cal_err.err_sdev);
		++itc_ac;
		++itc_ori;
	}
	err_sum = err_sum / a_vec.size();
	
	cout << "Error covariance: " << err_sum << endl;
	*/
	printf(" Error covariance: ");
	//getchar();
	return 1;
}
