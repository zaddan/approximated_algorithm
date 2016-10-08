#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "savejpg.h"
#include "modules_isolated.h"

using namespace std;



/** 
 * @brief This module implements the fdct
 * 
 * @param data :input data
 * @param fdtbl : 
 * @param datafloat: output
 * @param dataptr: I believe, this is only an intermediate variable
 */
void fdct(SBYTE *data,float *fdtbl, float *datafloat)
{
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp10, tmp11, tmp12, tmp13;
  float z1, z2, z3, z4, z5, z11, z13;
  //float datafloat[64];
  SBYTE ctr;
  BYTE i;
  float *dataptr; 
  for (i=0;i<64;i++) datafloat[i]=data[i];

  // Pass 1: process rows.
  dataptr=datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {
	tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];

	// Even part

    tmp10 = tmp0 + tmp3;	// phase 2
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; // phase 3
    dataptr[4] = tmp10 - tmp11;

    z1 = (tmp12 + tmp13) * ((float) 0.707106781); // c4
	dataptr[2] = tmp13 + z1;	// phase 5
    dataptr[6] = tmp13 - z1;

    // Odd part

    tmp10 = tmp4 + tmp5;	// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

	// The rotator is modified from fig 4-8 to avoid extra negations
    z5 = (tmp10 - tmp12) * ((float) 0.382683433); // c6
    z2 = ((float) 0.541196100) * tmp10 + z5; // c2-c6
    z4 = ((float) 1.306562965) * tmp12 + z5; // c2+c6
    z3 = tmp11 * ((float) 0.707106781); // c4

    z11 = tmp7 + z3;		// phase 5
    z13 = tmp7 - z3;

    dataptr[5] = z13 + z2;	// phase 6
    dataptr[3] = z13 - z2;
	dataptr[1] = z11 + z4;
    dataptr[7] = z11 - z4;

    dataptr += 8;		//advance pointer to next row
  }

  // Pass 2: process columns

  dataptr = datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {
    tmp0 = dataptr[0] + dataptr[56];
    tmp7 = dataptr[0] - dataptr[56];
    tmp1 = dataptr[8] + dataptr[48];
    tmp6 = dataptr[8] - dataptr[48];
    tmp2 = dataptr[16] + dataptr[40];
    tmp5 = dataptr[16] - dataptr[40];
    tmp3 = dataptr[24] + dataptr[32];
    tmp4 = dataptr[24] - dataptr[32];

    //Even part/

    tmp10 = tmp0 + tmp3;	//phase 2
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; // phase 3
    dataptr[32] = tmp10 - tmp11;

	z1 = (tmp12 + tmp13) * ((float) 0.707106781); // c4
    dataptr[16] = tmp13 + z1; // phase 5
    dataptr[48] = tmp13 - z1;

    // Odd part

    tmp10 = tmp4 + tmp5;	// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    // The rotator is modified from fig 4-8 to avoid extra negations.
	z5 = (tmp10 - tmp12) * ((float) 0.382683433); // c6
    z2 = ((float) 0.541196100) * tmp10 + z5; // c2-c6
    z4 = ((float) 1.306562965) * tmp12 + z5; // c2+c6
    z3 = tmp11 * ((float) 0.707106781); // c4

    z11 = tmp7 + z3;		// phase 5
    z13 = tmp7 - z3;
    dataptr[40] = z13 + z2; // phase 6
	dataptr[24] = z13 - z2;
    dataptr[8] = z11 + z4;
    dataptr[56] = z11 - z4;

    dataptr++;			// advance pointer to next column
  }

}

  

void quantization(float *fdtbl, SWORD *outdata, float *datafloat){
    // Quantize/descale the coefficients, and store into output array
    float temp;
    int i; 
    for (i = 0; i < 64; i++) {
        // Apply the quantization and scaling factor
        temp = datafloat[i] * fdtbl[i];

        //Round to nearest integer.
        //Since C does not specify the direction of rounding for negative
        //quotients, we have to force the dividend positive for portability.
        //The maximum coefficient size is +-16K (for 12-bit data), so this
        //code should work for either 16-bit or 32-bit ints.

        outdata[i] = (SWORD) ((SWORD)(temp + 16384.5) - 16384);
    }
}



void huffmanCoding(SWORD *DC, bitstring *HTDC,bitstring *HTAC,bitstring EOB, bitstring M16zeroes){

    //zigzag reordered DU which will be Huffman coded
//
 //---------guide::: the next step after quantizatin is the coding stage, where we try to compress
 //the way that we send data over.
 //---------guide:::  the reordering is for the sake of Run-length Encoding, where the consecutive zeros
 //are mainly considered and coded. To increase the number of zeros, we use zig-zag reordering
 //zigzag reorder
 BYTE end0pos;
 BYTE nrzeroes;
 BYTE nrmarker;
 BYTE startpos;
 
 int i;
 SWORD Diff;
 for (i=0;i<=63;i++) DU[zigzag[i]]=DU_DCT[i];
 Diff=DU[0]-*DC;
 *DC=DU[0];
 
 //Huffman coding  (HTDC and HTAC)
 //Encode DC
 if (Diff==0) writebits(HTDC[0]); //Diff might be 0
 else {writebits(HTDC[category[Diff]]);
     writebits(bitcode[Diff]);
 }

 //Encode ACs
 for (end0pos=63;(end0pos>0)&&(DU[end0pos]==0);end0pos--) ;

 //end0pos = first element in reverse order !=0
 if (end0pos==0) {writebits(EOB);return;}

 i=1;
 while (i<=end0pos)
  {
   startpos=i;
   for (; (DU[i]==0)&&(i<=end0pos);i++) ;
   nrzeroes=i-startpos;
   if (nrzeroes>=16) {
      for (nrmarker=1;nrmarker<=nrzeroes/16;nrmarker++) writebits(M16zeroes);
      nrzeroes=nrzeroes%16;
		     }
   writebits(HTAC[nrzeroes*16+category[DU[i]]]);writebits(bitcode[DU[i]]);
   i++;
  }
 if (end0pos!=63) writebits(EOB);

}




