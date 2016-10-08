#include <iostream>
#include <math.h>
using namespace std;
int mul_calc(const int &a, const int &b, int vbl) {

    // inaccurate part
    int abs_a = (a<0) ? -a : a;
    int abs_b = (b<0) ? -b : b;
    int sign = (a<0 && b>0) || (a>0 && b<0) ? 1 : 0;
    //  int iap_a = weight&a;
//  int iap_b = weight&b;
//  int a_rnd = (((a >> vbl)&((int)(pow(2, Nt-vbl-1) - 1))) == ((int)pow(2, Nt-vbl-1) - 1)) ?
//              (a >> vbl) : (a >> vbl) + 1;
//  int b_rnd = (((b >> vbl)&((int)pow(2, Nt-vbl-1) - 1)) == ((int)pow(2, Nt-vbl-1) - 1)) ?
//              (b >> vbl) : (b >> vbl) + 1;
    //printf("SGLEE VBL: %d, %x, %x, %x\n", vbl, (b >> vbl), b_rnd, (int)(pow(2, Nt-vbl-1) - 1));
    //if (((b >> vbl)&((int)pow(2, Nt-vbl-1) - 1)) == ((int)pow(2, Nt-vbl-1) - 1)) cout << "SGLEE OVERFLOW" << endl;
    //if ((a >> vbl) == (pow(2, Nt) - 1)) cout << "SGLEE OVERFLOW" << endl;
//  int a_op = (iap_a >> (vbl - 1)) == 0x1 ? a_rnd : (a >> vbl);
//  int b_op = (iap_b >> (vbl - 1)) == 0x1 ? b_rnd : (b >> vbl);

    int a_op = (abs_a >> (vbl - 1)) == 0x1 ? (abs_a >> vbl) + 1 : (abs_a >> vbl);

    int b_op = (abs_b >> (vbl - 1)) == 0x1 ? (abs_b >> vbl) + 1 : (abs_b >> vbl);

    //printf("SGLEE VBL: %d, %d, %d\n", a_op, b_op, ((a_op)*(b_op)) << (2*vbl));
    int tmp = ((a_op)*(b_op)) << (2*vbl);
    // accurate part
    return (sign ? -tmp : tmp);
}
#define BT_RND
int add_calc(const int &a, const int &b, int Nia) {
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

//uncomment when you want to test
/*
int main() {
    cout <<mul_calc(7, 15, 0)<<endl;;
    cout <<mul_calc(7, 15, 1)<<endl;;
    cout <<mul_calc(7, 15, 2)<<endl;;

    cout <<add_calc(7, 15, 2)<<endl;;
    cout <<add_calc(735, 1545, 2)<<endl;;
    cout <<add_calc(735, 1545, 4)<<endl;;
    cout <<add_calc(735, 1545, 5)<<endl;;
    cout <<add_calc(735, 1545, 6)<<endl;;
}
*/
