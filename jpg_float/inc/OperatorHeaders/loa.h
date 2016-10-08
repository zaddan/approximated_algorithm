#ifndef _LOA
#define _LOA

#include <iostream>
#include <complex>
#include <vector>

#include "hw_ac.h"
#include "ac_types.h"
#include "sim_config.h"

using namespace std;

// low part or adder (loa) adder
class loa : public hw_ac {

	private:
		size_t Nia; // the number of bits for inaccurate part

	public:
		// constructor 
		loa();
		loa(size_t Nt, size_t Nia);

		loa(size_t Nt, size_t Nia, size_t msb, size_t lsb, bool table_gen);
		virtual ~loa();

		virtual int calc(const int &a, const int &b);
		virtual unsigned int calc(const unsigned int &a, const unsigned int &b);
        virtual float calc(const float &a, const float &b);
        virtual float calc(const float &number1, const int &number2);
        virtual float calc(const int &number1, const float &number2);
        virtual int calc_ref(const int &a, const int &b);
        
        size_t get_ianum_bits(void);
		size_t get_tnum_bits(void);

		virtual void tbl_gen();
//		void tbl_gen2();
		void tbl_rec(int level, int i, int a, int b, int a_vld, int b_vld, int err_mag, int *err);

};

#endif
