#include "config.h"

//need this one
void setup_brightness_lut(bp,thresh,form)
    uchar **bp;
    int   thresh, form;
{
    int   k;
    float temp;
    int l;
    //  cout << *bp;
    //  cout<< **bp;

    //       (unsigned char *)malloc(516);
    //*bp = bpArray;
    *bp=*bp+258;
    for(k=-256;k<257;k++)
    {
        temp=((float)k)/((float)thresh);
        temp=temp*temp;
        //printf(":%f\n", temp); 
        if (form==6)
            temp=temp*temp*temp;
        temp=100.0*exp(-temp);
        //printf(":%c\n", (uchar)temp); 
        *(*bp+k)= (uchar)temp;
    }
   
}




