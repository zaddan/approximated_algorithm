#ifndef _IOANAL
#define _IOANAL

#include <iostream>
#include <complex>
#include <vector>
#include <random>

#include "ac_types.h"
#include "sim_config.h"

using namespace std;

class io_stat {
	private:

	public:
		size_t Nt;
		size_t mode; // mode == 0: only mean & var, mode = 1: verbose

		//double mean_a;		// mean of a
		//double sdev_a;		// variance of a
		//double mean_b;		// mean of b
		//double sdev_b;		// variance of b
		//int bw_apc_a;		// bit-wise positive auto correlation of a
		//int bw_anc_a;		// bit-wise negative auto correlation of a
		//int bw_apc_b;		// bit-wise positive auto correlation of b
		//int bw_anc_b;		// bit-wise negative auto correlation of b
		//int bw_cpc;		// bit-wise positive cross correlation of a & b
		//int bw_cnc;		// bit-wise negative cross correlation of a & b
		//vector<double> bw_pr_a;		// bit-wise '0' probability of a
		//vector<double> bw_pr_b;		// bit-wise '1' probability of b

		IO_STAT io;

		io_stat();
		virtual ~io_stat();

		io_stat (const size_t &Nt, const size_t &mode);
		io_stat (const size_t &Nt, const size_t &mode, const ACVEC &a, const ACVEC &b, const double &th_ac, const double &th_cc);
		io_stat (const size_t &Nt, const size_t &mode, const ACVEC &a, const double &th_ac);
		void gen_stat (const ACVEC &a, const ACVEC &b, const double &th_ac, const double &th_cc);
		void gen_stat (const ACVEC &a, const double &th_ac);
		void copy_b_stat (double mean_b, double sdev_b, int bw_apc_b, int bw_anc_b, vector<double> bw_pr_b, vector<double> bw_apr_b);
		// mean and variance calculator for a variable
		void mean_var_cal (const ACVEC &a, double &mean, double &sdev);

		// bitwise auto correlation calculator for a variable
		void bw_acor_cal (const ACVEC &a, int &bw_apc, int &bw_anc, vector<double> &bw_ac_cnt, const double &th_ac);

		// bitwise probability for a variable
		void bw_pr_cal (const ACVEC &a, vector<double> &bw_pr);

		// biwise cross correlation calculator between two variables	
		void bw_ccor_cal (const ACVEC &a, const ACVEC &b, int &bw_cpc, int &bw_cnc, const double &th_cc);
		void print_io_stat (void);
};

#endif
