#include <iostream>
#include <complex>
#include <vector>
#include "bam.h"

using namespace std;

bam::bam(size_t Nt, size_t hbl, size_t vbl, size_t msb, size_t lsb, bool table_gen) {
	this->Nt = Nt;
	this->hbl = hbl;
	this->vbl = vbl;
	this->msb = msb;
	this->lsb = lsb;

	if (table_gen) tbl_gen();

}

bam::~bam(void) {}

size_t bam::get_hbl_bits(void) {
	return hbl;
}

size_t bam::get_vbl_bits(void) {
	return vbl;
}

int bam::calc (const int &a, const int &b) {
	long pp; // partial product
	long lsb_zeros = (int)pow(2.0, (int)vbl) - 1;
	lsb_zeros = -lsb_zeros - 1;
	long sum = 0;
	int abs_a = (a<0) ? -a : a;
	int abs_b = (b<0) ? -b : b;
	int sign = (a<0 && b>0) || (a>0 && b<0) ? 1 : 0;
	for (int i = 0; i < Nt; i++) {
		if (i < hbl) {
			pp = 0;
		} else {
			pp = ((abs_a>>i)&0x1) == 1 ? abs_b : 0;
		}
		pp = pp<<i;
		pp = pp&lsb_zeros;
		sum += pp;
	}

	sum = sign == 1 ? -sum : sum;
	return (int) sum;
}

int bam::calc_ref (const int &a, const int &b) {
	return a*b;
}

/*
void bam::tbl_gen() {
	int i;
	int i_w;
	for (i=0; i<(int)pow(2.0,(int)(msb-lsb+1)); i++) {
		i_w = i<<lsb;

		if (i_w != 0) {
			err_tbl.push_back(make_pair(make_pair((0x80000000 | i_w),0xFFFFFFFF), make_pair(make_pair(0, i_w), 0)));
		}
	}
}
*/

void bam::tbl_gen() {
	int i, j;
	int i_w, j_w;
	for (i = 1; i < pow(2, msb - lsb + 1); i++) {
		for (int j = 0; j < pow(2, msb - lsb + 1); j++) {
					err_tbl.push_back(make_pair(make_pair((0x80000000 | i), (0x80000000 | j)), make_pair(make_pair(0, i), 0)));
		}
	}
}
