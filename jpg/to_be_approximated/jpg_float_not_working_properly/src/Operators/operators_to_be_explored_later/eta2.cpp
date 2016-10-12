#include <iostream>
#include <complex>
#include <vector>
#include "eta2.h"

using namespace std;

eta2::~eta2(void) {}

eta2::eta2(size_t Nt, size_t K, size_t msb, size_t lsb, bool table_gen) {
	this->Nt = Nt;
	this->K = K;
	this->msb = msb;
	this->lsb = lsb;

	if (table_gen) tbl_gen();

}

size_t eta2::get_seg_bits(void) {
	return K;
}


float eta2::calc(const float &number1, const int &number2) {
    float numberOut = number2; 
    calc(number1, numberOut);
}


float eta2::calc(const int &number1, const float &number2) {
    float numOut = number1; 
    calc(numOut, number2);
}

float eta2::calc (const float &a, const float &b) {
    cout <<"this function needs to be written"<<endl;
    exit(1);
    return (1.0);
}


int eta2::calc (const int &a, const int &b) {
	int carry;
	int prev_carry;
	int seg_a;
	int seg_b;
	int sum = 0;
	int seg_sum;
	int num_seg = ceil(Nt/K); // number of segmentation
	int numbit_lsb = Nt - K*(num_seg-1); // when Nt/K is not an integer
	int weight;
	int seg_bitpos = 0;
	int seg_numbit;
	int prev_seg_numbit = 0;
	for (int i=0;i<num_seg;i++) {
		seg_numbit = i == 0 ? numbit_lsb : K;
		weight = pow(2,seg_numbit)-1;
		seg_a = weight&(a>>seg_bitpos);
		seg_b = weight&(b>>seg_bitpos);
		seg_sum = seg_a + seg_b;
		prev_carry = i == 0 ? 0 : carry;

		carry = ((0x1<<seg_numbit)&seg_sum)>>seg_numbit;
		seg_sum += prev_carry;

		seg_sum = weight&seg_sum;
		seg_bitpos += seg_numbit;

		sum = sum|(seg_sum<<prev_seg_numbit);

		prev_seg_numbit += seg_numbit;

#ifdef DEBUG_ETA2
		cout << hex << "eta2 seg[" << i << "] pre_seg_sum: " << seg_a + seg_b << dec << endl;
		cout << hex << "eta2 seg[" << i << "] prev_carry: " << prev_carry << dec << endl;
		cout << hex << "eta2 seg[" << i << "] seg_sum: " << seg_sum << dec << endl;
		cout << hex << "eta2 seg[" << i << "] sum: " << sum << dec << endl;
#endif
	}

	// last sub-adder: need to be consider sign
	prev_carry = carry;
	seg_sum = ((a>>seg_bitpos) + (b>>seg_bitpos) + prev_carry)<<seg_bitpos;
	sum = sum|seg_sum;



	return sum;
	
}


unsigned int eta2::calc (const unsigned int &a, const unsigned int &b) {
	
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DISCLAMIER:"<<endl;
    cout<<"DISCLAMIER:"<<endl;
    cout<<"I DON't GAURATNEED THAT ETA2 WOULD ACTUALLY WORKD WITH UNSIGNED NUMBERS"<<endl;
    cout<<"END OF DISCLAMIER:"<<endl;
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DISCLAMIER:"<<endl;
    
    unsigned int carry;
	unsigned int prev_carry;
	unsigned int seg_a;
	unsigned int seg_b;
	unsigned int sum = 0;
	unsigned int seg_sum;
	unsigned int num_seg = ceil(Nt/K); // number of segmentation
	unsigned int numbit_lsb = Nt - K*(num_seg-1); // when Nt/K is not an unsigned integer
	unsigned int weight;
	unsigned int seg_bitpos = 0;
	unsigned int seg_numbit;
	unsigned int prev_seg_numbit = 0;
	for (unsigned int i=0;i<num_seg;i++) {
		seg_numbit = i == 0 ? numbit_lsb : K;
		weight = pow(2,seg_numbit)-1;
		seg_a = weight&(a>>seg_bitpos);
		seg_b = weight&(b>>seg_bitpos);
		seg_sum = seg_a + seg_b;
		prev_carry = i == 0 ? 0 : carry;

		carry = ((0x1<<seg_numbit)&seg_sum)>>seg_numbit;
		seg_sum += prev_carry;

		seg_sum = weight&seg_sum;
		seg_bitpos += seg_numbit;

		sum = sum|(seg_sum<<prev_seg_numbit);

		prev_seg_numbit += seg_numbit;

#ifdef DEBUG_ETA2
		cout << hex << "eta2 seg[" << i << "] pre_seg_sum: " << seg_a + seg_b << dec << endl;
		cout << hex << "eta2 seg[" << i << "] prev_carry: " << prev_carry << dec << endl;
		cout << hex << "eta2 seg[" << i << "] seg_sum: " << seg_sum << dec << endl;
		cout << hex << "eta2 seg[" << i << "] sum: " << sum << dec << endl;
#endif
	}

	// last sub-adder: need to be consider sign
	prev_carry = carry;
	seg_sum = ((a>>seg_bitpos) + (b>>seg_bitpos) + prev_carry)<<seg_bitpos;
	sum = sum|seg_sum;



	return sum;
	
}


int eta2::calc_ref (const int &a, const int &b) {
	// this is adder
	return a+b;
}

void eta2::tbl_gen() {
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

