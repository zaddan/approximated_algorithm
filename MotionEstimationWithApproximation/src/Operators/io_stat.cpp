#include <iostream>
#include <complex>
#include <vector>
#include <numeric>

#include "io_stat.h"

using namespace std;

io_stat::io_stat(void) {}

io_stat::~io_stat(void) {}

io_stat::io_stat (const size_t &Nt, const size_t &mode, const ACVEC &a, const ACVEC &b, const double &th_ac, const double &th_cc) {
	this->Nt = Nt;
	this->mode = mode;

	// initialize 
	io.mean_a = 0;
	io.sdev_a = 0;		// variance of a
	io.mean_b = 0;		// mean of b
	io.sdev_b = 0;		// variance of b
	io.bw_apc_a = 0;		// bit-wise positive auto correlation of a
	io.bw_anc_a = 0;		// bit-wise negative auto correlation of a
	io.bw_apc_b = 0;		// bit-wise positive auto correlation of b
	io.bw_anc_b = 0;		// bit-wise negative auto correlation of b
	io.bw_cpc = 0;		// bit-wise positive cross correlation of a & b
	io.bw_cnc = 0;		// bit-wise negative cross correlation of a & b
	//bw_pr_a;		// bit-wise '0' probability of a
	//bw_pr_b;		// bit-wise '1' probability of b
	
	for (size_t i=0;i<Nt;i++) {
		io.bw_pr_a.push_back(0);
		io.bw_pr_b.push_back(0);

		io.bw_apr_a.push_back(0);
		io.bw_apr_b.push_back(0);
	}

	mean_var_cal (a, io.mean_a, io.sdev_a);
	mean_var_cal (b, io.mean_b, io.sdev_b);
	
	if (mode == 1) {
		bw_acor_cal (a, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac); 
		bw_acor_cal (b, io.bw_apc_b, io.bw_anc_b, io.bw_apr_b, th_ac); 
		bw_pr_cal (a, io.bw_pr_a); 
		bw_pr_cal (b, io.bw_pr_b); 
		bw_ccor_cal (a, b, io.bw_cpc, io.bw_cnc, th_cc); 
	}
	else if (mode == 2) {
		ACVEC a_abs;
		for (ACVEC::const_iterator itr = a.begin(); itr != a.end(); itr++) {
			a_abs.push_back(abs(*itr));
			//cout << "sglee: " << *itr << ", " << abs(*itr) << endl;
		}
		ACVEC b_abs;
		for (ACVEC::const_iterator itr = b.begin(); itr != b.end(); itr++) {
			b_abs.push_back(abs(*itr));
			//cout << "sglee: " << *itr << ", " << abs(*itr) << endl;
		}
		bw_acor_cal (a_abs, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac); 
		bw_acor_cal (b_abs, io.bw_apc_b, io.bw_anc_b, io.bw_apr_b, th_ac);
		bw_pr_cal (a_abs, io.bw_pr_a); 
		bw_pr_cal (b_abs, io.bw_pr_b);
		bw_ccor_cal (a_abs, b, io.bw_cpc, io.bw_cnc, th_cc); 

	}
}

io_stat::io_stat (const size_t &Nt, const size_t &mode) {
	this->Nt = Nt;
	this->mode = mode;
}

