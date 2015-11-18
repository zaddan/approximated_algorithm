#include <iostream>
#include <complex>
#include <vector>
#include <map>

#include "ac_types.h"
#include "error_model.h"

using namespace std;

error_model::error_model(void) {  
  cal_err.err_max = 0;
  cal_err.err_min = 0;
  weak_corr_prob = 0.75; // for adder: 0.75, for mult: 0.5
  sdev_scale = sqrt(3);
  bw_pr_th = 0.99;
  full_tbl_cnt = 0;
}
error_model::~error_model(void) {}

void error_model::error_cal(const ACVEC &ori, const ACVEC &apr, const size_t &Nt, const size_t &mode) { 
  int tmp_err;
  int sum_err = 0;
  int err_cnt = 0;
  vector<int> err_vec;
  ACVEC::const_iterator it_ori = ori.begin();

  cal_err.err_max = 0;
  cal_err.err_min = 0;

  int apr_value;
  int sign_mask;
  int sign_bit;
  int bit_mask;
  int minus_mask;
  int plus_mask;
  for (ACVEC::const_iterator it_apr = apr.begin(); it_apr != apr.end(); it_apr++) {

	  sign_mask = (int)pow(2.0,(int)(Nt-1));
	  sign_bit = sign_mask & (*it_apr);
//	  sign_bit = sign_bit >> (Nt-1);
	  sign_bit = sign_bit >> (Nt-1);
	  bit_mask = (int)pow(2.0, (int)Nt) - 1;
	  minus_mask = 0xffffffff ^ bit_mask;
	  plus_mask = 0xffffffff & bit_mask;
	  apr_value = (sign_bit == 1) ? minus_mask | (*it_apr) : plus_mask & (*it_apr);

	  tmp_err = (*it_ori) - apr_value;

	  //cout << "EM: " << sign_bit << ", " << *it_apr << ", " << apr_value << ", " << *it_ori << ", " << tmp_err << endl;
	  //if (tmp_err > 50000) getchar();
	  sum_err += tmp_err;
	  err_vec.push_back(tmp_err);

	  if (tmp_err > cal_err.err_max)
		  cal_err.err_max = tmp_err;

	  if (tmp_err < cal_err.err_min)
		  cal_err.err_min = tmp_err;

	  if (tmp_err != 0) {
		  err_cnt += 1;

#ifdef DEBUG_EM
		  cout << "sglee apr: " << *it_apr << " , ori: " << *it_ori << " , err: " << tmp_err << endl;
#endif
	  }

	  ++it_ori;
  }

  // distribution	
  err_dist_cal(cal_err.err_dist, err_vec);

  // mean
  cal_err.err_mean = error_mean(cal_err.err_dist);

  // sdev
  cal_err.err_sdev = error_sdev(cal_err.err_dist, cal_err.err_mean);

  // error probability
  cal_err.err_total_prob = error_prob (cal_err.err_dist);

}

double error_model::err_prob (const int &err_cnt, const size_t &size) {
	return (double) err_cnt/size;
}

void error_model::err_dist_cal(map<int, double> &err_dist, const vector<int> &err_vec) {
	// offset for int -> size_t

	// initialize a vector of pair
	for (int i = cal_err.err_min; i < cal_err.err_max; i++) {
		err_dist[i] = 0;
	}

	// histogram
	for (vector<int>::const_iterator itr = err_vec.begin(); itr != err_vec.end(); itr++ ) {
		++err_dist[*itr];
	}

	// to probability
	for (map<int,double>::iterator itr = err_dist.begin(); itr != err_dist.end(); itr++) {
		(*itr).second = (*itr).second/err_vec.size();
	}
}


void error_model::error_est(const ACETBL &err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb, const size_t &mode) {
	int apc_a_vld;
	int anc_a_vld;
	int apc_b_vld;
	int anc_b_vld;

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
			err_tbl_refine2(&apc_b_vld, i, io.bw_pr_b, bw_pr_th, (*it).first.second);

			if (i != err_msb) {
				// positive correlation
				err_tbl_refine(&apc_a_vld, i, io.bw_apc_a, (*it).first.first);
				err_tbl_refine(&apc_b_vld, i, io.bw_apc_b, (*it).first.second);

				// negative correlation
				err_tbl_refine(&anc_a_vld, i, io.bw_anc_a, (*it).first.first);
				err_tbl_refine(&anc_b_vld, i, io.bw_anc_b, (*it).first.second);
			}
		}
