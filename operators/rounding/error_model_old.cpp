
#include <iostream>
#include <complex>
#include <vector>
#include <map>

#include "ac_types.h"
#include "error_model.h"

using namespace std;

void error_model::error_est_old1(const ACETBL &err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb, const size_t &mode) {
	int apc_a_vld;
	int anc_a_vld;
	int apc_b_vld;
	int anc_b_vld;

	//double bw_pr_th = 0.90; // ignore the case if Pr(A=1)>bw_pr_th
	// using bitwise correlation
	cout << "ERR table size: " << err_tbl.size() << endl;
	for (ACETBL::const_iterator it = err_tbl.begin(); it != err_tbl.end(); it++) {
		apc_a_vld = 1;
		apc_b_vld = 1;
		anc_a_vld = 1;
		anc_b_vld = 1;

		for (size_t i = err_lsb; i<err_msb+1; i++) {

			// bit probability
			err_tbl_refine2(&apc_a_vld, i, io.bw_pr_a, bw_pr_th, (*it).first.first);
//			if ((*it).first.second != 0xffffffff) {
			if ((((*it).first.second)&0x80000000) != 0x80000000) {
					err_tbl_refine2(&apc_a_vld, i, io.bw_pr_b, bw_pr_th, (*it).first.second);
			}

			if (i != err_msb) {
				// positive correlation
				err_tbl_refine(&apc_a_vld, i, io.bw_apc_a, (*it).first.first);
//				if ((*it).first.second != 0xffffffff) {
				if ((((*it).first.second) & 0x80000000) != 0x80000000) {

					err_tbl_refine(&apc_b_vld, i, io.bw_apc_b, (*it).first.second);
				}
				// negative correlation
				err_tbl_refine(&anc_a_vld, i, io.bw_anc_a, (*it).first.first);
//				if ((*it).first.second != 0xffffffff) {
				if ((((*it).first.second) & 0x80000000) != 0x80000000) {
					err_tbl_refine(&anc_b_vld, i, io.bw_anc_b, (*it).first.second);
				}
			}

		}

//		if ((apc_a_vld && apc_b_vld) && (anc_a_vld && anc_b_vld) && ((*it).first.first < 128)) {
		if ((apc_a_vld && apc_b_vld) && (anc_a_vld && anc_b_vld)) {
				vld_err_tbl.push_back(*it);
			//cout << "SGLEE: " << (*it).first.first << ", " << (*it).first.second << ", " << (*it).second.first << endl;
		}
	}

//	if ((*(err_tbl.begin())).first.second != 0xffffffff) {
	if ((((*(err_tbl.begin())).first.second)&0x80000000) != 0x80000000) {
			full_tbl_cnt = pow(2.0, 2 * (err_msb - err_lsb + 1));
	}
	else {
		full_tbl_cnt = pow(2.0,(err_msb-err_lsb+1));
	}

//	cout << "sglee " << full_tbl_cnt << ", " << err_msb-err_lsb << endl;

	for (size_t i = err_lsb; i<err_msb+1; i++) {
		// bitwise correlation
		if (i != err_msb) {
			// for A
			if (((io.bw_apc_a>>i)&0x1) || ((io.bw_anc_a>>i)&0x1)) {
				full_tbl_cnt = full_tbl_cnt/2;
			}
			// for B
			if (((io.bw_apc_b>>i)&0x1) || ((io.bw_anc_b>>i)&0x1)) {

				// only for adder
//				if ((*(err_tbl.begin())).first.second != 0xffffffff) {
				if ((((*(err_tbl.begin())).first.second) & 0x80000000) != 0x80000000) {
						full_tbl_cnt = full_tbl_cnt / 2;
				}
			}
		}
		else {
			// bitwise probability of A
			if ((io.bw_pr_a[i] > bw_pr_th) || (io.bw_pr_a[i] < (1-bw_pr_th))) {
//				cout << "sglee2 " << full_tbl_cnt << ", " << io.bw_pr_a[i] << endl;
				full_tbl_cnt = full_tbl_cnt/2;
//				cout << "sglee2 " << full_tbl_cnt << ", " << i << endl;

			}
			// bitwise probability of B
			if ((io.bw_pr_b[i] > bw_pr_th) || (io.bw_pr_b[i] < (1-bw_pr_th))) {
				// only for adder
//				if ((*(err_tbl.begin())).first.second != 0xffffffff) {
				if ((((*(err_tbl.begin())).first.second) & 0x80000000) != 0x80000000) {
					full_tbl_cnt = full_tbl_cnt / 2;
				}
			}
		}

	//	full_tbl_cnt = 2*full_tbl_cnt;
	//cout << "sglee2 " << full_tbl_cnt << endl;


	}

	//full_tbl_cnt = full_tbl_cnt/2; //sglee_temp

	//cout << "sglee2 " << full_tbl_cnt << endl;


	// Don't need to get the number of full possible truth table in this way
    /*
	// build a complete table
	//map<int,int> full_tbl;

	for (size_t i=0;i<pow(2,err_msb-err_lsb+1);i++) {
		for (size_t j=0;j<pow(2,err_msb-err_lsb+1);j++) {
			full_tbl.push_back(make_pair(i<<err_lsb,j<<err_lsb));
		}
	}



	int apc_a_vld_full;
	int apc_b_vld_full;
	int anc_a_vld_full;
	int anc_b_vld_full;
	full_tbl_cnt = 0;

	for (vector<pair<int,int> >::iterator it = full_tbl.begin(); it != full_tbl.end(); it++) {
		apc_a_vld_full = 1;
		apc_b_vld_full = 1;
		anc_a_vld_full = 1;
		anc_b_vld_full = 1;

		for (size_t i = err_lsb; i<err_msb; i++) {
			// for positive correlation
			err_tbl_refine(&apc_a_vld_full, i, io.bw_apc_a, (*it).first);
			err_tbl_refine(&apc_b_vld_full, i, io.bw_apc_b, (*it).second);

			// for negative correlation
			err_tbl_refine(&anc_a_vld_full, i, io.bw_anc_a, (*it).first);
			err_tbl_refine(&anc_b_vld_full, i, io.bw_anc_b, (*it).second);

		}

		if ((apc_a_vld_full && apc_b_vld_full) && (anc_a_vld_full && anc_b_vld_full)) {
			full_tbl_cnt += 1;
		}
	}
	*/

	cout << "EST1 vld_err_tbl: " << vld_err_tbl.size() << " / " << full_tbl_cnt << endl;

	// error statistics
	est_err.err_max = 0;
	est_err.err_min = 0;
	vector<int> err_vec;
	//int err_cnt = 0;
	//int sum_err = 0;
	int tmp_max;
	int tmp_min;
	for (ACETBL::iterator it = vld_err_tbl.begin(); it != vld_err_tbl.end(); it++) {
		//err_vec.push_back((*it).second.second);

		// figuring out min/max
//		if ((*it).first.second != 0xffffffff) {
		if ((((*it).first.first) & 0x80000000) == 0x80000000) {
// compile error			tmp_max = (*it).second.first*(int)floor((io.mean_b + sdev_scale*io.sdev_b));
// compile error			tmp_max = tmp_max + (*it).second.second;
// compile error			tmp_min = (*it).second.first*(int)floor((io.mean_b - sdev_scale*io.sdev_b));
// compile error			tmp_min = tmp_min + (*it).second.second;
			//cout << "sglee for btm : " << tmp_max << ", " << tmp_min << endl;
		}
		else if ((((*it).first.second) & 0x80000000) != 0x80000000) {
			tmp_max = (*it).second.second;
			tmp_min = (*it).second.second;
		}
		else {
// compile error			tmp_max = (*it).second.first*(int)floor((io.mean_b+sdev_scale*io.sdev_b));
// compile error			tmp_max = tmp_max+(*it).second.second;
// compile error			tmp_min = (*it).second.first*(int)floor((io.mean_b-sdev_scale*io.sdev_b));
// compile error			tmp_min = tmp_min+(*it).second.second;
			//cout << "sglee " << tmp_max << ", " << tmp_min << ", " << io.mean_b << ", " << io.sdev_b << endl;
		}

		if (tmp_max > est_err.err_max)
			est_err.err_max = tmp_max;

		if (tmp_min < est_err.err_min)
			est_err.err_min = tmp_min;

		//if ((*it).second.second != 0) {
		//	err_cnt += 1;
		//}

		//sum_err += (*it).second.second;

	}

	err_dist_est(est_err.err_dist, full_tbl_cnt, vld_err_tbl, io, err_msb, err_lsb);

	// mean
	est_err.err_mean = error_mean(est_err.err_dist);

	// sdev
	est_err.err_sdev = error_sdev (est_err.err_dist, est_err.err_mean);

	// total error probability
	est_err.err_total_prob = error_prob(est_err.err_dist);


}


