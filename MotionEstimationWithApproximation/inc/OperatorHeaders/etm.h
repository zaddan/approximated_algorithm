#ifndef _ETM
#define _ETM

#include <iostream>
#include <complex>
#include <vector>

#include "hw_ac.h"
#include "ac_types.h"
#include "sim_config.h"

using namespace std;

class etm : public hw_ac {

	private:
		size_t Nia; // the number of bits for inaccurate LSBs

	public:
		// constructor 
		etm(size_t Nt, size_t Nia, bool table_gen);

		virtual ~etm();

		int calc(const int &a, const int &b);
		int calc_ref(const int &a, const int &b);

		size_t get_ianum_bits(void);
		virtual void tbl_gen();

};

#endif
