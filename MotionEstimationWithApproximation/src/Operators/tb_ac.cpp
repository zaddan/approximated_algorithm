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
#include "bta.h"
#include "btm.h"
#include "operand_gen.h"
#include "io_stat.h"
#include "error_model.h"

int printVectorValues(ACVEC inputVec) {
    for (ACVEC::iterator ita = inputVec.begin(); ita != inputVec.end(); ita++) {
        cout<<*ita<<endl;
    }
}
using namespace std;


int main2(void) {
	ACINT a = 46;
	ACINT b = 38;
	size_t Nt = 16;//total number of bits
	size_t Nia = 6;//total number of apx bits starting from 0
	size_t msb = Nia-1;
	size_t lsb = 0;
//	size_t K = 4;
	size_t hbl = Nia; //some type of apx 
	size_t vbl = hbl; // some type of apx
	size_t op_type = 2; // '1' for adder, '2' for multiplier

	//eta1 y0(Nt, Nia, msb, lsb, 1);
	//bta y0(Nt, Nia, msb, lsb, 1);
	//eta2 x1(Nt, K, msb, lsb);
	//loa y0(Nt, Nia, msb, lsb, 1);
	bam y0 (Nt,  hbl, vbl,  hbl-1,  0, 1);
	//etm y0(Nt, Nia, msb, lsb);
	//btm y0 (Nt,  hbl, vbl,  hbl-1,  0);

	size_t size = 10;
	int success = 0;
	ACVEC a_vec(size);
	ACVEC b_vec(size);
	size_t in_mode=1;

	float mean_a = 0;
	float var_a = 1024;
	float mean_b = 0;
	float var_b = var_a;
	double corr_th = 0.99;
	
	if (in_mode == 0) { // uniform distribution 
		success =  operand_gen(Nt, a_vec, b_vec); // uniform generator
	} else if (in_mode == 1) { // gaussian distribution
		success = operand_gen(Nt, mean_a, var_a, mean_b, var_b, a_vec, b_vec, 1, 1); // gaussian generator
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
	//cout << "BAM result:" << y0.calc(a,b) << ", correct one: " << a*b << endl;
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

		ACVEC::iterator itb = b_vec.begin();
		int cal;
		int cal_ref;
		for (ACVEC::iterator ita = a_vec.begin(); ita != a_vec.end(); ita++) {
			cal = y0.calc(*ita, *itb);
#ifdef DEBUG
			cout << hex << "output: " << cal << ", correct: " << *ita + *itb <<  dec << endl;
#endif
			cal_ref = y0.calc_ref(*ita, *itb);
//			c.push_back(cal);
			c.push_back(cal_ref);
            c_ori.push_back(cal_ref); //ori stands for original
			++itb;
		}
        printVectorValues(c_ori);
        printVectorValues(c);
        return 1;
}