void io_stat::gen_stat (const ACVEC &a, const ACVEC &b, const double &th_ac, const double &th_cc) {

	// initialize
	io.mean_a = 0;
	io.sdev_a = 0;		// variance of a
	io.mean_b = 0;		// mean of b
	io.sdev_b = 0;		// variance of b
	io.bw_apc_a = 0;		// bit-wise positive auto correlation of a
	io.bw_anc_a = 0;		// bit-wise negative auto correlation of a
	io.bw_apc_b = 0;		// bit-wise positive auto correlation of b
	io.bw_anc_b = 0;		// bit-wise negative auto correlation of b
	io.bw_cpc = 0;		// bit-wise positive cross correlation of a & b
	io.bw_cnc = 0;		// bit-wise negative cross correlation of a & b
	//bw_pr_a;		// bit-wise '0' probability of a
	//bw_pr_b;		// bit-wise '1' probability of b

	for (size_t i=0;i<Nt;i++) {
		io.bw_pr_a.push_back(0);
		io.bw_pr_b.push_back(0);

		io.bw_apr_a.push_back(0);
		io.bw_apr_b.push_back(0);
	}

	mean_var_cal (a, io.mean_a, io.sdev_a);
	mean_var_cal (b, io.mean_b, io.sdev_b);

	if (mode == 1) {
		bw_acor_cal (a, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac);
		bw_acor_cal (b, io.bw_apc_b, io.bw_anc_b, io.bw_apr_b, th_ac);
		bw_pr_cal (a, io.bw_pr_a);
		bw_pr_cal (b, io.bw_pr_b);
		bw_ccor_cal (a, b, io.bw_cpc, io.bw_cnc, th_cc);
	}
	else if (mode == 2) {
		ACVEC a_abs;
		for (ACVEC::const_iterator itr = a.begin(); itr != a.end(); itr++) {
			a_abs.push_back(abs(*itr));
			//cout << "sglee: " << *itr << ", " << abs(*itr) << endl;
		}
		ACVEC b_abs;
		for (ACVEC::const_iterator itr = b.begin(); itr != b.end(); itr++) {
			b_abs.push_back(abs(*itr));
			//cout << "sglee: " << *itr << ", " << abs(*itr) << endl;
		}
		bw_acor_cal (a_abs, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac);
		bw_acor_cal (b_abs, io.bw_apc_b, io.bw_anc_b, io.bw_apr_b, th_ac);
		bw_pr_cal (a_abs, io.bw_pr_a);
		bw_pr_cal (b_abs, io.bw_pr_b);
		bw_ccor_cal (a_abs, b, io.bw_cpc, io.bw_cnc, th_cc);

	}
}

io_stat::io_stat (const size_t &Nt, const size_t &mode, const ACVEC &a, const double &th_ac) {
	this->Nt = Nt;
	this->mode = mode;

	// initialize 
	io.mean_a = 0;
	io.sdev_a = 0;		// variance of a
	io.mean_b = 0;		// mean of b
	io.sdev_b = 0;		// variance of b
	io.bw_apc_a = 0;		// bit-wise positive auto correlation of a
	io.bw_anc_a = 0;		// bit-wise negative auto correlation of a
	io.bw_apc_b = 0;		// bit-wise positive auto correlation of b
	io.bw_anc_b = 0;		// bit-wise negative auto correlation of b
	io.bw_cpc = 0;		// bit-wise positive cross correlation of a & b
	io.bw_cnc = 0;		// bit-wise negative cross correlation of a & b

	
	for (size_t i=0;i<Nt;i++) {
		io.bw_pr_a.push_back(0);
		io.bw_apr_a.push_back(0);
	}

	mean_var_cal (a, io.mean_a, io.sdev_a);
		
	if (mode == 1) {
		bw_acor_cal (a, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac); 
		bw_pr_cal (a, io.bw_pr_a); 
	}
	else if (mode == 2) {
		ACVEC a_abs;
		for (ACVEC::const_iterator itr = a.begin(); itr != a.end(); itr++) {
			a_abs.push_back(abs(*itr));
//			cout << "sglee: " << *itr << ", " << abs(*itr) << endl;
		}
		bw_acor_cal (a_abs, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac); 
		bw_pr_cal (a_abs, io.bw_pr_a); 
	}

}

void io_stat::gen_stat (const ACVEC &a, const double &th_ac) {

	// initialize
	io.mean_a = 0;
	io.sdev_a = 0;		// variance of a
	io.mean_b = 0;		// mean of b
	io.sdev_b = 0;		// variance of b
	io.bw_apc_a = 0;		// bit-wise positive auto correlation of a
	io.bw_anc_a = 0;		// bit-wise negative auto correlation of a
	io.bw_apc_b = 0;		// bit-wise positive auto correlation of b
	io.bw_anc_b = 0;		// bit-wise negative auto correlation of b
	io.bw_cpc = 0;		// bit-wise positive cross correlation of a & b
	io.bw_cnc = 0;		// bit-wise negative cross correlation of a & b


	for (size_t i=0;i<Nt;i++) {
		io.bw_pr_a.push_back(0);
		io.bw_apr_a.push_back(0);
	}

	mean_var_cal (a, io.mean_a, io.sdev_a);

	if (mode == 1) {
		bw_acor_cal (a, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac);
		bw_pr_cal (a, io.bw_pr_a);
	}
	else if (mode == 2) {
		ACVEC a_abs;
		for (ACVEC::const_iterator itr = a.begin(); itr != a.end(); itr++) {
			a_abs.push_back(abs(*itr));
		//	cout << "sglee: " << *itr << ", " << abs(*itr) << endl;
		}
		bw_acor_cal (a_abs, io.bw_apc_a, io.bw_anc_a, io.bw_apr_a, th_ac);
		bw_pr_cal (a_abs, io.bw_pr_a);
	}
}

