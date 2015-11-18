#include <iostream>
#include <complex>
#include <vector>
#include "eta1.h"

using namespace std;

//eta1::eta1(void) {}

eta1::eta1(size_t Nt, size_t Nia, size_t msb, size_t lsb, bool table_gen) {
	this->Nt = Nt;
	this->Nia = Nia;
	this->msb = msb;
	this->lsb = lsb;

	if (table_gen) tbl_gen();

}

eta1::~eta1(void) {}

size_t eta1::get_ianum_bits(void) {
	return Nia;
}

int eta1::calc (const int &a, const int &b) {
	// accurate part
	int ap = ((a>>Nia) + (b>>Nia))<<Nia;
	// inaccurate part
	int weight = (int)pow(2.0,(int)Nia)-1;
	int in_and = (weight&a)&(weight&b);
	int iap = (weight&a)^(weight&b);
	bool ones; // check both input bits are '1's
	int comp; 
	for (size_t i = 0; i < Nia; i++) {
		ones = (0x1)&(in_and>>(Nia-1-i)) == 0x1 ? true : false;
		if (ones) {
			comp = (int)pow(2.0, (int)(Nia - i)) - 1;
			iap = iap | comp;
			break;
		}
	}  
	
	return (ap | iap);
}

int eta1::calc_ref (const int &a, const int &b) {
	// this is adder
	return a+b;
}

void eta1::tbl_gen() {
	int i,j;
	int i_w, j_w;
	int calc_tmp1;
	int calc_tmp2;
	for (i=0; i<pow(2,msb-lsb+1); i++) {
		for (j=0; j<pow(2,msb-lsb+1); j++) {
			i_w = i<<lsb;
			j_w = j<<lsb;
			calc_tmp1 = calc(i_w, j_w);
			calc_tmp2 = calc_ref(i_w, j_w);

			//cout << "sglee: " << i_w << "," << j_w << ", apr: " << calc_tmp1 << ", ori: " << calc_tmp2 << endl;
			if (calc_tmp1 != calc_tmp2) {
				//err_tbl.insert(make_pair(make_pair(i_w,j_w), calc_tmp2-calc_tmp1));
				err_tbl.push_back(make_pair(make_pair(i_w,j_w), make_pair(make_pair(0,0),calc_tmp2-calc_tmp1)));
			}
		}
	}
}
