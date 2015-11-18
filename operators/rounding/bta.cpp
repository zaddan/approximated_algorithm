#include <iostream>
#include <complex>
#include <vector>
#include "bta.h"

using namespace std;

bta::bta(size_t Nt, size_t Nia, size_t msb, size_t lsb, bool table_gen) {
	this->Nt = Nt; //total number of bits
	this->Nia = Nia; //number of apx 
    this->msb = Nia - 1; //most sig
	this->lsb = 0; //least sig

	if (table_gen) tbl_gen();

}

bta::~bta(void) {}

size_t bta::get_ianum_bits(void) {
	return Nia;
}

int bta::calc(const int &a, const int &b) {

	// inaccurate part
	int weight = pow(2, Nia) - 1;
	int iap_a = weight&a;
	int a_op = (iap_a >> (Nia - 1)) == 0x1 ? (a >> Nia) + 1 : (a >> Nia);
	int iap_b = weight&b;
	int b_op = (iap_b >> (Nia - 1)) == 0x1 ? (b >> Nia) + 1 : (b >> Nia);

	// accurate part
	return ((a_op) + (b_op)) << Nia;
}

int bta::calc_ref(const int &a, const int &b) {
	// this is adder
	return a + b;
}

void bta::tbl_gen() {
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
				err_tbl.push_back(make_pair(make_pair(i_w,j_w), make_pair(make_pair(0,0),calc_tmp2-calc_tmp1)));
			}
		}
	}
}
