#ifndef _ERR_MODEL
#define _ERR_MODEL

#include <iostream>
#include <complex>
#include <vector>
#include <map>

#include "ac_types.h"
#include "sim_config.h"

using namespace std;

class error_model {

	private:
		double sdev_scale;
		double weak_corr_prob;
		double bw_pr_th;
		
	public:
		// from simulation
		ERR_STAT cal_err;

		// from estimation
		ERR_STAT est_err;
		ERR_STAT est_err2;

		ACETBL vld_err_tbl;
		int full_tbl_cnt;

		error_model();

		virtual ~error_model();

		// error calculation from simulation
		void error_cal(const ACVEC &ori, const ACVEC &apr, const size_t &Nt, const size_t &mode);
		void err_dist_cal(map<int, double> &err_dist, const vector<int> &err_vec);
		//double error_mean_cal(const int &sum_err, const size_t &size);
		//double error_sdev(vector<int>::iterator itr_begin, vector<int>::iterator itr_end, const double &err_mean, const size_t &size);
		double err_prob (const int &err_cnt, const size_t &size);

		// error estimation 
		void error_est(const ACETBL &err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb, const size_t &mode);
		void err_dist_est(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb);
		void err_dist_est2(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb);
		void err_prob_update(double *prob, const size_t &err_lsb, const size_t &err_msb, const int &pattern, const vector<double> &bw_pr, const int &apc, const int &anc);
		void err_prob_update_weakcorr(const vector<double> &bw_apr, double *prob, const size_t &err_lsb, const size_t &err_msb, const vector<double> &bw_pr, const int &pattern, const double &corr_pr, const int &apc, const int &anc);
		void err_tbl_refine(int *pvld, const size_t &i, const int &bw_ac, const int &err_tbl_val);
		void err_tbl_refine2(int *pvld, const size_t &i, const vector<double> &bw_pr, const double &prob_th, const int &err_tbl_val);
		void err_prob_update2 (double *prob, const size_t &err_lsb, const size_t &err_msb, const int &pattern, const vector<double> &bw_pr, const int &apc, const int &anc, const vector<double> &apr);

		double error_mean(const map<int,double> &err_dist);
		double error_sdev (const map<int,double> &err_dist, const double &mean); 
		double error_prob(const map<int,double> &err_dist);

		void print_err_stat(ERR_STAT &err, const size_t mode);
		void print_vld_err_tbl(void);

		// Old files: error_model_old.cpp
		void error_est_old1(const ACETBL &err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb, const size_t &mode);
		void error_est_old2(const ACETBL &err_tbl, const ACETBL &err_tbl_h, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb, const size_t &mode);
		void err_dist_est_old1(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb);
		void err_dist_est_old2(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const ACETBL &vld_err_tbl_h, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb);
		void err_prob_update_old(double *prob, const size_t &err_lsb, const size_t &err_msb, const int &pattern, const int &pattern_h, const vector<double> &bw_pr, const int &apc, const int &anc);
		
};

#endif
