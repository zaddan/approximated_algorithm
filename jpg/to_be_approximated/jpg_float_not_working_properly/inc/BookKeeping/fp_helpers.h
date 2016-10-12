#define floatingPtWidth 32
#define MANTISA_WIDTH 23
#define bias 127

#ifndef FP_HELPERS
#define FP_HELPERS
typedef struct{
    long Exp, Sign, Mantisa; 
    long MantisaWithOne;
}fpType;

unsigned getNumberOfDigits (long unsigned i);
void getFPComponents(float number, fpType &num);
float convertFPCompToFP(fpType num);
void normalizeMul(fpType &resultNum);
void normalizeAdd(fpType &resultNum);
#endif
