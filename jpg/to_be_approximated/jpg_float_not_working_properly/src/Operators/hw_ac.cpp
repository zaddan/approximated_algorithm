#include <iostream>
#include <complex>
#include <vector>
#include "hw_ac.h"

using namespace std;

hw_ac::hw_ac(void) {}

hw_ac::~hw_ac(void) {}

size_t hw_ac::get_Nt(void) {
	return Nt;
}


/*
size_t hw_ac::get_ianum_bits(void) {
  printf("right here");
  return 1;
}
*/

void hw_ac::print_err_tbl(void) {
	for (ACETBL::iterator it = err_tbl.begin(); it != err_tbl.end(); it++) {
		cout << hex << "Pattern A: " << (*it).first.first << ", Pattern B: " << (*it).first.second << dec << ", Error: " << (*it).second.first.first << "A, " << (*it).second.first.second << "B, " << (*it).second.second << dec << endl;
	}
}
