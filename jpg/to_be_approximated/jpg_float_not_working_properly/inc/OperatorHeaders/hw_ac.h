#ifndef _HWAC
#define _HWAC

#include <iostream>
#include <complex>
#include <vector>
#include <map>

#include "ac_types.h"
#include "sim_config.h"

using namespace std;

class hw_ac {

	private:

	protected:
		size_t Nt; // total number of bits
		size_t msb; // msb of error table
		size_t lsb; // lsb of error table
		//map<int , pair<int, int> > err_tbl; // error table

	public:
		ACETBL err_tbl; // error table
//		ACETBL err_tbl2; // error table helper
//		ACETBL2 err_tbl_h; // error table helper
//		ACETBL3 err_tbl3; // error table helper

		hw_ac(); // constructor
		virtual ~hw_ac();
		//void tbl_gen(const size_t &msb, const size_t &lsb, int (*calc) (const int &, const int &), int (*calc_ref) (const int &, const int &));
		virtual void tbl_gen()=0;

		virtual int calc(const int &a, const int &b) = 0; // pure virtual
		virtual int calc(const long &a, const int &b) = 0; // pure virtual
		virtual int calc(const int &a, const long &b) = 0; // pure virtual
        virtual unsigned int calc(const unsigned int &a, const unsigned int &b) = 0; // pure virtual
        
        
        
        virtual size_t get_ianum_bits(void)=0;
        virtual int calc(const unsigned int &a, const int &b) = 0; // pure virtual
        virtual int calc(const int &a, const unsigned int &b) = 0; // pure virtual
        
        virtual float calc(const float &a, const float &b) = 0; // pure virtual
		virtual float calc(const float &a, const int &b) = 0; // pure virtual
		virtual float calc(const int &a, const float &b) = 0; // pure virtual
        virtual int calc_ref(const int &a, const int &b) = 0; // pure virtual

		void print_err_tbl(void);
//		void print_err_tbl3(void);

		size_t get_Nt();
};

#endif
