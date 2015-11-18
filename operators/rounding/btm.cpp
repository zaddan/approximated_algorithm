#include <iostream>
#include <complex>
#include <vector>
#include "btm.h"

using namespace std;

btm::btm(void) {}

btm::btm(size_t Nt, size_t hbl, size_t vbl, bool table_gen) {
	this->Nt = Nt;
	this->hbl = hbl;
    this->vbl = hbl;	
    //this->vbl = vbl;

	if (table_gen) tbl_gen();
}

btm::~btm(void) {}

size_t btm::get_hbl_bits(void) {
	return hbl;
}

size_t btm::get_vbl_bits(void) {
	return vbl;
}

int btm::calc(const int &a, const int &b) {

	// inaccurate part
	int weight = pow(2, vbl) - 1;
	int iap_a = weight&a;
	int a_rnd = ((a >> vbl) == (pow(2, Nt) - 1)) ? 0 : (a >> vbl) + 1;
	int b_rnd = ((b >> vbl) == (pow(2, Nt) - 1)) ? 0 : (b >> vbl) + 1;
	//printf("SGLEE VBL: %d, %x, %x\n", vbl, (b >> vbl), (b >> vbl)+1);
	//if ((b >> vbl) == (pow(2, Nt) - 1)) cout << "SGLEE OVERFLOW" << endl;
	//if ((a >> vbl) == (pow(2, Nt) - 1)) cout << "SGLEE OVERFLOW" << endl;
	int a_op = (iap_a >> (vbl - 1)) == 0x1 ? a_rnd : (a >> vbl);
	int iap_b = weight&b;
	int b_op = (iap_b >> (vbl - 1)) == 0x1 ? b_rnd : (b >> vbl);

	// accurate part
	return ((a_op)*(b_op)) << (2*vbl);
}

int btm::calc_ref(const int &a, const int &b) {
	return a*b;
}

void btm::tbl_gen() {
	int i, j;
	int i_w, j_w;
	for (i = 0; i < pow(2, msb - lsb + 1); i++) {
		for (int j = 0; j < pow(2, msb - lsb + 1); j++) {
					err_tbl.push_back(make_pair(make_pair((0x80000000 | i), (0x80000000 | j)), make_pair(make_pair(i, j), i*j)));
		}
	}
}



