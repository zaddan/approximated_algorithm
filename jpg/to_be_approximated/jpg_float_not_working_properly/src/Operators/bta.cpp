#include <iostream>
#include <complex>
#include <vector>
#include "bta.h"
#include "globals.h"
#include "fp_helpers.h"
using namespace std;
#include <cstring>
#include <cassert>

bta::bta(size_t Nt, size_t Nia, size_t msb, size_t lsb, bool table_gen) {
	this->Nt = Nt;
	this->Nia = Nia;
	this->msb = msb;
	this->lsb = lsb;

	if (table_gen) tbl_gen();

}

bta::~bta(void) {}

size_t bta::get_ianum_bits(void) {
	return Nia;
}

int bta::calc(const long &number1, const int &number2) {
    printf("this needs to be defined\n");
    exit(0);
}

int bta::calc(const int &number1, const long &number2) {
    printf("this needs to be defined\n");
    exit(0);
}

float bta::calc(const float &number1, const int &number2) {
    #ifdef VERBOSE 
    cout<<"=============insde half float"<<endl; 
    #endif 
    float numOut = number2; 
    calc(number1, numOut);
} 

float bta::calc(const int &number1, const float &number2) {
    cout<<"=============insde other half float"<<endl; 
    float numOut = number1; 
    calc(numOut, number2);
} 



float bta::calc(const float &number1, const float &number2) {
   
    /*
    FILE* fp;
    fp = fopen("diff_file.txt", "ab+");
    
    if ( num1_inverse_converted+num2_inverse_converted != (number1+number2)){
        float diff_part_1 =  num1_inverse_converted+num2_inverse_converted;
        float diff_part_2 =  number1+number2;
        float diff = (diff_part_1 - diff_part_2);
        
        //--checking for under/over-flow 
        if (std::isinf(diff_part_1)) {
            fprintf(fp, "par_1_overflow\n");
        }
        if (std::isinf(diff_part_2)) {
            fprintf(fp, "par_2_overflow\n");
        }
        if ((diff_part_1 != diff_part_2) && (diff ==0)){
            fprintf(fp, "diff underflow\n");
        }

        fprintf(fp, "error   ");
        if ( diff > 1){
            fprintf(fp, "acc:%f apx: %f diff:%f\n",num1_inverse_converted+num2_inverse_converted ,number1+number2, diff);
        }
        fprintf(fp, "------\n");
    }
    
    fclose(fp);
    */ 
    
    
    /*
    int *num1_ptr = (int *)malloc(sizeof(int));
    memcpy(num1_ptr, &number1, sizeof(num1_ptr));
    int num1_mantisa =  *num1_ptr & ~(0xff800000);
    num1_mantisa = (num1_mantisa >> Nia) <<Nia;
    *num1_ptr &= (0xff800000);
    *num1_ptr |= num1_mantisa;
    
    int *num2_ptr = (int *)malloc(sizeof(int));
    memcpy(num2_ptr, &number2, sizeof(num2_ptr));
    int num2_mantisa =  *num2_ptr & ~(0xff800000);
    num2_mantisa = (num2_mantisa >> Nia) <<Nia;
    *num2_ptr &= (0xff800000);
    *num2_ptr |= num2_mantisa;
 */

    int num1_ptr ;
    memcpy(&num1_ptr, &number1, sizeof(num1_ptr));
    int num1_mantisa =  num1_ptr & ~(0xff800000);
    num1_mantisa = (num1_mantisa >> Nia) <<Nia;
    num1_ptr &= (0xff800000);
    num1_ptr |= num1_mantisa;
    
    int num2_ptr;
    memcpy(&num2_ptr, &number2, sizeof(num2_ptr));
    int num2_mantisa =  num2_ptr & ~(0xff800000);
    num2_mantisa = (num2_mantisa >> Nia) <<Nia;
    num2_ptr &= (0xff800000);
    num2_ptr |= num2_mantisa;
    

    float num2_restored, num1_restored; 
    memcpy(&num1_restored, &num1_ptr, sizeof(num1_restored));
    memcpy(&num2_restored, &num2_ptr, sizeof(num2_restored));
    return num2_restored + num1_restored;
   /*
    fpType num1;
    fpType num2;
    getFPComponents(number1, num1); //get the fp componenets
    getFPComponents(number2, num2); //get the fp components

    num1.Mantisa = ((num1.Mantisa)>> Nia) <<Nia;
    num2.Mantisa = ((num2.Mantisa)>> Nia) <<Nia;
    
    float num1_inverse_converted = convertFPCompToFP(num1);
    float num2_inverse_converted = convertFPCompToFP(num2);
    
    if (num1_inverse_converted != num1_restored){
        cout<<"sadf"<<endl; 
        cout<<"inverse: "<<num1_inverse_converted<< " num1_ptr: "<<num1_restored<<endl;
        exit(0); 
    }
    if (num2_inverse_converted != num2_restored){
        cout<<"inverse: "<<num2_inverse_converted<< " num2_ptr: "<<num2_restored<<endl;
        exit(0); 
    }
    return num1_inverse_converted + num2_inverse_converted;
     */
}


