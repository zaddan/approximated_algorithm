// This is a project for approximate computing. version 001
// Author : Seogoo Lee
// History :
// Initial creation : 2013.12.17
// 

#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <time.h>

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

using namespace std;

int main_chernoff(void) {

	///////////////////////////////////////////////////////
	// Input generation
	///////////////////////////////////////////////////////

	size_t Nt = 16;
	size_t size = 100000;
	int gen_success;
	ACVEC x1_vec(size);
	ACVEC x2_vec(size);
	ACVEC x3_vec(size);
	ACVEC x4_vec(size);

	float mean_x1 = 0;
	float var_x1 = 1000;
	float mean_x2 = 0;
	float var_x2 = 1000;
	float mean_x3 = 0;
	float var_x3 = 8000;
	float mean_x4 = 0;
	float var_x4 = 8000;

	// decision variables
	size_t Nia_m1 = 5;
	size_t Nia_m2 = 5;
	size_t Nia_a1 = 5;

	size_t Nia_start = 1;
	size_t Nia_end = 8;

	size_t Nia_m2_start = 1;
	size_t Nia_m2_end = 8;

	size_t Nia_a1_start = 1;
	size_t Nia_a1_end = 8;


	gen_success = operand_gen(Nt, mean_x1, var_x1, mean_x2, var_x2, x1_vec, x2_vec, 1); // gaussian genenrator
	gen_success = operand_gen(Nt, mean_x3, var_x3, mean_x4, var_x4, x3_vec, x4_vec, 3); // gaussian genenrator

	/*
	ACVEC::iterator it2 = x2_vec.begin();
	ACVEC::iterator it3 = x3_vec.begin();
	ACVEC::iterator it4 = x4_vec.begin();
	for (ACVEC::iterator it1 = x1_vec.begin(); it1 != x1_vec.end(); it1++) {
	cout << "sglee x1: " << *it1 << ", x3: " << *it3 << endl;
	cout << "sglee x2: " << *it2 << ", x4: " << *it4 << endl;
	it2++;
	it3++;
	it4++;
	}
	*/

	///////////////////////////////////////////////////////
	// Input statistics information extraction
	///////////////////////////////////////////////////////

	double corr_th = 0.95;

	// second argument: 1 : for adder, 2: for multiplier
	io_stat in_corr_m1(Nt, 2, x1_vec, x2_vec, corr_th, corr_th);
	io_stat in_corr_m2(Nt, 2, x3_vec, x4_vec, corr_th, corr_th);

	cout << "Sglee: Input statistics extraction " << endl;

	///////////////////////////////////////////////////////
	// Initial accurate simulation 
	// to get intermediate stages' statistical information
	///////////////////////////////////////////////////////

	ACVEC m1_vec;
	ACVEC m2_vec;
	ACVEC a1_vec;
	int a1_cal;
	int m1_cal;
	int m2_cal;
	ACVEC::iterator it_x2 = x2_vec.begin();
	ACVEC::iterator it_x3 = x3_vec.begin();
	ACVEC::iterator it_x4 = x4_vec.begin();
	for (ACVEC::iterator it_x1 = x1_vec.begin(); it_x1 != x1_vec.end(); it_x1++) {
		m1_cal = (*it_x1)*(*it_x2);
		m2_cal = (*it_x3)*(*it_x4);
		a1_cal = m1_cal + m2_cal;
		m1_vec.push_back(m1_cal);
		m2_vec.push_back(m2_cal);
		a1_vec.push_back(a1_cal);
		it_x2++;
		it_x3++;
		it_x4++;

	}

	/*
	ACVEC::iterator it2 = m1_vec.begin();
	ACVEC::iterator it3 = a1_vec.begin();
	for (ACVEC::iterator it1 = m2_vec.begin(); it1 != m2_vec.end(); it1++) {
	cout << "sglee m1: " << *it1 << ", m2: " << *it2 << ", a1: " << *it3 << endl;
	it2++;
	it3++;
	}
	*/

	cout << "Sglee: Initial accurate simulation" << endl;

	///////////////////////////////////////////////////////
	// Statistical infromation extraction for adder
	///////////////////////////////////////////////////////

	io_stat in_corr_a1(2 * Nt, 1, m1_vec, m2_vec, corr_th, corr_th);
	in_corr_a1.print_io_stat();

	cout << "Sglee: Statistical information extraction for adder" << endl;

	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	// Main loop to change Nia
	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
//	for (size_t Nia_a1 = Nia_a1_start; Nia_a1 < Nia_a1_end + 1; Nia_a1 = Nia_a1 + 2) {

//		for (size_t Nia_m2 = Nia_m2_start; Nia_m2 < Nia_m2_end + 1; Nia_m2 = Nia_m2 + 2) {

//			for (size_t Nia = Nia_start; Nia < Nia_end + 1; Nia = Nia + 2) {


				cout << "////////////////////////////////////////////////////////" << endl;
				cout << "// Start of Nia_A = " << Nia_a1 << ", " << "Nia_m: " << Nia_m1 << "Nia_m2: " << Nia_m2 << " calculation" << endl;
				cout << "////////////////////////////////////////////////////////" << endl;

				///////////////////////////////////////////////////////
				// Error estimation for each of arithmetic unit
				///////////////////////////////////////////////////////

				bam m1(Nt, Nia_m1, Nia_m1, Nia_m1 - 1, 0, 1);
				bam m2(Nt, Nia_m2, Nia_m2, Nia_m2 - 1, 0, 1);
				loa a1(2 * Nt, Nia_a1, Nia_a1 - 1, 0, 1);

				error_model err_m1;
				error_model err_m2;
				error_model err_a1;

				err_m1.error_est(m1.err_tbl, in_corr_m1.io, Nia_m1 - 1, 0, 1);
				err_m2.error_est(m2.err_tbl, in_corr_m2.io, Nia_m2 - 1, 0, 1);
				err_a1.error_est(a1.err_tbl, in_corr_a1.io, Nia_a1 - 1, 0, 1);

				cout << "Sglee: Error estimation for each of arithmetic unit" << endl;

				///////////////////////////////////////////////////////
				// Simulation
				///////////////////////////////////////////////////////

				ACVEC m1a_vec;
				ACVEC m2a_vec;
				ACVEC a1a_vec;
				int a1a_cal;
				ACVEC a1a2_vec;
				int a1a2_cal;
				int m1a_cal;
				int m2a_cal;

				ACVEC::iterator ita_x2 = x2_vec.begin();
				ACVEC::iterator ita_x3 = x3_vec.begin();
				ACVEC::iterator ita_x4 = x4_vec.begin();

				ACVEC::iterator ita_m1 = m1_vec.begin();
				ACVEC::iterator ita_m2 = m2_vec.begin();

				for (ACVEC::iterator ita_x1 = x1_vec.begin(); ita_x1 != x1_vec.end(); ita_x1++) {

					//cout << "sglee x1: " << *ita_x1 << ", x2: " << *ita_x2 << endl;

					m1a_cal = m1.calc((*ita_x1), (*ita_x2));
					m2a_cal = m2.calc((*ita_x3), (*ita_x4));

					//cout << "sglee m1&2: " << m1a_cal << ", " << m2a_cal << endl;

					a1a_cal = a1.calc(m1a_cal, m2a_cal);

					a1a2_cal = a1.calc(*ita_m1, *ita_m2);
					a1a2_vec.push_back(a1a2_cal);

					m1a_vec.push_back(m1a_cal);
					m2a_vec.push_back(m2a_cal);
					a1a_vec.push_back(a1a_cal);
					ita_x2++;
					ita_x3++;
					ita_x4++;

					ita_m1++;
					ita_m2++;
				}

				/*
				ACVEC::iterator it2 = m2a_vec.begin();
				for (ACVEC::iterator it1 = m2_vec.begin(); it1 != m2_vec.end(); it1++) {
				cout << "sglee m2 apr: " << *it1 << ", acr" << *it2 << endl;
				it2++;
				}
				*/

				err_m1.error_cal(m1_vec, m1a_vec, 2 * Nt - 1, 1);
				err_m2.error_cal(m2_vec, m2a_vec, 2 * Nt - 1, 1);
				err_a1.error_cal(a1_vec, a1a2_vec, 2 * Nt - 1, 1);


				cout << "Sglee: Simulation" << endl;

				cout << "N1 mean(sim): " << err_m1.cal_err.err_mean << ", N1 mean(est): " << err_m1.est_err.err_mean << endl;
				cout << "N2 mean(sim): " << err_m2.cal_err.err_mean << ", N2 mean(est): " << err_m2.est_err.err_mean << endl;
				cout << "N3 mean(sim): " << err_a1.cal_err.err_mean << ", N3 mean(est): " << err_a1.est_err.err_mean << endl;
				cout << "N1 sdev(sim): " << err_m1.cal_err.err_sdev << ", N1 sdev(est): " << err_m1.est_err.err_sdev << endl;
				cout << "N2 sdev(sim): " << err_m2.cal_err.err_sdev << ", N2 sdev(est): " << err_m2.est_err.err_sdev << endl;
				cout << "N3 sdev(sim): " << err_a1.cal_err.err_sdev << ", N3 sdev(est): " << err_a1.est_err.err_sdev << endl;

				///////////////////////////////////////////////////////
				// Final error metric calculation
				///////////////////////////////////////////////////////

				// min/max error
				// from simulation
				error_model sim_err;
				cout << "sglee: " << a1_vec.size() << ", " << a1a_vec.size() << endl;

				sim_err.error_cal(a1_vec, a1a_vec, 2 * Nt - 1, 0);
				cout << "Simulated Error Max: " << sim_err.cal_err.err_max << endl;
				cout << "Simulated Error Min: " << sim_err.cal_err.err_min << endl;


				// from estimation
				int est_err_max = err_m1.est_err.err_max + err_m2.est_err.err_max + err_a1.est_err.err_max;
				int est_err_min = err_m1.est_err.err_min + err_m2.est_err.err_min + err_a1.est_err.err_min;
				cout << "Estimated Error Max: " << est_err_max << endl;
				cout << "Estimated Error Min: " << est_err_min << endl;

				// estimation error probability
				double out_prob = 0;
				for (map<int, double>::iterator it = sim_err.cal_err.err_dist.begin(); it != sim_err.cal_err.err_dist.end(); it++) {
					if (((*it).first < est_err_min) || ((*it).first > est_err_max)) {
						out_prob += (*it).second;
					}
				}

				cout << "Outage probability: " << out_prob << endl;

				///////////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////////
				// Close the main loop
				///////////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////////

				// SNR
				ACVEC::iterator ita = a1_vec.begin();
				double err;
				double sim_sig_pwr = 0;
				double sim_noise_pwr = 0;
				double sim_avg_snr = 0;
				for (ACVEC::iterator iti = a1a_vec.begin(); iti != a1a_vec.end(); iti++) {
					err = (double)(*ita - *iti);
					sim_sig_pwr += (*ita)*(*ita);
					sim_noise_pwr += (err*err);

					ita++;
				}

				sim_sig_pwr = sim_sig_pwr / size;
				sim_noise_pwr = sim_noise_pwr / size;

				sim_avg_snr = sim_sig_pwr / sim_noise_pwr;

				double sig_pwr = var_x1*var_x2 + var_x3*var_x4;
				double noise_pwr = (err_m1.est_err.err_sdev)*(err_m1.est_err.err_sdev) +
					(err_m2.est_err.err_sdev)*(err_m2.est_err.err_sdev) +
					(err_a1.est_err.err_sdev)*(err_a1.est_err.err_sdev);// +
				//						(err_m1.est_err.err_mean)*(err_m1.est_err.err_mean) +
				//						(err_m2.est_err.err_mean)*(err_m2.est_err.err_mean) +
				//						(err_a1.est_err.err_mean)*(err_a1.est_err.err_mean);

				double est_avg_snr = sig_pwr / noise_pwr;

				cout << "Simulated AVG_SNR: " << 20 * log10(sim_avg_snr) << ", Estimated AVG_SNR: " << 20 * log10(est_avg_snr) << endl;

//			}
//		}
//	}


	// Chernoff bound
	//	double max_th = est_err_max;
	//	double min_th = est_err_min;
	double max_th = 7000;
	double min_th = -7000;
	double t;

	// estimation error probability
	out_prob = 0;
	for (map<int,double>::iterator it = sim_err.cal_err.err_dist.begin(); it != sim_err.cal_err.err_dist.end(); it++) {
		if ((*it).first < min_th) {
			out_prob += (*it).second;
		}
	}

	cout << "Simulated max outage probability: " << out_prob << endl;

	out_prob = 0;
	for (map<int,double>::iterator it = sim_err.cal_err.err_dist.begin(); it != sim_err.cal_err.err_dist.end(); it++) {
		if ((*it).first > max_th) {
			out_prob += (*it).second;
		}
	}

	cout << "Simulated min outage probability: " << out_prob << endl;


	for (t=0.001; t<0.004;t=t+0.00005) {

		double sim_mgf_m1p = mgf_cal(err_m1.est_err.err_dist, t);
		double sim_mgf_m2p = mgf_cal(err_m2.est_err.err_dist, t);
		double sim_mgf_a1p = mgf_cal(err_a1.est_err.err_dist, t);

		double sim_mgf_m1m = mgf_cal(err_m1.est_err.err_dist, -1 * t);
		double sim_mgf_m2m = mgf_cal(err_m2.est_err.err_dist, -1 * t);
		double sim_mgf_a1m = mgf_cal(err_a1.est_err.err_dist, -1 * t);

		double sim_max_bound = sim_mgf_m1p*sim_mgf_m2p*sim_mgf_a1p*exp(-1*t*max_th);
		double sim_min_bound = sim_mgf_m1m*sim_mgf_m2m*sim_mgf_a1m*exp(t*min_th);

		cout <<"Max bound: " << sim_max_bound << ", Min bound: " << sim_min_bound << ", t: " << t << endl;
	}



	getchar();
	return 1;

}
