//#include "mytype.h"
#include <stdio.h>
#include <iostream>
using namespace std;
#include <bitset>
#include "fi/Fixed.hpp"
#include "fi/overflow/Wrap.hpp"


int main(int argc, char** argv){
    Fi::Fixed<32, 10, Fi::SIGNED> newVar("12345.122");
    cout << ((12345122>>2) << 2)<<endl; 
    
    cout<<newVar <<endl;
    //cout<<((newVar >> 2) << 2)<<endl; 
    cout<<((newVar >> 2) <<2) <<endl; 
    /* fxp_coeff_t newVar2 = 12345;*/
    //fxp_coeff_t newVar3 = 12344.2;
    //bitset<64> x(newVar); 
    //bitset<32> y(newVar2); 
    //bitset<32> z(newVar3); 
    //cout << x <<endl;
    //cout << y <<endl;
    //cout << z <<endl;
    //exit(1); 
    //for (int i=0; i<7; i++) {
        //cout << (newVar>>i)<<endl; 
   /* }*/
}
