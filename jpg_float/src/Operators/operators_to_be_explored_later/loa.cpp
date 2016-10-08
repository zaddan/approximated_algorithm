#include <iostream>
#include <complex>
#include <vector>
#include "loa.h"

using namespace std;

loa::loa(void) {}

loa::loa(size_t Nt, size_t Nia) {
	this->Nt = Nt;
	this->Nia = Nia;
}

loa::loa(size_t Nt, size_t Nia, size_t msb, size_t lsb, bool table_gen) {
	this->Nt = Nt;
	this->Nia = Nia;
	this->msb = msb;
	this->lsb = lsb;

	if (table_gen) tbl_gen();
	//tbl_gen2();

}

loa::~loa(void) {}

size_t loa::get_ianum_bits(void) {
	return Nia;
}

float loa::calc (const float &a, const float &b) {
    cout <<"this function needs to be written"<<endl;
    exit(1);
    return (1.0);
}

float loa::calc(const int &number1, const float &number2) {
    float numOut = number1; 
    calc(numOut, number2);
}

float loa::calc(const float &number1, const int &number2) {
   float numOut = number2; 
    calc(number1, numOut);
} 


int loa::calc (const int &a, const int &b) {

	// inaccurate part
	int weight = pow(2,Nia)-1;
	int iap = (weight&a)|(weight&b);
	int carry = ((weight&a)&(weight&b))>>(Nia-1);
	
	// accurate part
	int ap = ((a>>Nia) + (b>>Nia) + carry)<<Nia;

	return (ap | iap);
	
}



unsigned int loa::calc (const unsigned int &a, const unsigned int &b) {

	// inaccurate part
	unsigned int weight = pow(2,Nia)-1;
	unsigned int iap = (weight&a)|(weight&b);
	unsigned int carry = ((weight&a)&(weight&b))>>(Nia-1);
	
	// accurate part
	unsigned int ap = ((a>>Nia) + (b>>Nia) + carry)<<Nia;

	return (ap | iap);
	
}

int loa::calc_ref (const int &a, const int &b) {
	// this is adder
	return a+b;
}

/*
void loa::tbl_gen2() {
	int err[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
//	int final_level = Nia-1;
	int bit_mask = pow(2, Nia) - 1;

	for (int i = 1; i < pow(2,Nia); i++) { // no 0 since 0 means no error.
//		int i_inv = bit_mask & !i;
		int a = 0;
		int b = 0;
		int a_vld = 0;
		int b_vld = 0;
		int level = 0;
		int err_mag = 0;

		tbl_rec(level, i, a, b, a_vld, b_vld, err_mag, &err[0]);

	}
}
*/

void loa::tbl_rec(int level, int i, int a, int b, int a_vld, int b_vld, int err_mag, int *err) {
	if (level != Nia - 1) {
		if ((0x1 & (i >> level)) == 1) {
			a = a | (0x1 << level);
			b = b | (0x1 << level);
			a_vld = a_vld | (0x1 << level);
			b_vld = b_vld | (0x1 << level);
			err_mag = err_mag + err[level];
			level = level + 1;
			tbl_rec(level, i, a, b, a_vld, b_vld, err_mag, &err[0]);
		}
		else {
			a = a | (0x0 << level);
			b = b | (0x0 << level);
			a_vld = a_vld | (0x1 << level);
			b_vld = b_vld | (0x0 << level);
			level = level + 1;
			tbl_rec(level, i, a, b, a_vld, b_vld, err_mag, &err[0]);

			level = level - 1;
			a = a | (0x1 << level);
			b = b | (0x0 << level);
			a_vld = a_vld | (0x1 << level);
			b_vld = b_vld | (0x1 << level);
			level = level + 1;
			tbl_rec(level, i, a, b, a_vld, b_vld, err_mag, &err[0]);
		}
	}
	else { // level = Nia-1
		if (0x1 & (i >> level) == 1) {
			a = a | (0x1 << level);
			b = b | (0x1 << level);
			a_vld = a_vld | (0x1 << level);
			b_vld = b_vld | (0x1 << level);
			err_mag = err_mag + err[level];
//			err_tbl2.push_back(make_pair(make_pair(a, b), make_pair(0, err_mag)));
//			err_tbl_h.push_back(make_pair(a_vld, b_vld));

//			printf("TBL2: %x, %x, %d, vld: %x, %x \n", a, b, err_mag, a_vld, b_vld);

		}
		else {
			a = a | (0x0 << level);
			b = b | (0x0 << level);
			a_vld = a_vld | (0x1 << level);
			b_vld = b_vld | (0x0 << level);
//			err_tbl2.push_back(make_pair(make_pair(a, b), make_pair(0, err_mag)));
//			err_tbl_h.push_back(make_pair(a_vld, b_vld));

//			printf("TBL2: %x, %x, %d, vld: %x, %x \n", a, b, err_mag, a_vld, b_vld);

			a = a | (0x1 << level);
			b = b | (0x0 << level);
			a_vld = a_vld | (0x1 << level);
			b_vld = b_vld | (0x1 << level);
//			err_tbl2.push_back(make_pair(make_pair(a, b), make_pair(0, err_mag)));
//			err_tbl_h.push_back(make_pair(a_vld, b_vld));

//			printf("TBL2: %x, %x, %d, vld: %x, %x \n", a, b, err_mag, a_vld, b_vld);

		}
	}

}

/*
void loa::tbl_gen() {
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
				err_tbl.push_back(make_pair(make_pair(i_w,j_w), make_pair(0,calc_tmp2-calc_tmp1)));
			}
		}
	}
}
*/

void loa::tbl_gen() {
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
