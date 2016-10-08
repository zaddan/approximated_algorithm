#include<iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "fp_helpers.h"
#include <cstring>
using namespace std;

unsigned getNumberOfDigits (long unsigned i)
{
    return i > 0 ? (int) log2 ((double) i) + 1 : 1;
}

void getFPComponents(float number, fpType &num){
 //unsigned char *ptr = reinterpret_cast<unsigned char *>(&number);
 int *ptr = (int *)malloc(sizeof(int));
 memcpy(ptr, &number, sizeof(ptr));
// num.Sign = *ptr>> 31;
// num.Exp = *ptr& 0x7f800000;
// num.Exp >>= MANTISA_WIDTH;
// num.Mantisa = *ptr& 0x007fffff;
 num.Sign = *ptr>> 31;
 num.Exp = *ptr& 0x7f800000;
 num.Exp >>= MANTISA_WIDTH;
 num.Mantisa = *ptr& 0x007fffff;
 //cout<<"number ghabl conversion: "<<number<<" floating point is: "<<number<< "sign is: "<<num.Sign<< " exp is: "<<num.Exp<< " Mantisa is"<< num.Mantisa<<endl;
}

float convertFPCompToFP(fpType num){
    //cout<<"sign"<<num.Sign<<" exp: "<<num.Exp<< "matisa :"<<num.Mantisa<<endl;
   
    float result = 0;
    int expSoFar = num.Exp - bias; //what exp to use at the moment
    int decodedMantisaWithExtraOne; 
    if (num.Exp != 0) {   
        decodedMantisaWithExtraOne = (1 <<MANTISA_WIDTH) + num.Mantisa;//decode mantisa with Extra one
    } else{
        decodedMantisaWithExtraOne = num.Mantisa;
    }
    int mask; 
    if (num.Exp != 0) { 
        mask = 1 << MANTISA_WIDTH;
    } else{
        mask = 1 << (MANTISA_WIDTH-1);
    }
    int shftRightAmt = MANTISA_WIDTH;
    while(1)  {
        result += ((decodedMantisaWithExtraOne & mask) >>  shftRightAmt)* pow(2, expSoFar );
        mask = mask >> 1; 
        shftRightAmt -= 1;
        expSoFar -=1;
        if (mask == 0) {
            break;
        }
    }

    if (num.Sign) {
        result = -1*result;
    }
    
    float result_under_test;
    int int_version = 0x0;
    int_version |= (num.Sign<<31);
    int_version |= (num.Exp <<MANTISA_WIDTH); 
    int_version |= num.Mantisa; 
    //cout<<"int_version"<<int_version<<endl; 
    memcpy(&result_under_test, &int_version, sizeof(int_version));
     
    if (result_under_test != result) {
        cout<<"ERRR: "<<"results are not the same result: "<< result<< " result_under_test: "<< result_under_test<<endl;
        exit(0); 
    }
    //cout<<"res: : "<<result<<endl;
    return result;
    

    return result_under_test;
  
}

void normalizeMul(fpType &resultNum){ 
    int numOfDig = getNumberOfDigits (resultNum.Mantisa);
    if (numOfDig > MANTISA_WIDTH + 1) {
        resultNum.Mantisa = resultNum.Mantisa >> (numOfDig - (MANTISA_WIDTH + 1));
        resultNum.Exp= resultNum.Exp + (numOfDig - (2*MANTISA_WIDTH + 1));
    }else{
        ; //do nothing
    }
}

void normalizeAdd(fpType &resultNum){ 
    printf("mantisa is %x",   resultNum.Mantisa);
    int numOfDig = getNumberOfDigits (resultNum.Mantisa);
    if (numOfDig >= (MANTISA_WIDTH + 1)) {
        resultNum.Mantisa = resultNum.Mantisa >> (numOfDig - (MANTISA_WIDTH + 1));
        resultNum.Exp= resultNum.Exp + (numOfDig - (MANTISA_WIDTH + 1));
        cout<<"spilling"<<endl; 
    }else{
        ; //do nothing
    }
}