int bta::calc(const int &a, const int &b) {
    // inaccurate part
    #ifdef VERBOSE 
    cout<<"=============in int version"<<endl; 
    #endif  
    int weight = pow(2, Nia) - 1;
	int iap_a = weight&a;
#ifdef BT_RND
	int a_op = (iap_a >> (Nia - 1)) == 0x1 ? (a >> Nia) + 1 : (a >> Nia);
#else
	int a_op = (a >> Nia);
#endif
	int iap_b = weight&b;
#ifdef BT_RND
	int b_op = (iap_b >> (Nia - 1)) == 0x1 ? (b >> Nia) + 1 : (b >> Nia);
#else
	int b_op = (b >> Nia);
#endif

	// accurate part
	return ((a_op) + (b_op)) << Nia;
}
//
//


unsigned int bta::calc(const unsigned int &a, const unsigned int &b) {
    // inaccurate part
    #ifdef VERBOSE 
    cout<<"=============in unsigned unsigned int version"<<endl; 
    #endif  
    unsigned int weight = pow(2, Nia) - 1;
	unsigned int iap_a = weight&a;
#ifdef BT_RND
	unsigned int a_op = (iap_a >> (Nia - 1)) == 0x1 ? (a >> Nia) + 1 : (a >> Nia);
#else
	unsigned int a_op = (a >> Nia);
#endif
	unsigned int iap_b = weight&b;
#ifdef BT_RND
	unsigned int b_op = (iap_b >> (Nia - 1)) == 0x1 ? (b >> Nia) + 1 : (b >> Nia);
#else
	unsigned int b_op = (b >> Nia);
#endif

	// accurate part
	return ((a_op) + (b_op)) << Nia;
}

int bta::calc(const unsigned int &a_unsigned, const int &b) {
    // inaccurate part
    #ifdef VERBOSE 
    cout<<"=============in unsigned unsigned int version"<<endl; 
    #endif  
    int a = (int)a_unsigned; 
    int weight = pow(2, Nia) - 1;
	int iap_a = weight&(int)a;
#ifdef BT_RND
	int a_op = (iap_a >> (Nia - 1)) == 0x1 ? (a >> Nia) + 1 : (a >> Nia);
#else
	int a_op = ((int)a >> Nia);
#endif
	int iap_b = weight&b;
#ifdef BT_RND
	int b_op = (iap_b >> (Nia - 1)) == 0x1 ? (b >> Nia) + 1 : (b >> Nia);
#else
	int b_op = (b >> Nia);
#endif

	// accurate part
	return ((a_op) + (b_op)) << Nia;
}

int bta::calc(const int &a, const unsigned int &b_unsigned) {
    // inaccurate part
    #ifdef VERBOSE 
    cout<<"=============in unsigned unsigned int version"<<endl; 
    #endif  
    int b = (int)b_unsigned; 
    int weight = pow(2, Nia) - 1;
	int iap_a = weight&(int)a;
#ifdef BT_RND
	int a_op = (iap_a >> (Nia - 1)) == 0x1 ? (a >> Nia) + 1 : (a >> Nia);
#else
	int a_op = ((int)a >> Nia);
#endif
	int iap_b = weight&b;
#ifdef BT_RND
	int b_op = (iap_b >> (Nia - 1)) == 0x1 ? (b >> Nia) + 1 : (b >> Nia);
#else
	int b_op = (b >> Nia);
#endif

	// accurate part
	return ((a_op) + (b_op)) << Nia;
}




int bta::calc_ref(const int &a, const int &b) {
	// this is adder
	return a + b;
}

void bta::tbl_gen() {
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
				err_tbl.push_back(make_pair(make_pair(i_w,j_w), make_pair(make_pair(0,0),calc_tmp2-calc_tmp1)));
			}
		}
	}
}