void error_model::error_est_old2(const ACETBL &err_tbl, const ACETBL &err_tbl_h, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb, const size_t &mode) {
	int apc_a_vld;
	int anc_a_vld;
	int apc_b_vld;
	int anc_b_vld;
/* compile error
	//double bw_pr_th = 0.90; // ignore the case if Pr(A=1)>bw_pr_th
	// using bitwise correlation
	for (ACETBL::const_iterator it = err_tbl.begin(); it != err_tbl.end(); it++) {
		apc_a_vld = 1;
		apc_b_vld = 1;
		anc_a_vld = 1;
		anc_b_vld = 1;

		for (size_t i = err_lsb; i<err_msb + 1; i++) {

			// bit probability
			err_tbl_refine2(&apc_a_vld, i, io.bw_pr_a, bw_pr_th, (*it).first.first);
			if ((*it).first.second != 0xffffffff) {
				err_tbl_refine2(&apc_a_vld, i, io.bw_pr_b, bw_pr_th, (*it).first.second);
			}

			if (i != err_msb) {
				// positive correlation
				err_tbl_refine(&apc_a_vld, i, io.bw_apc_a, (*it).first.first);
				if ((*it).first.second != 0xffffffff) {
					err_tbl_refine(&apc_b_vld, i, io.bw_apc_b, (*it).first.second);
				}
				// negative correlation
				err_tbl_refine(&anc_a_vld, i, io.bw_anc_a, (*it).first.first);
				if ((*it).first.second != 0xffffffff) {
					err_tbl_refine(&anc_b_vld, i, io.bw_anc_b, (*it).first.second);
				}
			}

		}

		if ((apc_a_vld && apc_b_vld) && (anc_a_vld && anc_b_vld)) {
			vld_err_tbl2.push_back(*it);
		}
	}

	if ((*(err_tbl.begin())).first.second != 0xffffffff) {
		full_tbl_cnt = (int)pow(2.0, (int)(2 * (err_msb - err_lsb + 1)));
	}
	else {
		full_tbl_cnt = (int)pow(2.0, (int)(err_msb - err_lsb + 1));
	}

	//	cout << "sglee " << full_tbl_cnt << ", " << err_msb-err_lsb << endl;

	for (size_t i = err_lsb; i<err_msb + 1; i++) {
		// bitwise correlation
		if (i != err_msb) {
			// for A
			if (((io.bw_apc_a >> i) & 0x1) || ((io.bw_anc_a >> i) & 0x1)) {
				full_tbl_cnt = full_tbl_cnt / 2;
			}
			// for B
			if (((io.bw_apc_b >> i) & 0x1) || ((io.bw_anc_b >> i) & 0x1)) {

				// only for adder
				if ((*(err_tbl.begin())).first.second != 0xffffffff) {
					full_tbl_cnt = full_tbl_cnt / 2;
				}
			}
		}
		else {
			// bitwise probability of A
			if ((io.bw_pr_a[i] > bw_pr_th) || (io.bw_pr_a[i] < (1 - bw_pr_th))) {
				//				cout << "sglee2 " << full_tbl_cnt << ", " << io.bw_pr_a[i] << endl;
				full_tbl_cnt = full_tbl_cnt / 2;
				//				cout << "sglee2 " << full_tbl_cnt << ", " << i << endl;

			}
			// bitwise probability of B
			if ((io.bw_pr_b[i] > bw_pr_th) || (io.bw_pr_b[i] < (1 - bw_pr_th))) {
				// only for adder
				if ((*(err_tbl.begin())).first.second != 0xffffffff) {
					full_tbl_cnt = full_tbl_cnt / 2;
				}
			}
		}

		//	full_tbl_cnt = 2*full_tbl_cnt;
		//cout << "sglee2 " << full_tbl_cnt << ", " << err_msb-err_lsb << endl;


	}

	// error statistics
	est_err2.err_max = 0;
	est_err2.err_min = 0;
	vector<int> err_vec;
	//int err_cnt = 0;
	//int sum_err = 0;
	int tmp_max;
	int tmp_min;
	for (ACETBL::iterator it = vld_err_tbl2.begin(); it != vld_err_tbl2.end(); it++) {
		//err_vec.push_back((*it).second.second);

		// figuring out min/max
		if ((*it).first.second != 0xffffffff) {
			tmp_max = (*it).second.second;
			tmp_min = (*it).second.second;
		}
		else {
			tmp_max = (*it).second.first*(int)floor((io.mean_b + sdev_scale*io.sdev_b) + 0.5);
			tmp_max = tmp_max + (*it).second.second;
			tmp_min = (*it).second.first*(int)floor((io.mean_b - sdev_scale*io.sdev_b) + 0.5);
			tmp_min = tmp_min + (*it).second.second;
			//cout << "sglee " << tmp_max << ", " << tmp_min << ", " << io.mean_b << ", " << io.sdev_b << endl;
		}

		if (tmp_max > est_err2.err_max)
			est_err2.err_max = tmp_max;

		if (tmp_min < est_err2.err_min)
			est_err2.err_min = tmp_min;

		//if ((*it).second.second != 0) {
		//	err_cnt += 1;
		//}

		//sum_err += (*it).second.second;

	}

	err_dist_est2(est_err2.err_dist, full_tbl_cnt, vld_err_tbl2, err_tbl_h, io, err_msb, err_lsb);

	// mean
	est_err2.err_mean = error_mean(est_err2.err_dist);

	// sdev
	est_err2.err_sdev = error_sdev(est_err2.err_dist, est_err2.err_mean);

	// total error probability
	est_err2.err_total_prob = error_prob(est_err2.err_dist);

*/

}