//		cout << hex << "SGLEE: " << (*it).first.first << ", " << (*it).first.second << dec << apc_a_vld << apc_b_vld<< anc_a_vld<<anc_b_vld<< endl;

		if ((apc_a_vld && apc_b_vld) && (anc_a_vld && anc_b_vld)) {
				vld_err_tbl.push_back(*it);
				//cout << hex << "SGLEE: " << (*it).first.first << ", " << (*it).first.second << dec << endl;
		}
	}

	// Maximum possible error table size:
	full_tbl_cnt = pow(2.0, 2 * (err_msb - err_lsb + 1));

	// Reduce the full error table size considering pair-wise correlation
	// If two bit i and j are highly correlated, don't need to consider i != j,
	// So, the error table size can be reduced by 2
	for (size_t i = err_lsb; i<err_msb+1; i++) {
		// bitwise correlation
		//cout << "SGLEE i: " << i << endl;
		if (i != err_msb) {
			// for A
			if (((io.bw_apc_a>>i)&0x1) || ((io.bw_anc_a>>i)&0x1)) {
				full_tbl_cnt = full_tbl_cnt/2;
				//cout << "SGLEE cut A: " << full_tbl_cnt << endl;
			}
			// for B
			if (((io.bw_apc_b>>i)&0x1) || ((io.bw_anc_b>>i)&0x1)) {
				full_tbl_cnt = full_tbl_cnt/2;
				//cout << "SGLEE cut B: " << full_tbl_cnt << endl;
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
				full_tbl_cnt = full_tbl_cnt/2;
			}
		}

	}

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

		// figuring out min/max
		// for multiplier
		if ((((*it).first.first) & 0x80000000) == 0x80000000) {
			tmp_max = (*it).second.first.first*(int)floor((io.mean_b + sdev_scale*io.sdev_b));
			tmp_max = tmp_max + (*it).second.first.second*(int)floor((io.mean_a+sdev_scale*io.sdev_a));
			tmp_max = tmp_max + (*it).second.second;
			tmp_min = (*it).second.first.first*(int)floor((io.mean_b - sdev_scale*io.sdev_b));
			tmp_min = tmp_min + (*it).second.first.second*(int)floor((io.mean_a-sdev_scale*io.sdev_a));
			tmp_min = tmp_min - (*it).second.second;
			//cout << "sglee for btm : " << tmp_max << ", " << tmp_min << endl;
		}
		else {
			tmp_max = (*it).second.second;
			tmp_min = (*it).second.second;
			//cout << "sglee " << tmp_max << ", " << tmp_min << ", " << io.mean_b << ", " << io.sdev_b << endl;
		}

		if (tmp_max > est_err.err_max)
			est_err.err_max = tmp_max;

		if (tmp_min < est_err.err_min)
			est_err.err_min = tmp_min;

	}

	err_dist_est(est_err.err_dist, full_tbl_cnt, vld_err_tbl, io, err_msb, err_lsb);

	// mean
	est_err.err_mean = error_mean(est_err.err_dist);

	// sdev
	est_err.err_sdev = error_sdev (est_err.err_dist, est_err.err_mean);

	// total error probability
	est_err.err_total_prob = error_prob(est_err.err_dist);


}

void error_model::err_tbl_refine(int *pvld, const size_t &i, const int &bw_ac, const int &err_tbl_val) {
	if ((bw_ac>>i)&0x1) { 
		*pvld =  (*pvld)&(0x1&(!((0x1&(err_tbl_val>>i))^(0x1&(err_tbl_val>>(i+1))))));
//		cout << "sglee3: " << (err_tbl_val>>i) << ", " << (err_tbl_val>>(i+1)) << ", " << !((err_tbl_val>>i)^(err_tbl_val>>(i+1))) << endl;
	} else { 
		*pvld = *pvld;
	}
}

