#ifndef _BAM
#define _BAM

#include <iostream>
#include <complex>
#include <vector>

#include "hw_ac.h"
#include "ac_types.h"
#include "sim_config.h"

using namespace std;

// broken array multiplier
class bam : public hw_ac {

	private:
		size_t hbl; // the number of bits row nulling
		size_t vbl; // the number of bits column nulling

	public:
		// constructor 

		bam(size_t Nt, size_t hbl, size_t vbl, size_t msb, size_t lsb, bool table_gen);

		virtual ~bam();

		virtual int calc(const int &a, const int &b);
		virtual int calc_ref(const int &a, const int &b);

		size_t get_hbl_bits(void);
		size_t get_vbl_bits(void);

		virtual void tbl_gen();
};

#endif