//double error_model::error_mean_cal(const int &sum_err, const size_t &size) {
//	return (double) sum_err/size;
//}


//double error_model::error_sdev(vector<int>::iterator itr_begin, vector<int>::iterator itr_end, const double &err_mean, const size_t &size) {
//	// sdev
//	double E = 0;
//	for (vector<int>::iterator itr = itr_begin; itr != itr_end; itr++) {
//		E += (*itr - cal_err.err_mean)*(*itr - cal_err.err_mean);
//	}
//	return sqrt(E/size);
//
//}






void error_model::err_dist_est_old1(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb) {

/*
	double total_err_prob = 0;
	double prob;
	int i;

	// initialize a vector of pair
	for (int i = est_err.err_min; i < est_err.err_max; i++) {
		err_dist[i] = 0;
	}

	int tmp_max;
	int tmp_min;
	double subprob;


	for (ACETBL::const_iterator itr = vld_err_tbl.begin(); itr != vld_err_tbl.end(); itr++) {
		// initialize the prob. with uniform distribution
		prob = (double) 1/ (double) full_tbl_cnt;

		//cout <<"sglee2:" << prob << endl;
		if ((((*itr).first.first) & 0x80000000) == 0x80000000) {
			// for operand A
			err_prob_update(&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);

			// Weak correlation: for operand A
			err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// for operand B
			err_prob_update(&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b);

			// Weak correlation: for operand B
			err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

			// error min/max definition
			tmp_max = (int)floor(io.mean_b + sdev_scale*io.sdev_b);
			tmp_min = (int)floor(io.mean_b - sdev_scale*io.sdev_b);

			//cout << "sglee5 " << io.sdev_b << ", " << io.mean_b << endl;

			for (int i = tmp_min; i != tmp_max + 1; i++) {

				// uniformly distributed B within min/max range
				subprob = prob / (tmp_max - tmp_min + 1);

				err_dist[(*itr).second.first*i + (*itr).second.second] += subprob;
			}

			total_err_prob += prob;

		}
//		if ((*itr).first.second != 0xffffffff) {
		else if ((((*itr).first.second) & 0x80000000) != 0x80000000) {
			// for operand A
			err_prob_update (&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);

			// Weak correlation: for operand A
			err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// for operand B
			err_prob_update (&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b);

			// Weak correlation: for operand B
			err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

			err_dist[(*itr).second.second] += prob;
			total_err_prob += prob;
		}
		else {
			// for operand A
			//cout << "sglee3 " << prob << ", " << (*itr).first.first << ", " << io.bw_pr_a[0] << endl;
			//cout << "sglee4 " << prob << ", " << (*itr).first.first << endl;

			err_prob_update (&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);
			// Weak correlation for operand A
			//cout << "sglee8 " << prob << ", " << (*itr).first.first << endl;
			err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);
			//cout << "sglee7 " << prob << ", " << (*itr).first.first << endl;

			// error min/max definition
			tmp_max = (int)floor(io.mean_b+sdev_scale*io.sdev_b);
			tmp_min = (int)floor(io.mean_b-sdev_scale*io.sdev_b);

			//cout << "sglee5 " << tmp_max << ", " << tmp_min << endl;

			for (int i=tmp_min; i != tmp_max+1; i++) {

				// uniformly distributed B within min/max range
				subprob = prob/(tmp_max-tmp_min+1);

				err_dist[(*itr).second.first*i+(*itr).second.second] += subprob;
			}

			total_err_prob += prob;

		}


	}

	err_dist[0] = 1-total_err_prob;

*/
}




