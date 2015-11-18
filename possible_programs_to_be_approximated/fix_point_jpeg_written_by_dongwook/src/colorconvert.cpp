#include "mytype.h"
inline unsigned char Clamp(fxp_c_t i)
{
	if (i<0)
		return 0;
	else if (i>255)
		return 255;
	else
		return i;
}   
inline unsigned char Clamp_s(short i)
{
	if (i<0)
		return 0;
	else if (i>255)
		return 255;
	else
		return i;
}   
void ConvertYCrCbtoRGB(short y[64], short cb[64], short cr[64],
							  unsigned char r[64], unsigned char g[64], unsigned char b[64])

{
  fxp_c_t c0=1.402;
  fxp_c_t c1=0.34414;
  fxp_c_t c2=0.71414;
  fxp_c_t c3=1.772;
  for(int i =0; i<64; i++)
  {
    int y_t = Clamp_s(y[i]+128);
    int cb_t = Clamp_s(cb[i]+128); 
    int cr_t = Clamp_s(cr[i]+128);
    //int y_t = y[i];
    //int cb_t = cb[i];
    //int cr_t = cr[i];
	  fxp_c_t red   = y_t + c0*(cb_t-128);
  	fxp_c_t green = y_t - c1*(cr_t-128)-c2*(cb_t-128);
	  fxp_c_t blue  = y_t + c3*(cr_t-128);

	  r[i]=  Clamp(red);
	  g[i]=  Clamp(green);
	  b[i]=  Clamp(blue);
//    printf("%d %d %d\n",y[i],cb[i],cr[i]);
  }
}
