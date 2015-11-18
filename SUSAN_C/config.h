#define BP_CONST 516 //migh have to modify later
//#define X_SIZE_CONST 76
//#define Y_SIZE_CONST 95
//#define IMAGE_SIZE 7220 
 #define X_SIZE_CONST 3072
 #define Y_SIZE_CONST 2048 


#define IMAGE_SIZE 6291456 


#define FILE_NAME_SIZE 100 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>    /* may want to remove this line */
#include <malloc.h>      /* may want to remove this line */

#define  exit_error(IFB,IFC) { fprintf(stderr,IFB,IFC); exit(0); }
#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )
typedef  unsigned char uchar;

void susan_thin(int r[],uchar mid[],int x_size,int y_size);
void edge_draw(uchar *in, uchar *mid,int x_size,int y_size,int drawing_mode);
void setup_brightness_lut(uchar **bp,int thresh, int form);
void susan_edges(uchar *in,int *r,uchar *mid,uchar *bp,int max_no,int x_size,int y_size);
void get_image(char filename[],unsigned char *in_global);
void put_image(char filename[],char *in);