void error_model::err_dist_est_old2(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl2, const ACETBL &vld_err_tbl_h, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb) {

/*
	double total_err_prob = 0;
	double prob;
	int i;


	// initialize a vector of pair
	for (int i = est_err.err_min; i < est_err.err_max; i++) {
		err_dist[i] = 0;
	}

	int tmp_max;
	int tmp_min;
	double subprob;

	ACETBL2::const_iterator itr_h = vld_err_tbl_h.begin();
	for (ACETBL::const_iterator itr = vld_err_tbl2.begin(); itr != vld_err_tbl2.end(); itr++) {
		// initialize the prob. with uniform distribution
		prob = (double)1 / (double)full_tbl_cnt;

		//cout <<"sglee2:" << prob << endl;
//		if ((*itr).first.second != 0xffffffff) {
		if ((((*itr).first.second) & 0x80000000) != 0x80000000) {

			// for operand A
			err_prob_update2(&prob, err_lsb, err_msb, (*itr).first.first, (*itr_h).first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);

			// Weak correlation: for operand A
			err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// for operand B
			err_prob_update2(&prob, err_lsb, err_msb, (*itr).first.second, (*itr_h).second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b);

			// Weak correlation: for operand B
			err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

			err_dist[(*itr).second.second] += prob;
			total_err_prob += prob;
		}

		itr_h++;

	}

	err_dist[0] = 1 - total_err_prob;
*/

}

