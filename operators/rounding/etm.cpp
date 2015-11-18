#include <iostream>
#include <complex>
#include <vector>
#include "etm.h"

using namespace std;

//etm::etm(void) {}

etm::etm(size_t Nt, size_t Nia, bool table_gen) {
	this->Nt = Nt;
	this->Nia = Nia;

	if (table_gen) tbl_gen();
}

etm::~etm(void) {}

size_t etm::get_ianum_bits(void) {
	return Nia;
}

int etm::calc (const int &a, const int &b) {

	// accurate part
	int a_abs;
	int b_abs;
	size_t a_sign;
	size_t b_sign;

	a_sign = (0x1&(a>>(8*sizeof(int)-1))) == 0x1 ? 1 : 0;
	b_sign = (0x1&(b>>(8*sizeof(int)-1))) == 0x1 ? 1 : 0;

	// cout << "sglee sign: " << a_sign << ", " << b_sign << endl;

	a_abs = a_sign == 0x1 ? -a : a;
	b_abs = b_sign == 0x1 ? -b : b;

//	long ap = ((a_abs>>Nia) * (b_abs>>Nia))<<(2*Nia);
	long ap = ((a>>Nia) * (b>>Nia))<<(2*Nia);

	//cout << "sglee2 ap: " << ap << ", ap_a: " << (a>>Nia) << ", ap_b:" << (b>>Nia) << endl;

	// inaccurate part
	int weight = (int)pow(2.0,(int)Nia)-1;
//	int iap = (weight&a_abs)|(weight&b_abs);
	int iap = (weight&a)|(weight&b);

	bool ones; // check both input bits are '1's
	int comp = 0; 
	for (size_t i = 0; i < Nia; i++) {
		ones = (0x1)&(iap>>(Nia-1-i)) == 0x1 ? true : false;
		if (ones) {
			comp = (int)pow(2.0, (int)(2*Nia - i)) - 1;
			iap = comp;
			break;
		}
	}  

//	cout << hex << "sglee3 ap: " << iap << ", iap_a: " << (weight&a) << ", iap_b:" << (weight&b) << dec << endl;

	int mul_out = (int) (ap | iap);

	//return (a_sign ^ b_sign) == 0x1 ? -mul_out : mul_out;
	return mul_out;
}

int etm::calc_ref (const int &a, const int &b) {
	// this is multiplier
	return a*b;
}

void etm::tbl_gen() {
	int i, j;
	int i_or_j;
	int i_w, j_w;
	for (i = 0; i < pow(2.0, Nia); i++) {
		for (int j = 0; j < pow(2.0, Nia); j++) {
			// make the partial error
			i_or_j = i | j;
			if (i_or_j != 0) {
				int idx = 0;
				for (int k=0;k<(Nia);k++) {
					if (((i_or_j>>k) && 0x1) == 0x1) {
						idx = k;
					}
				}
				i_or_j = pow(2.0, idx+1)-1;
				i_or_j = i_or_j << (Nia);
				i_or_j = i_or_j + (pow(2.0,Nia)-1);
			}
			err_tbl.push_back(make_pair(make_pair((0x80000000 | j), (0x80000000 | i)), make_pair(make_pair(i, j), -i*j-i_or_j)));
		}
	}
}
