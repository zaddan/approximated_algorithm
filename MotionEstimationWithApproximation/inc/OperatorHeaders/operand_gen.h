#ifndef _OPERAND
#define _OPERAND

#include <iostream>
#include <complex>
#include <vector>
#include <random>

#include "ac_types.h"

using namespace std;

// for uniform distribution
int operand_gen(size_t Nt, 
				ACVEC &a, ACVEC &b); 

// for Gaussian distribution
int operand_gen(size_t Nt, 
				float mean_a, float var_a, float mean_b, float var_b,
				ACVEC &a, ACVEC &b, int rand_seed);

// for Gaussian distribution
int operand_gen(size_t Nt,
	float mean_a, float var_a, float mean_b, float var_b,
	ACVEC &a, ACVEC &b, int rand_seed, int mode);

// for correlation based 
int operand_gen(size_t Nt, 
				int bw_apc_a, int bw_anc_a, int bw_apc_b, int bw_anc_b, 
				int bw_cpc, int bw_cnc, vector<float> bw_pr_a, vector<float> bw_pr_b,
				ACVEC &a, ACVEC &b); 

// print operands
void print_operand(ACVEC &a, ACVEC &b);

#endif