void error_model::err_prob_update_old(double *prob, const size_t &err_lsb, const size_t &err_msb, const int &pattern, const int &pattern_h, const vector<double> &bw_pr, const int &apc, const int &anc) {

	for (size_t i = err_lsb; i<err_msb + 1; i++) {

		// always work for LSB
		if (i == 0) {
			if ((pattern_h & 0x1) == 1) {
				if ((pattern & 0x1) == 0) { // LSB is 0
					*prob = (*prob) * 2 * (1 - bw_pr[i]);
				}
				else { // LSB is 1
					*prob = (*prob) * 2 * bw_pr[i];
				}
			} // if patterh_h
			else { // don't care
				*prob = (*prob) * 2;
			} // else pattern_h
		}
		else { // i>0
			if ((((apc >> (i - 1)) & 0x1) == 1) || (((anc >> (i - 1)) & 0x1) == 1)) {// correlated: either APC or ANC is 1
				*prob = *prob; // skip updating
			}

			// weakly correlated: skip updating
			else if ((((apc >> i) & 0x1) == 1) && (((apc >> (i - 1)) & 0x1) == 0)) {
				*prob = *prob; // skip updating
			}
			// weakly correlated: skip updating
			else if ((((anc >> i) & 0x1) == 1) && (((anc >> (i - 1)) & 0x1) == 0)) {
				*prob = *prob; // skip updating
			}

			else { // uncorrelated
				if (((pattern_h >> i) & 0x1) == 0x1) {
					if (((pattern >> i) & 0x1) == 0) {// bit is 0

						//cout << "sglee: " << i << ", " << pattern << ", " << bw_pr[i] << ", " << (*prob) << endl;
						*prob = (*prob) * 2 * (1 - bw_pr[i]);
						//cout << "sglee2: " << (*prob) << endl;

					}
					else { // bit is 1
						//cout << "sglee8: " << i << ", " << pattern << ", " << bw_pr[i] << ", " << (*prob) << endl;
						*prob = (*prob) * 2 * bw_pr[i];
						//cout << "sglee9: " << (*prob) << endl;
					}
				}
				else { // dont care
					*prob = (*prob) * 2;
				}
			}
		}
	}
}


