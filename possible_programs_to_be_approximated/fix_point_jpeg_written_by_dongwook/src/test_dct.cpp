#include <stdio.h>
#define _CHENDCT_
#ifndef _CHENDCT_
void 
PerformIDCT(int outBlock[8][8], const int inBlock[8][8]);

int main(int argc, char** argv)
{
  int outBlock[8][8];
  int outBlock_ref[8][8];
  int inBlock[8][8];
  FILE* input=fopen("dct_vector","r");
  int loop=0;
  int go=0;
  for (int z=0; z<4096*3; z++) //4096*3
  { 
    for (int i=0; i<8; i++)
    {
      for(int j=0; j<8; j++)
      {
        go=fscanf(input,"%d %d\n",&inBlock[i][j], &outBlock_ref[i][j]);
      }
    }
    PerformIDCT(outBlock, inBlock);
    for(int i=0; i<8;i++)
    {
      for(int j=0; j<8; j++)
      {
        if(outBlock[i][j]!=outBlock_ref[i][j])
        {
          printf("error: %d %d: %d %d\n",i,j,outBlock[i][j],outBlock_ref[i][j]);
        }
      }
    }
  }
  fclose(input);
  return 0;
}
#else
#include "mytype.h"
void chenidct(int outBlock[64], int inBlock[64], fxp_t tmp[64]);

int main(int argc, char** argv)
{
  int outBlock[8][8];
  int outBlock_ref[8][8];
  int inBlock[8][8];
  int inBlock_chen[8*8];
  int outBlock_chen[8*8];
  fxp_t tmp[64];
  FILE* input=fopen("1_out","r");
  int loop=0;
  int go=0;
  double diff;
  double diff_sum=0.0;
  double diff_2_sum=0.0;
  for (int z=0; z<4096*3; z++) //4096*3
  { 
    for (int i=0; i<8; i++)
    {
      for(int j=0; j<8; j++)
      {
        go=fscanf(input,"%d %d\n",&inBlock[i][j], &outBlock_ref[i][j]);
        inBlock_chen[i*8+j]=inBlock[i][j];
      }
    }
    chenidct(inBlock_chen, outBlock_chen, tmp);
    for(int i=0; i<8;i++)
    {
      for(int j=0; j<8; j++)
      {/*
        if(outBlock_chen[i*8+j]!=outBlock_ref[i][j])
        {
          printf("error: %d %d: %d %d\n",i,j,outBlock_chen[i*8+j],outBlock_ref[i][j]);
        }*/
//        printf("%d %d\n",inBlock[i][j],outBlock_ref[i][j]);
//        printf("%d   \n",inBlock[i][j]);
        diff=outBlock_chen[i*8+j]-outBlock_ref[i][j];
        diff_sum+=diff;
        diff_2_sum+=(diff*diff);
      }
    }
  }
  printf("%f\n",diff_sum/4096/3/64);
  printf("%f\n",diff_2_sum/4096/3/64);
  double m2=(diff_sum/4096/3/64);
  printf("%f\n",diff_2_sum/4096/3/64-m2*m2);
  fclose(input);
  return 0;
}
#endif
