#include <math.h>
#include <stdio.h>
/*
inline
float C(int u)
{
    if (u == 0)
         return (1.0f/sqrtf(2));
    else
         return 1.0f;
}

inline
int func(int x, int y, const int block[8][8])
{
	  const float PI = M_PI;
    float sum=0;
    for( int u=0; u<8; u++)
    {
         for(int v=0; v<8; v++)
         {
             sum += ( C(u) * C(v) ) * block[u][v] * cosf( ((2*x+1) * u * PI) / 16)  * cosf( ((2*y+1) * v * PI) / 16);
         }
    }         
    return (int) ((1.0/4.0) * sum);
}*/
/*
void PerformIDCT(int outBlock[8][8], const int inBlock[8][8])
{
	for(int y=0; y<8; y++)
	{
		for(int x=0; x<8; x++)
		{
			outBlock[x][y]  =  func( x, y, inBlock);
		}
	}
}


void PerformIDCT(int outBlock[8][8], const int inBlock[8][8])
{
	for(int y=0; y<8; y++)
	{
		for(int x=0; x<8; x++)
		{
			//outBlock[x][y]  =  func( x, y, inBlock);
      const float PI = M_PI;
      float sum=0;
      for( int u=0; u<8; u++)
      {
           for(int v=0; v<8; v++)
           {
             float cu= (u==0) ? 1.0f/1.4142135623730951 : 1.0;
             float cv= (v==0) ? 1.0f/1.4142135623730951 : 1.0;
             sum += ( cu * cv ) * inBlock[u][v] * cosf( ((2*x+1) * u * PI) / 16)  * cosf( ((2*y+1) * v * PI) / 16);
           }
      }         
      outBlock[x][y]= (int) ((1.0/4.0) * sum);
		}
	}
}*/
typedef float fxp_t;
void PerformIDCT(int outBlock[8][8], const int inBlock[8][8])
{
  const fxp_t coeff[64]=
  {
      0.707106781187  ,
      0.980785280403,
      0.923879532511,
      0.831469612303,
      0.707106781187,
      0.55557023302,
      0.382683432365,
      0.195090322016,
      0.707106781187,
      0.831469612303,
      0.382683432365,
      -0.195090322016,
      -0.707106781187,
      -0.980785280403,
      -0.923879532511,
      -0.55557023302,
      0.707106781187,
      0.55557023302,
      -0.382683432365,
      -0.980785280403,
      -0.707106781187,
      0.195090322016,
      0.923879532511,
      0.831469612303,
      0.707106781187,
      0.195090322016,
      -0.923879532511,
      -0.55557023302,
      0.707106781187,
      0.831469612303,
      -0.382683432365,
      -0.980785280403,
      0.707106781187,
      -0.195090322016,
      -0.923879532511,
      0.55557023302,
      0.707106781187,
      -0.831469612303,
      -0.382683432365,
      0.980785280403,
      0.707106781187,
      -0.55557023302,
      -0.382683432365,
      0.980785280403,
      -0.707106781187,
      -0.195090322016,
      0.923879532511,
      -0.831469612303,
      0.707106781187,
      -0.831469612303,
      0.382683432365,
      0.195090322016,
      -0.707106781187,
      0.980785280403,
      -0.923879532511,
      0.55557023302,
      0.707106781187,
      -0.980785280403,
      0.923879532511,
      -0.831469612303,
      0.707106781187,
      -0.55557023302,
      0.382683432365,
      -0.195090322016
  };
	for(int y=0; y<8; y++)
	{
		for(int x=0; x<8; x++)
		{
			//outBlock[x][y]  =  func( x, y, inBlock);
      const fxp_t PI = M_PI;
      fxp_t sum=0;
      for( int u=0; u<8; u++)
      {
           for(int v=0; v<8; v++)
           {
             //float cu= (u==0) ? 1.0f/1.4142135623730951 : 1.0;
             //float cv= (v==0) ? 1.0f/1.4142135623730951 : 1.0;
             //float coeff= (( cu * cv ) * cosf( ((2*x+1) * u * PI) / 16)  * cosf( ((2*y+1) * v * PI) / 16));
             sum+=coeff[x*8+u]*coeff[y*8+v]*inBlock[u][v];
             //printf("%f\n",coeff);
           }
      }         
      outBlock[x][y]= (int) ((1.0/4.0) * sum);
		}
	}
}