void error_model::err_dist_est2(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb) {

	double total_err_prob = 0;
	double prob;
	int i;

	// initialize a vector of pair
	for (int i = est_err.err_min; i < est_err.err_max; i++) {
		err_dist[i] = 0;
	}

	int tmp_max;
	int tmp_min;
	double subprob;


	for (ACETBL::const_iterator itr = vld_err_tbl.begin(); itr != vld_err_tbl.end(); itr++) {
		// initialize the prob. with uniform distribution
		prob = (double) 1/ (double) full_tbl_cnt;

		//cout << "BASE PROB.:" << prob << endl;
		// for multiplier
		if ((((*itr).first.first) & 0x80000000) == 0x80000000) {
			// for operand A
			err_prob_update(&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);

			// Weak correlation: for operand A
			err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// for operand B
			err_prob_update(&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b);

			// Weak correlation: for operand B
			err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

			// error min/max definition
			tmp_max = (int)floor((io.mean_b + sdev_scale*io.sdev_b)*(*itr).second.first.second)+(int)floor((io.mean_a + sdev_scale*io.sdev_a)*(*itr).second.first.first)+(*itr).second.first.second;
			tmp_min = (int)floor((io.mean_b - sdev_scale*io.sdev_b)*(*itr).second.first.second)+(int)floor((io.mean_a - sdev_scale*io.sdev_a)*(*itr).second.first.first)-(*itr).second.first.second;

			//cout << "sglee5 " << (*itr).second.first.first << ", " << (*itr).second.first.second << endl;

			for (int i = tmp_min; i != tmp_max + 1; i++) {

				// uniformly distributed B within min/max range
				subprob = prob / (tmp_max - tmp_min + 1);

				err_dist[i] += subprob;
			}

			total_err_prob += prob;

		}
		// for adder
		else if ((((*itr).first.first) & 0x80000000) != 0x80000000) {
			// for operand A
			err_prob_update (&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);

			// Weak correlation: for operand A
			err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// for operand B
			err_prob_update (&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b);

			// Weak correlation: for operand B
			err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

			err_dist[(*itr).second.second] += prob;
			total_err_prob += prob;
		}

	}

	err_dist[0] = 1-total_err_prob;

}