void error_model::err_tbl_refine2(int *pvld, const size_t &i, const vector<double> &bw_pr, const double &prob_th, const int &err_tbl_val) {
	// Pr(A=1) > threshold: ignore if A=0
	if (bw_pr[i] > prob_th) { 
		*pvld =  (*pvld)&(0x1&(err_tbl_val>>i));
	} 
	// Pr(A=1) < 1-threshold: ignore if A=1
	else if (bw_pr[i] < (1-prob_th)) {
		*pvld =  (*pvld)&(!(0x1&(err_tbl_val>>i)));
	} else { 
		*pvld = *pvld;
	}
}



void error_model::err_dist_est(map<int, double> &err_dist, const int &full_tbl_cnt, const ACETBL &vld_err_tbl, const IO_STAT &io, const size_t &err_msb, const size_t &err_lsb) {

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
			// err_prob_update(&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a);
			err_prob_update2(&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a);

			// Weak correlation: for operand A
			// err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// err_prob_update(&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b);
			err_prob_update2(&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b, io.bw_apr_b);

			// Weak correlation: for operand B
			// err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

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
			err_prob_update2 (&prob, err_lsb, err_msb, (*itr).first.first, io.bw_pr_a, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a);

			// Weak correlation: for operand A
			//err_prob_update_weakcorr(io.bw_apr_a, &prob, err_lsb, err_msb, io.bw_pr_a, (*itr).first.first, weak_corr_prob, io.bw_apc_a, io.bw_anc_a);

			// for operand B
			err_prob_update2 (&prob, err_lsb, err_msb, (*itr).first.second, io.bw_pr_b, io.bw_apc_b, io.bw_anc_b, io.bw_apr_b);

			// Weak correlation: for operand B
			//err_prob_update_weakcorr(io.bw_apr_b, &prob, err_lsb, err_msb, io.bw_pr_b, (*itr).first.second, weak_corr_prob, io.bw_apc_b, io.bw_anc_b);

			err_dist[(*itr).second.second] += prob;
			total_err_prob += prob;
		}

	}

	err_dist[0] = 1-total_err_prob;


}



void error_model::err_prob_update2 (double *prob, const size_t &err_lsb, const size_t &err_msb, const int &pattern, const vector<double> &bw_pr, const int &apc, const int &anc, const vector<double> &apr) {

	for (size_t i = err_lsb; i<err_msb+1;i++) {

		//cout << "sglee3: " << *prob << ", " << i << ", " << bw_pr[i] << hex << ", pattern: " << pattern << ", " << dec << apr[i-1] << endl;
		// always work for LSB
		if (i==err_lsb) {
			if ((pattern & 0x1) == 0) { // LSB is 0
				//cout << "sglee0 pre: " << (*prob) << ", " << bw_pr[i] << endl;
				*prob = (*prob)*2*(1-bw_pr[i]);
				//cout << "sglee0 post: " << (*prob) << ", " << bw_pr[i] << endl;
			}
			else { // LSB is 1
				//cout << "sglee1 pre: " << (*prob) << ", " << bw_pr[i] << endl;
				*prob = (*prob)*2*bw_pr[i];
				//cout << "sglee1 post: " << (*prob) << ", " << bw_pr[i] << endl;
			}
		}
		else { // i>0
//			if ((((apc>>(i-1))&0x1) == 1) || (((anc>>(i-1))&0x1) == 1)) {// correlated: either APC or ANC is 1
			if (((((apc>>(i-1))&0x1) == 1) || (((anc>>(i-1))&0x1) == 1)) ||
				((((bw_pr[i+1] > bw_pr_th) || (bw_pr[i+1] < (1-bw_pr_th)))) &&
				 ((((apc>>i)&0x1) == 1) || (((anc>>i)&0x1) == 1)))
				) {// correlated: either APC or ANC is 1
				*prob = *prob; // skip updating
			}

			// weakly correlated or uncorrelated
			else {

				//cout << "sglee: " << *prob << ", " << i << ", " << ((apc>>i)&0x1) << ", " << ((apc>>(i-1))&0x1) << endl;

				// if two bits are the same
				if (((pattern>>i)&0x1) == 0x1) {

					// '11'
					if (((pattern>>(i-1))&0x1) == 0x1) {
						//cout << "sglee11 pre: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
						*prob = (*prob)*2*(bw_pr[i]+bw_pr[i-1]+(1-apr[i-1])-1);
						//cout << "sglee11 post: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
					}
					else { // '10'
						//cout << "sglee10 pre: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
						*prob = (*prob)*2*(bw_pr[i]-bw_pr[i-1]-(1-apr[i-1])+1);
						//cout << "sglee10 post: " << i << ", " << pattern << ", " << (*prob) << ", " << bw_pr[i+1] << endl;
					}
					//cout << "sgle4: " << (*prob) << endl;
				}
				else {
					// '01'
					if (((pattern>>(i-1))&0x1) == 0x1) {
						//cout << "sglee01 pre: " << i << ", " << pattern << ", " << (*prob) << endl;
						*prob = (*prob)*2*(bw_pr[i-1]-bw_pr[i]-(1-apr[i-1])+1);
						//cout << "sglee01 post: " << i << ", " << pattern << ", " << (*prob) << endl;
					}
					else { // '00'
						//cout << "sglee00 pre: " << i << ", " << pattern << ", " << (*prob) << endl;
						*prob = (*prob)*2*((1-apr[i-1])-bw_pr[i]-bw_pr[i-1]+1);
						//cout << "sglee00 post: " << i << ", " << pattern << ", " << (*prob) << endl;
					}

				}
			} //if ((((apc>>(i-1))&0x1) == 1) || (((anc>>(i-1))&0x1) == 1))

		} // if (i==err_lsb)
	} // for (size_t i = err_lsb; i<err_msb+1;i++)
}