void io_stat::copy_b_stat (double mean_b, double sdev_b, int bw_apc_b, int bw_anc_b, vector<double> bw_pr_b, vector<double> bw_apr_b) {
	this->io.mean_b = mean_b;
	this->io.sdev_b = sdev_b;
	this->io.bw_apc_b = bw_apc_b;
	this->io.bw_anc_b = bw_anc_b;
	this->io.bw_pr_b = bw_pr_b;
	this->io.bw_apr_b = bw_apr_b;

}

void io_stat::mean_var_cal(const ACVEC &a, double &mean, double &sdev) {
	// mean
	double sum = accumulate(a.begin(), a.end(), 0.0);
	mean = sum / a.size();

	double E=0;
    for(size_t i=0;i<a.size();i++)
        E+=(a[i] - mean)*(a[i] - mean);
    sdev = sqrt(E/a.size());

}

void io_stat::bw_acor_cal (const ACVEC &a, int &bw_apc, int &bw_anc, vector<double> &bw_ac_cnt, const double &th_ac) {
	//vector<double> bw_ac_cnt(Nt-1,0);

	//vector<double> bw_ac0_cnt(Nt-1,0);
	//vector<double> bw_ac1_cnt(Nt-1,0);
	//vector<double> bw_uc0_cnt(Nt-1,0);
	//vector<double> bw_uc1_cnt(Nt-1,0);
	for (ACVEC::const_iterator itr = a.begin(); itr != a.end(); itr++) {
		int bw_xor;
		bw_xor = *itr ^ (*itr >> 1);
		for (size_t i=0;i<Nt-1;i++) {
//			cout << "SGLEE SIZE2: " << a.size() << Nt << endl;
			bw_ac_cnt[i] = ((bw_xor&0x1) == 0x1) ? bw_ac_cnt[i]+1: bw_ac_cnt[i];

#ifdef DEBUG_IS
			// sglee for test
			bw_ac0_cnt[i] = (((bw_xor&0x1) == 0x1) && ((((*itr)>>i)&0x1) == 0x0)) ? bw_ac0_cnt[i]+1: bw_ac0_cnt[i];
			bw_ac1_cnt[i] = (((bw_xor&0x1) == 0x1) && ((((*itr)>>i)&0x1) == 0x1)) ? bw_ac1_cnt[i]+1: bw_ac1_cnt[i];
			bw_uc0_cnt[i] = (((bw_xor&0x1) == 0x0) && ((((*itr)>>i)&0x1) == 0x0)) ? bw_uc0_cnt[i]+1: bw_uc0_cnt[i];
			bw_uc1_cnt[i] = (((bw_xor&0x1) == 0x0) && ((((*itr)>>i)&0x1) == 0x1)) ? bw_uc1_cnt[i]+1: bw_uc1_cnt[i];
#endif
//			cout << "SGLEE SIZE3: " << a.size() << Nt << endl;

			bw_xor = bw_xor>>1;

		}
	}	

	int j=0;
	for (vector<double>::iterator itr = bw_ac_cnt.begin(); itr != bw_ac_cnt.end(); itr++) {
		*itr = *itr/a.size();
		cout <<"sglee: bit[" << j << ":" << j+1 << "] corr: " << 1-(*itr) << endl;
		j=j+1;
	}

#ifdef DEBUG_IS
	int j=0;
	vector<double>::iterator it_ac0 = bw_ac0_cnt.begin();
	vector<double>::iterator it_ac1 = bw_ac1_cnt.begin();
	vector<double>::iterator it_uc0 = bw_uc0_cnt.begin();
	vector<double>::iterator it_uc1 = bw_uc1_cnt.begin();

	for (vector<double>::iterator itr = bw_ac_cnt.begin(); itr != bw_ac_cnt.end(); itr++) {

		*it_ac0 = *it_ac0/(*itr);
		*it_ac1 = *it_ac1/(*itr);
		*it_uc0 = *it_uc0/(a.size()-(*itr));
		*it_uc1 = *it_uc1/(a.size()-(*itr));

		*itr = *itr/a.size();
		cout <<"sglee: bit[" << j << ":" << j+1 << "] corr: " << *itr << ", AC0: " << (*it_ac0) << ", AC1: " << (*it_ac1) << ", UC0: " << (*it_uc0) << ", UC1: " << (*it_uc1) << endl;  
		j=j+1;
		it_ac0++;
		it_ac1++;
		it_uc0++;
		it_uc1++;
	}
#endif

	for (size_t i=Nt-2;i != -1 ;i--) {
			
		if (bw_ac_cnt[i] > th_ac) { 
			bw_anc = bw_anc<<1;
			bw_apc = bw_apc<<1;
			bw_anc = bw_anc|0x1; 
			bw_apc = bw_apc|0x0;
		}
		else if (bw_ac_cnt[i] < 1-th_ac) {
			bw_anc = bw_anc<<1;
			bw_apc = bw_apc<<1; 
			bw_anc = bw_anc|0x0; 
			bw_apc = bw_apc|0x1; 
		}
		else {
			bw_anc = bw_anc<<1;
			bw_apc = bw_apc<<1;
			bw_anc = bw_anc|0x0;
			bw_apc = bw_apc|0x0;
		}
	}
}

