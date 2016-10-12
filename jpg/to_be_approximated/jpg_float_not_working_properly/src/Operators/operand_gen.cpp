#include <iostream>
#include <complex>
#include <vector>
#include <time.h>

#include "operand_gen.h"

using namespace std;

int operand_gen(size_t Nt, 
				 ACVEC &a, ACVEC &b) {

	ACVEC::iterator itb = b.begin();
	for (ACVEC::iterator itr = a.begin(); itr != a.end(); itr++) {
		
		*itr = (ACINT)(double(RAND_MAX-2*rand()))/(double((8*sizeof(int)-1)/Nt));

		*itb = (ACINT)(double(RAND_MAX-2*rand()))/(double((8*sizeof(int)-1)/Nt));
		++itb;
	}

	return 1;
};

int operand_gen(size_t Nt,  
			    float mean_a, float var_a, float mean_b, float var_b,
				ACVEC &a, ACVEC &b, int rand_seed) {
	double std_a = sqrt(var_a);	// standard deviation
	double std_b = sqrt(var_b); // standard deviation
	default_random_engine gen(rand_seed);
	
	normal_distribution<double> dist_a(mean_a, std_a);
	normal_distribution<double> dist_b(mean_b, std_b);
	ACVEC::iterator ita = a.begin();
	ACVEC::iterator itb = b.begin();

	for (int i = 0; i != a.size();i++) {
		ACINT raw_rand_a = (ACINT) dist_a(gen);
		if ((raw_rand_a > -pow(2,Nt-1)+1) && (raw_rand_a < pow(2,Nt-1)-1)) {
			*ita = raw_rand_a;
			++ita;
		} else { i--; }
	}
	
	for (int i = 0; i != a.size();i++) {
		ACINT raw_rand_b = (ACINT) dist_b(gen);
		if ((raw_rand_b > -pow(2,Nt-1)+1) && (raw_rand_b < pow(2,Nt-1)-1)) {
			*itb = raw_rand_b;
			++itb;
		} else { i--; }
	}

	return 1;
};

int operand_gen(size_t Nt,
	float mean_a, float var_a, float mean_b, float var_b,
	ACVEC &a, ACVEC &b, int rand_seed, int mode) {
	double std_a = sqrt(var_a);	// standard deviation
	double std_b = sqrt(var_b); // standard deviation
	default_random_engine gen(rand_seed);

	if (mode == 1) { // Gaussian
		normal_distribution<double> dist_a(mean_a, std_a);
		normal_distribution<double> dist_b(mean_b, std_b);
		ACVEC::iterator ita = a.begin();
		ACVEC::iterator itb = b.begin();

		for (int i = 0; i != a.size(); i++) {
			ACINT raw_rand_a = (ACINT)dist_a(gen);
			if ((raw_rand_a > -pow(2, Nt - 1) + 1) && (raw_rand_a < pow(2, Nt - 1) - 1)) {
				*ita = raw_rand_a;
				++ita;
			}
			else { i--; }
		}

		for (int i = 0; i != a.size(); i++) {
			ACINT raw_rand_b = (ACINT)dist_b(gen);
			if ((raw_rand_b > -pow(2, Nt - 1) + 1) && (raw_rand_b < pow(2, Nt - 1) - 1)) {
				*itb = raw_rand_b;
				++itb;
			}
			else { i--; }
		}
	} // mode == 1
	else if (mode == 0) { // Uniform
		int max_a = sqrt(3)* std_a;
		int max_b = sqrt(3)* std_b;
		uniform_int_distribution<int> dist_a(-max_a + mean_a, max_a + mean_a);
		uniform_int_distribution<int> dist_b(-max_b + mean_b, max_b + mean_b);
		ACVEC::iterator ita = a.begin();
		ACVEC::iterator itb = b.begin();

		for (int i = 0; i != a.size(); i++) {
			ACINT raw_rand_a = (ACINT)dist_a(gen);
			if ((raw_rand_a > -pow(2, Nt - 1) + 1) && (raw_rand_a < pow(2, Nt - 1) - 1)) {
				*ita = raw_rand_a;
				++ita;
			}
			else { i--; }
		}

		for (int i = 0; i != a.size(); i++) {
			ACINT raw_rand_b = (ACINT)dist_b(gen);
			if ((raw_rand_b > -pow(2, Nt - 1) + 1) && (raw_rand_b < pow(2, Nt - 1) - 1)) {
				*itb = raw_rand_b;
				++itb;
			}
			else { i--; }
		}
	} // mode == 0

	return 1;
};


int operand_gen(size_t Nt,  
				int bw_apc_a, int bw_anc_a, int bw_apc_b, int bw_anc_b, 
				int bw_cpc, int bw_cnc, vector<float> bw_pr_a, vector<float> bw_pr_b,
				ACVEC &a, ACVEC &b) {
	ACVEC::iterator itb = b.begin();
	for (ACVEC::iterator itr = a.begin(); itr != a.end(); itr++) {
		*itr = RAND_MAX-2*rand();

		*itb = RAND_MAX-2*rand();
		++itb;
	}

	return 1;
};

void print_operand(ACVEC &a, ACVEC &b) {
	ACVEC::iterator ita = a.begin();
	for (vector<int>::iterator it = b.begin(); it != b.end(); it++) {
		cout << "Operand A, B: " << endl;
		cout << hex << "a : " << *ita << dec << endl;
		cout << hex << "b : " << *it << dec << endl;
		++ita;
	}
}