//double error_model::error_mean_cal(const int &sum_err, const size_t &size) {
//	return (double) sum_err/size;
//}

double error_model::error_mean(const map<int,double> &err_dist)  {
	double mean = 0;
	for (map<int,double>::const_iterator itr = err_dist.begin(); itr != err_dist.end(); itr++) {
		mean += (*itr).first * (*itr).second;
	}
	return mean;
}

double error_model::error_sdev (const map<int,double> &err_dist, const double &mean) {
	double sdev = 0;
	for (map<int,double>::const_iterator itr = err_dist.begin(); itr != err_dist.end(); itr++) {
		sdev += ((*itr).first-mean)*((*itr).first-mean)*(*itr).second;
	}

	return sqrt(sdev);
}

//double error_model::error_sdev(vector<int>::iterator itr_begin, vector<int>::iterator itr_end, const double &err_mean, const size_t &size) {
//	// sdev
//	double E = 0;
//	for (vector<int>::iterator itr = itr_begin; itr != itr_end; itr++) {
//		E += (*itr - cal_err.err_mean)*(*itr - cal_err.err_mean);
//	}
//	return sqrt(E/size);
//
//}

double error_model::error_prob(const map<int,double> &err_dist) {
	double total_err_prob = 0;
	for (map<int,double>::const_iterator itr = err_dist.begin(); itr != err_dist.end(); itr++) {
		if ((*itr).first != 0) {
			total_err_prob += (*itr).second;
		}
	}

	return total_err_prob;
}

void error_model::print_err_stat(ERR_STAT &err, const size_t mode) {
	cout << dec << "err_mean:" << err.err_mean << ", err_sdev:" << err.err_sdev << endl;
	cout << "err_max:" << err.err_max << " err_min:" << err.err_min << endl;
	cout << "err_prob:" << err.err_total_prob << endl;

	if (mode == 1) { // verbose print
		for (map<int,double>::iterator it = err.err_dist.begin(); it != err.err_dist.end(); it++) {
			cout << "error distribution:" << (*it).first << ", " << (*it).second << endl;
		}
	}
}

void error_model::print_vld_err_tbl(void) {
	for (ACETBL::iterator it = vld_err_tbl.begin(); it != vld_err_tbl.end(); it++) {
		cout << hex << "Pattern A: " << (*it).first.first << ", Pattern B: " << (*it).first.second << dec << ", Error: " << (*it).second.first.first << "B, " << (*it).second.first.second << "A, " << dec << (*it).second.second << endl;
	}
}
