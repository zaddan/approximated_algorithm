#ifndef _AC_TYPES
#define _AC_TYPES

#include <vector>
#include <map>
using namespace std;

typedef int ACINT;
typedef vector<ACINT> ACVEC;

// typedef map<pair<int,int>,int> ACETBL;	// AC error table 
//typedef map<pair<int,int>, pair<int,int> > ACETBL;	// AC error table 
//typedef vector<pair<pair<int, int>, pair<int, int> > > ACETBL;	// AC error table
typedef vector<pair<pair<int, int>, pair<pair<int, int>, int> > > ACETBL;	// AC error table

//typedef vector<pair<int, int> > ACETBL2;	// error table helper

struct IO_STAT {
	double mean_a;		// mean of a
	double sdev_a;		// variance of a
	double mean_b;		// mean of b
	double sdev_b;		// variance of b
	int bw_apc_a;		// bit-wise positive auto correlation of a
	int bw_anc_a;		// bit-wise negative auto correlation of a
	int bw_apc_b;		// bit-wise positive auto correlation of b
	int bw_anc_b;		// bit-wise negative auto correlation of b
	int bw_cpc;		// bit-wise positive cross correlation of a & b
	int bw_cnc;		// bit-wise negative cross correlation of a & b
	vector<double> bw_pr_a;		// bit-wise '0' probability of a
	vector<double> bw_pr_b;		// bit-wise '1' probability of b
	vector<double> bw_apr_a;		// bit-wise positive auto correlation of a
	vector<double> bw_apr_b;		// bit-wise positive auto correlation of b

};

struct ERR_STAT {
	double err_mean;
	double err_sdev;
	map<int, double> err_dist;
	int err_max;
	int err_min;
	double err_total_prob;
};

#endif