void error_model::err_prob_update (double *prob, const size_t &err_lsb, const size_t &err_msb, const int &pattern, const vector<double> &bw_pr, const int &apc, const int &anc) {

	for (size_t i = err_lsb; i<err_msb+1;i++) {

		//cout << "sglee3: " << *prob << ", " << i << ", " << bw_pr[i] << ", pattern: " << pattern << endl;
		// always work for LSB
		if (i==0) {
			if ((pattern & 0x1) == 0) { // LSB is 0
				*prob = (*prob)*2*(1-bw_pr[i]);
			}
			else { // LSB is 1
				*prob = (*prob)*2*bw_pr[i];
			}
		}
		else { // i>0
			if ((((apc>>(i-1))&0x1) == 1) || (((anc>>(i-1))&0x1) == 1)) {// correlated: either APC or ANC is 1
				*prob = *prob; // skip updating
			}

			// weakly correlated: skip updating
			else if ((((apc>>i)&0x1) == 1) && (((apc>>(i-1))&0x1) == 0)) {
				*prob = *prob; // skip updating
			}
			// weakly correlated: skip updating
			else if ((((anc>>i)&0x1) == 1) && (((anc>>(i-1))&0x1) == 0)) {
				*prob = *prob; // skip updating
			}

			else { // uncorrelated
				if (((pattern>>i)&0x1) == 0) {// bit is 0

					//cout << "sglee: " << i << ", " << pattern << ", " << bw_pr[i] << ", " << (*prob) << endl;
					*prob = (*prob)*2*(1-bw_pr[i]);
					//cout << "sglee2: " << (*prob) << endl;

				}
				else { // bit is 1
					//cout << "sglee8: " << i << ", " << pattern << ", " << bw_pr[i] << ", " << (*prob) << endl;
					*prob = (*prob)*2*bw_pr[i];
					//cout << "sglee9: " << (*prob) << endl;
				}
			}
		}
	}
}

void error_model::err_prob_update_weakcorr (const vector<double> &bw_apr, double *prob, const size_t &err_lsb, const size_t &err_msb, const vector<double> &bw_pr, const int &pattern, const double &corr_pr, const int &apc, const int &anc) {

	for (size_t i = err_lsb+1; i<err_msb+1;i++) {

		// positive weak correlation
		if ((((apc>>i)&0x1) == 1) && (((apc>>(i-1))&0x1) == 0)) {

			//cout << "sglee: " << *prob << ", " << i << ", " << ((apc>>i)&0x1) << ", " << ((apc>>(i-1))&0x1) << endl;

			// if two bits are the same
			if (((pattern>>i)&0x1) == ((pattern>>(i-1))&0x1)) {

				// if both are 1s
				if (((pattern>>i)&0x1) == 0x1) {
					//cout << "sgle30: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
					// *prob = (*prob)*2*bw_pr[i];
					*prob = (*prob)*2*corr_pr;
					//*prob = (*prob)*2*(1-bw_apr[i-1])*bw_pr[i]/bw_pr[i-1];
					//cout << "sgle30: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
				}
				else {
					//cout << "sgle31: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
					//*prob = (*prob)*2*(1-bw_pr[i]);
					*prob = (*prob)*2*corr_pr;
					//*prob = (*prob)*2*((1-bw_pr[i-1])-bw_apr[i-1]*bw_pr[i])/(1-bw_pr[i-1]);
					//cout << "sgle31: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
				}
				//cout << "sgle4: " << (*prob) << endl;
			}
			else {
				if (((pattern>>i)&0x1) == 0x1) {
					//cout << "sgle50: " << i << ", " << pattern << ", " << (*prob) << endl;
					//*prob = (*prob)*2*bw_pr[i];
					*prob = (*prob)*2*(1-corr_pr);
					//*prob = (*prob)*2*bw_apr[i-1]*bw_pr[i]/(1-bw_pr[i-1]);
					//cout << "sgle50: " << i << ", " << pattern << ", " << (*prob) << endl;
				}
				else {
					//cout << "sgle51: " << i << ", " << pattern << ", " << (*prob) << endl;
					//*prob = (*prob)*2*(1-bw_pr[i]);
					*prob = (*prob)*2*(1-corr_pr);
					//*prob = (*prob)*2*(bw_pr[i-1]-(1-bw_apr[i-1])*bw_pr[i])/bw_pr[i-1];
					//cout << "sgle51: " << i << ", " << pattern << ", " << (*prob) << endl;
				}

			}
		}
		// negative weak correlation
		else if ((((anc>>i)&0x1) == 1) && (((anc>>(i-1))&0x1) == 0)) {

			if (((pattern>>i)&0x1) != ((pattern>>(i-1))&0x1)) {
				*prob = (*prob)*2*corr_pr;

			}
			else {
				*prob = (*prob)*2*(1-corr_pr);
			}
		}
	}

	//cout << "sglee3: " << *prob << endl;

}

