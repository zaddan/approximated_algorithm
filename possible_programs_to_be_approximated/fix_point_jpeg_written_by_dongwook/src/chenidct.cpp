#define LS(r,s) ((r) << (s))
#define RS(r,s) ((r) >> (s))       /* Caution with rounding... */

#define MSCALE(expr)  RS((expr),9)

/* Cos constants */
/*
#define c1d4 362L

#define c1d8 473L
#define c3d8 196L

#define c1d16 502L
#define c3d16 426L
#define c5d16 284L
#define c7d16 100L
*/
#include "mytype.h"
//  >>> math.cos(math.pi /16 *1)
const fxp_coeff_t c1d16 = 0.9807852804032304;
//  >>> math.cos(math.pi /16 *2)
const fxp_coeff_t c1d8   =0.9238795325112867;
//  >>> math.cos(math.pi /16 *3)
const fxp_coeff_t c3d16 = 0.8314696123025452;
//  >>> math.cos(math.pi /16 *4)
const fxp_coeff_t c1d4  = 0.7071067811865476;
//  >>> math.cos(math.pi /16 *5)
const fxp_coeff_t c5d16 = 0.5555702330196023;
//  >>> math.cos(math.pi /16 *6)
const fxp_coeff_t c3d8  = 0.38268343236508984;
//  >>> math.cos(math.pi /16 *7)
const fxp_coeff_t c7d16=  0.1950903220161283;

/*
 *
 * ChenIDCT() implements the Chen inverse dct. Note that there are two
 * input vectors that represent x=input, and y=output, and must be
 * defined (and storage allocated) before this routine is called.
 */

void chenidct(int x[64],int y[64], fxp_t tmp[64])
{
  int i;
  //fxp_t tmp[64];
  //short a0,a1,a2,a3;
  //short b0,b1,b2,b3;
  //fxp_t c0,c1,c2,c3;

  /* Loop over columns */

  for(i=0;i<8;i++)
  {
    fxp_t b0 = x[i+0];
    fxp_t a0 = x[i+8*1];
    fxp_t b2 = x[i+8*2];
    fxp_t a1 = x[i+8*3];
    fxp_t b1 = x[i+8*4];
    fxp_t a2 = x[i+8*5];
    fxp_t b3 = x[i+8*6];
    fxp_t a3 = x[i+8*7];

    /* Split into even mode  b0 = x0  b1 = x4  b2 = x2  b3 = x6.
       And the odd terms a0 = x1 a1 = x3 a2 = x5 a3 = x7.
     */

    fxp_t c0 = ((c7d16*a0)-(c1d16*a3));
    fxp_t c1 = ((c3d16*a2)-(c5d16*a1));
    fxp_t c2 = ((c3d16*a1)+(c5d16*a2));
    fxp_t c3 = ((c1d16*a0)+(c7d16*a3));

    /* First Butterfly on even terms.*/

    a0 = (c1d4*(b0+b1));
    a1 = (c1d4*(b0-b1));

    a2 = ((c3d8*b2)-(c1d8*b3));
    a3 = ((c1d8*b2)+(c3d8*b3));

    b0 = a0+a3;
    b1 = a1+a2;
    b2 = a1-a2;
    b3 = a0-a3;

    /* Second Butterfly */

    a0 = c0+c1;
    a1 = c0-c1;
    a2 = c3-c2;
    a3 = c3+c2;

    c0 = a0;
    c1 = (c1d4*(a2-a1));
    c2 = (c1d4*(a2+a1));
    c3 = a3;

    tmp[i+0*8]=b0+c3;
    tmp[i+1*8]=b1+c2;
    tmp[i+2*8]=b2+c1;
    tmp[i+3*8]=b3+c0;
    tmp[i+4*8]=b3-c0;
    tmp[i+5*8]=b2-c1;
    tmp[i+6*8]=b1-c2;
    tmp[i+7*8]=b0-c3;
  }

  /* Loop over rows */  

  for(i=0;i<8;i++)
  {
    fxp_dct2_t b0 = tmp[i*8+0];
    fxp_dct2_t a0 = tmp[i*8+1];
    fxp_dct2_t b2 = tmp[i*8+2];
    fxp_dct2_t a1 = tmp[i*8+3];
    fxp_dct2_t b1 = tmp[i*8+4];
    fxp_dct2_t a2 = tmp[i*8+5];
    fxp_dct2_t b3 = tmp[i*8+6];
    fxp_dct2_t a3 = tmp[i*8+7];

    /*
       Split into even mode  b0 = x0  b1 = x4  b2 = x2  b3 = x6.
       And the odd terms a0 = x1 a1 = x3 a2 = x5 a3 = x7.
     */

    fxp_dct2_t c0 = ((c7d16*a0)-(c1d16*a3));
    fxp_dct2_t c1 = ((c3d16*a2)-(c5d16*a1));
    fxp_dct2_t c2 = ((c3d16*a1)+(c5d16*a2));
    fxp_dct2_t c3 = ((c1d16*a0)+(c7d16*a3));

    /* First Butterfly on even terms.*/

    a0 = (c1d4*(b0+b1));
    a1 = (c1d4*(b0-b1));

    a2 = ((c3d8*b2)-(c1d8*b3));
    a3 = ((c1d8*b2)+(c3d8*b3));

    /* Calculate last set of b's */

    b0 = a0+a3;
    b1 = a1+a2;
    b2 = a1-a2;
    b3 = a0-a3;

    /* Second Butterfly */

    a0 = c0+c1;
    a1 = c0-c1;
    a2 = c3-c2;
    a3 = c3+c2;

    c0 = a0;
    c1 = (c1d4*(a2-a1));
    c2 = (c1d4*(a2+a1));
    c3 = a3;
    fxp_t quad=0.25;
    y[i*8+0]=(b0+c3)*quad;
    y[i*8+1]=(b1+c2)*quad;
    y[i*8+2]=(b2+c1)*quad;
    y[i*8+3]=(b3+c0)*quad;
    y[i*8+4]=(b3-c0)*quad;
    y[i*8+5]=(b2-c1)*quad;
    y[i*8+6]=(b1-c2)*quad;
    y[i*8+7]=(b0-c3)*quad;
  }

  /*
     Retrieve correct accuracy. We have additional factor
     of 16 that must be removed.
   */

}

