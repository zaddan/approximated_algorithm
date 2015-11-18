#include <ap_int.h>

#define DT(A,B,C) \
typedef ap_fixed<32,32-A, AP_RND_ZERO> fxp_coeff_t;\
typedef ap_fixed<32,32-B, AP_RND_ZERO> fxp_t;\
typedef ap_fixed<32,32-C, AP_RND_ZERO> fxp_dct2_t;
// typedef ap_fixed<32,12, AP_RND_ZERO> fxp_coeff_t;\
// typedef ap_fixed<32,12, AP_RND_ZERO> fxp_t;\
// typedef ap_fixed<32,12, AP_RND_ZERO> fxp_dct2_t;
//each index is how many variable dedicated to the decimal points
DT(3,2,2)


//DT(6,7,7)
//DT(6,7,6)
//DT(6,7,5)
//DT(6,6,7)
//DT(6,6,6)
//DT(6,6,5)
//DT(6,5,7)
//DT(6,5,6)
//DT(6,5,5)

//DT(5,7,7)
//DT(5,7,6)
//DT(5,7,5)
//DT(5,6,7)
//DT(5,6,6)
//DT(5,6,5)
//DT(5,5,7)
//DT(5,5,6)
//DT(5,5,5)
typedef ap_fixed<16+2,16>  fxp_c_t;
