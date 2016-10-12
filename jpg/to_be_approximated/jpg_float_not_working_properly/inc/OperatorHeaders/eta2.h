#ifndef _ETA2
#define _ETA2

#include <iostream>
#include <complex>
#include <vector>

#include "hw_ac.h"
#include "ac_types.h"
#include "sim_config.h"

using namespace std;

class eta2 : public hw_ac {

	private:
		size_t K; // the number of bits for each segmentation

	public:
		// constructor 
		eta2(size_t Nt, size_t K, size_t msb, size_t lsb, bool table_gen);

		virtual ~eta2();

		virtual int calc(const int &a, const int &b);
		virtual unsigned int calc(const unsigned int &a, const unsigned int &b);
        virtual float calc(const float &a, const float &b);
        virtual float calc(const float &number1, const int &number2);
        virtual float calc(const int &number1, const float &number2);
        virtual int calc_ref(const int &a, const int &b);

		virtual void tbl_gen();
		size_t get_seg_bits(void);

};

#endif