void io_stat::bw_pr_cal (const ACVEC &a, vector<double> &bw_pr) {

	for (ACVEC::const_iterator itr = a.begin(); itr != a.end(); itr++) {
		ACINT a = *itr;
		for (size_t i=0;i<Nt;i++) {
			bw_pr[i] = (a&0x1 == 0x1) ? bw_pr[i]+1: bw_pr[i];
			a = a>>1;
		}
	}

	for (vector<double>::iterator itr = bw_pr.begin(); itr != bw_pr.end(); itr++) {
		*itr = *itr/a.size();
	}
}

void io_stat::bw_ccor_cal (const ACVEC &a, const ACVEC &b, int &bw_cpc, int &bw_cnc, const double &th_cc) {
	vector<double> bw_ac_cnt(Nt,0);
	ACVEC::const_iterator itb = b.begin();
	int bw_xor;

	for (ACVEC::const_iterator ita = a.begin(); ita != a.end(); ita++) {
		bw_xor = *ita ^ *itb;
		for (size_t i=0;i<Nt;i++) {
			bw_ac_cnt[i] = (bw_xor&0x1 == 0x1) ? bw_ac_cnt[i]+1: bw_ac_cnt[i];
			bw_xor = bw_xor>>1;
		}
		++itb;
	}

	for (vector<double>::iterator itr = bw_ac_cnt.begin(); itr != bw_ac_cnt.end(); itr++) {
		*itr = *itr/a.size();
	}

	for (size_t i=Nt;i<0;i--) {
		if (bw_ac_cnt[i] > th_cc) { 
			bw_cnc = bw_cnc|0x1; 
			bw_cpc = bw_cpc|0x0;
			bw_cnc = bw_cnc<<1;
			bw_cpc = bw_cpc<<1;
		}
		else if (bw_ac_cnt[i] < 1-th_cc) {
			bw_cnc = bw_cnc|0x0; 
			bw_cpc = bw_cpc|0x1; 
			bw_cnc = bw_cnc<<1;
			bw_cpc = bw_cpc<<1; 
		}
		else {
			bw_cnc = bw_cnc|0x0;
			bw_cpc = bw_cpc|0x0;
			bw_cnc = bw_cnc<<1;
			bw_cpc = bw_cpc<<1;
		}
	}
}

void io_stat::print_io_stat(void) {
	cout << dec << "mean(a): " << io.mean_a << ", sdev(a): " << io.sdev_a << endl;
	cout << "mean(b): " << io.mean_b << ", sdev(b): " << io.sdev_b << endl;
	cout << hex << "APC(a): " << hex << io.bw_apc_a << ", ANC(a): " << io.bw_anc_a << endl;
	cout << "APC(b): " << hex << io.bw_apc_b << ", ANC(b): " << io.bw_anc_b << endl;
	cout << dec << "CPC: " << io.bw_cpc << ", CNC: " << io.bw_cnc << endl;
	
}
