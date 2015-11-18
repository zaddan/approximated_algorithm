#include <stdio.h>
#include "mytype.h"
void ConvertYCrCbtoRGB(short y[64], short cb[64], short cr[64],
							  unsigned char r[64], unsigned char g[64], unsigned char b[64]);
void chenidct(int outBlock[64], int inBlock[64], fxp_t tmp[64]);
int main(int argc, char** argv)
{
  int outBlock[8][8];
  int outBlock_ref[8][8];
  int inBlock[8][8];
  int inBlock_chen[8*8];
  int outBlock_chen[3][8*8];
  short c[3][8*8];
  unsigned char r[64];
  unsigned char g[64];
  unsigned char b[64];
  int r_ref[64];
  int g_ref[64];
  int b_ref[64];
  fxp_t tmp[64];
  FILE* input=fopen("testvector/dct_50_in","r");
  FILE* ref=fopen("testvector/color_1_out","r");
  double diff;
  double diff_sum=0.0;
  double diff_2_sum=0.0;
  for (int z=0; z<4096; z++) //4096*3
  { 
    for (int k=0; k<3; k++)
    {
      for (int i=0; i<8; i++)
      {
        for(int j=0; j<8; j++)
        {
          fscanf(input,"%d\n",&inBlock[i][j]);
          inBlock_chen[i*8+j]=inBlock[i][j];
        }
      }
      chenidct(inBlock_chen, outBlock_chen[k], tmp);
      for(int i=0; i<64; i++)
      {
        c[k][i]=outBlock_chen[k][i];
      }
    }
    for(int i=0; i<64; i++)
    {
      fscanf(ref,"%d %d %d\n",&r_ref[i],&g_ref[i],&b_ref[i]);
    }
    ConvertYCrCbtoRGB(c[0],c[2],c[1],r,g,b);
    for(int i=0; i<8;i++)
    {
      for(int j=0; j<8; j++)
      {
        diff=r[i*8+j]-r_ref[i*8+j];
        diff_sum+=diff;
        diff_2_sum+=(diff*diff);
//       if(diff>5)
//         printf("r %d %d %d\n",i,j,diff);
        diff=g[i*8+j]-g_ref[i*8+j];
        diff_sum+=diff;
        diff_2_sum+=(diff*diff);
//       if(diff>5)
//         printf("g %d %d %d\n",i,j,diff);

        diff=b[i*8+j]-b_ref[i*8+j];
        diff_sum+=diff;
        diff_2_sum+=(diff*diff);
//       if(diff>5)
//         printf("b %d %d %d\n",i,j,diff);
/*        printf("%d    %d    %d    %d     %d     %d\n",
            c[0][i*8+j],
            c[2][i*8+j],
            c[1][i*8+j],
            r[i*8+j],
            g[i*8+j],
            b[i*8+j]);*/
        printf("%d,%d,%d\n", r[i*8+j],b[i*8+j],g[i*8+j]);
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
