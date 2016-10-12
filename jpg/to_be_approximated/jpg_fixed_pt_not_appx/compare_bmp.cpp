/*
 * compare_bmp.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: sglee
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "savejpg.h"
#include "loadjpg.h"
#include "compare_bmp.h"

using namespace std;

static WORD Ximage1,Yimage1;// image dimensions divisible by 8
static colorRGB *RGB_buffer1; //image to be encoded

static WORD Ximage2,Yimage2;// image dimensions divisible by 8
static colorRGB *RGB_buffer2; //image to be encoded

static SBYTE YDU1[64]; // This is the Data Unit of Y after YCbCr->RGB transformation
static SBYTE CbDU1[64];
static SBYTE CrDU1[64];

static SBYTE YDU2[64]; // This is the Data Unit of Y after YCbCr->RGB transformation
static SBYTE CbDU2[64];
static SBYTE CrDU2[64];

static SDWORD YRtab1[256],YGtab1[256],YBtab1[256];
static SDWORD CbRtab1[256],CbGtab1[256],CbBtab1[256];
static SDWORD CrRtab1[256],CrGtab1[256],CrBtab1[256];

const double var_i = 1450.0;
const double var_c = 0.25*0.5;

void init_all1()
{
 precalculate_YCbCr_tables1();
}

void precalculate_YCbCr_tables1()
{
 WORD R,G,B;
 for (R=0;R<=255;R++) {YRtab1[R]=(SDWORD)(65536*0.299+0.5)*R;
			   CbRtab1[R]=(SDWORD)(65536*-0.16874+0.5)*R;
			   CrRtab1[R]=(SDWORD)(32768)*R;
			  }
 for (G=0;G<=255;G++) {YGtab1[G]=(SDWORD)(65536*0.587+0.5)*G;
			   CbGtab1[G]=(SDWORD)(65536*-0.33126+0.5)*G;
			   CrGtab1[G]=(SDWORD)(65536*-0.41869+0.5)*G;
			  }
 for (B=0;B<=255;B++) {YBtab1[B]=(SDWORD)(65536*0.114+0.5)*B;
			   CbBtab1[B]=(SDWORD)(32768)*B;
			   CrBtab1[B]=(SDWORD)(65536*-0.08131+0.5)*B;
			  }
}

void exitmessage2(char *error_message)
{
 printf("%s\n",error_message);exit(EXIT_FAILURE);
}

void load_bitmap1(char *bitmap_name, WORD *Ximage_original, WORD *Yimage_original)
{
  WORD Xdiv8,Ydiv8;
  BYTE nr_fillingbytes;//The number of the filling bytes in the BMP file
                       // (the dimension in bytes of a BMP line on the disk is divisible by 4)
  colorRGB lastcolor;
  WORD column;
  BYTE TMPBUF[256];
  WORD nrline_up,nrline_dn,nrline;
  WORD dimline;
  colorRGB *tmpline;

  FILE *fp_bitmap=fopen(bitmap_name,"rb");

  if (fp_bitmap==NULL) exitmessage2("Cannot open bitmap file.File not found ?");

  if (fread(TMPBUF,1,54,fp_bitmap)!=54)
    exitmessage2("Need a truecolor BMP to encode.");

  if ((TMPBUF[0]!='B')||(TMPBUF[1]!='M')||(TMPBUF[28]!=24))
    exitmessage2("Need a truecolor BMP to encode.");

  Ximage1=(WORD)TMPBUF[19]*256+TMPBUF[18];
  Yimage1=(WORD)TMPBUF[23]*256+TMPBUF[22];
  *Ximage_original=Ximage1;
  *Yimage_original=Yimage1; //Keep the old dimensions of the image

  if (Ximage1%8!=0)
    Xdiv8=(Ximage1/8)*8+8;
  else
    Xdiv8=Ximage1;

  if (Yimage1%8!=0)
    Ydiv8=(Yimage1/8)*8+8;
  else
    Ydiv8=Yimage1;

  // The image we encode shall be filled with the last line and the last column
  // from the original bitmap, until Ximage and Yimage are divisible by 8
  // Load BMP image from disk and complete X
  RGB_buffer1=(colorRGB *)(malloc(3*Xdiv8*Ydiv8));

  if (RGB_buffer1==NULL) exitmessage2("Not enough memory for the bitmap image.");

  if (Ximage1%4!=0) nr_fillingbytes=4-(Ximage1%4);
  else nr_fillingbytes=0;

  for (nrline=0;nrline<Yimage1;nrline++)
  {
    fread(RGB_buffer1+nrline*Xdiv8,1,Ximage1*3,fp_bitmap);
    fread(TMPBUF,1,nr_fillingbytes,fp_bitmap);
    memcpy(&lastcolor,RGB_buffer1+nrline*Xdiv8+Ximage1-1,3);

    for (column=Ximage1;column<Xdiv8;column++)
      {memcpy(RGB_buffer1+nrline*Xdiv8+column,&lastcolor,3);}
  }

  Ximage1=Xdiv8;
  dimline=Ximage1*3;tmpline=(colorRGB *)malloc(dimline);

  if (tmpline==NULL) exitmessage2("Not enough memory.");

  //Reorder in memory the inversed bitmap
  for (nrline_up=Yimage1-1,nrline_dn=0;nrline_up>nrline_dn;nrline_up--,nrline_dn++)
  {
    memcpy(tmpline,RGB_buffer1+nrline_up*Ximage1, dimline);
    memcpy(RGB_buffer1+nrline_up*Ximage1,RGB_buffer1+nrline_dn*Ximage1,dimline);
    memcpy(RGB_buffer1+nrline_dn*Ximage1,tmpline,dimline);
  }

  // Y completion:
  memcpy(tmpline,RGB_buffer1+(Yimage1-1)*Ximage1,dimline);
  for (nrline=Yimage1;nrline<Ydiv8;nrline++)
    {memcpy(RGB_buffer1+nrline*Ximage1,tmpline,dimline);}

  Yimage1=Ydiv8;
  free(tmpline);fclose(fp_bitmap);
}

void load_bitmap2(char *bitmap_name, WORD *Ximage_original, WORD *Yimage_original)
{
  WORD Xdiv8,Ydiv8;
  BYTE nr_fillingbytes;//The number of the filling bytes in the BMP file
                       // (the dimension in bytes of a BMP line on the disk is divisible by 4)
  colorRGB lastcolor;
  WORD column;
  BYTE TMPBUF[256];
  WORD nrline_up,nrline_dn,nrline;
  WORD dimline;
  colorRGB *tmpline;

  FILE *fp_bitmap=fopen(bitmap_name,"rb");

  if (fp_bitmap==NULL) exitmessage2("Cannot open bitmap file.File not found ?");
  //else cout << "File found" << endl;

  if (fread(TMPBUF,1,54,fp_bitmap)!=54)
    exitmessage2("Need a truecolor BMP to encode.");

  if ((TMPBUF[0]!='B')||(TMPBUF[1]!='M')||(TMPBUF[28]!=24))
    exitmessage2("Need a truecolor BMP to encode.");

  Ximage2=(WORD)TMPBUF[19]*256+TMPBUF[18];
  Yimage2=(WORD)TMPBUF[23]*256+TMPBUF[22];
  *Ximage_original=Ximage2;
  *Yimage_original=Yimage2; //Keep the old dimensions of the image

  if (Ximage2%8!=0)
    Xdiv8=(Ximage2/8)*8+8;
  else
    Xdiv8=Ximage2;

  if (Yimage2%8!=0)
    Ydiv8=(Yimage2/8)*8+8;
  else
    Ydiv8=Yimage2;

  // The image we encode shall be filled with the last line and the last column
  // from the original bitmap, until Ximage and Yimage are divisible by 8
  // Load BMP image from disk and complete X
  RGB_buffer2=(colorRGB *)(malloc(3*Xdiv8*Ydiv8));

  if (RGB_buffer2==NULL) exitmessage2("Not enough memory for the bitmap image.");

  if (Ximage2%4!=0) nr_fillingbytes=4-(Ximage2%4);
  else nr_fillingbytes=0;

  for (nrline=0;nrline<Yimage2;nrline++)
  {
    fread(RGB_buffer2+nrline*Xdiv8,1,Ximage2*3,fp_bitmap);
    fread(TMPBUF,1,nr_fillingbytes,fp_bitmap);
    memcpy(&lastcolor,RGB_buffer2+nrline*Xdiv8+Ximage2-1,3);

    for (column=Ximage2;column<Xdiv8;column++)
      {memcpy(RGB_buffer2+nrline*Xdiv8+column,&lastcolor,3);}
  }

  Ximage2=Xdiv8;
  dimline=Ximage2*3;tmpline=(colorRGB *)malloc(dimline);

  if (tmpline==NULL) exitmessage2("Not enough memory.");

  //Reorder in memory the inversed bitmap
  for (nrline_up=Yimage2-1,nrline_dn=0;nrline_up>nrline_dn;nrline_up--,nrline_dn++)
  {
    memcpy(tmpline,RGB_buffer2+nrline_up*Ximage2, dimline);
    memcpy(RGB_buffer2+nrline_up*Ximage2,RGB_buffer2+nrline_dn*Ximage2,dimline);
    memcpy(RGB_buffer2+nrline_dn*Ximage2,tmpline,dimline);
  }

  // Y completion:
  memcpy(tmpline,RGB_buffer2+(Yimage2-1)*Ximage2,dimline);
  for (nrline=Yimage2;nrline<Ydiv8;nrline++)
    {memcpy(RGB_buffer2+nrline*Ximage2,tmpline,dimline);}

  Yimage2=Ydiv8;
  free(tmpline);fclose(fp_bitmap);
}

void load_data_units_from_RGB_buffer_cmp(WORD xpos,WORD ypos)
{
 BYTE x,y;
 BYTE pos=0;
 DWORD location;
 BYTE R,G,B;
 location=ypos*Ximage1+xpos;
 for (y=0;y<8;y++)
 {
  for (x=0;x<8;x++)
   {
    R=RGB_buffer1[location].R;
    G=RGB_buffer1[location].G;
    B=RGB_buffer1[location].B;
    YDU1[pos]=Y1(R,G,B);
    CbDU1[pos]=Cb1(R,G,B);
    CrDU1[pos]=Cr1(R,G,B);

    R=RGB_buffer2[location].R;
    G=RGB_buffer2[location].G;
    B=RGB_buffer2[location].B;
    YDU2[pos]=Y1(R,G,B);
    CbDU2[pos]=Cb1(R,G,B);
    CrDU2[pos]=Cr1(R,G,B);

    location++;pos++;
   }
  location+=Ximage1-8;
 }
}

void main_compare()
{
 SWORD DCY=0,DCCb=0,DCCr=0; //DC coefficients used for differential encoding
 WORD xpos,ypos;

 int k = 0;
 double td_mse_y;
 double td_mse_cb;
 double td_mse_cr;
 double blk_pwr_y;
 double blk_pwr_cb;
 double blk_pwr_cr;
 double td_mse_sum;
 double td_mse_sum2 = 0;

 for (ypos=0;ypos<Yimage1;ypos+=8) {
  for (xpos=0;xpos<Ximage1;xpos+=8)
   {
    load_data_units_from_RGB_buffer_cmp(xpos,ypos);

    td_mse_sum = 0;
    for (int j=0;j<64;j++) {
      td_mse_y = (YDU1[j]-YDU2[j])*(YDU1[j]-YDU2[j]);
      td_mse_cr = (CrDU1[j]-CrDU2[j])*(CrDU1[j]-CrDU2[j]);
      td_mse_cb = (CbDU1[j]-CbDU2[j])*(CbDU1[j]-CbDU2[j]);
      td_mse_sum += (td_mse_y + td_mse_cr + td_mse_cb)/3.0;
    }

    td_mse_sum2 += td_mse_sum/64.0;
    k=k+1;
   }
  }
  double psnr_cal = 10 * log10( 255.0*255.0 / (td_mse_sum2/(float)k));

  cout << "PSNR_CAL: " << psnr_cal << endl;

}

float main_compare_ret()
{
 SWORD DCY=0,DCCb=0,DCCr=0; //DC coefficients used for differential encoding
 WORD xpos,ypos;

 int k = 0;
 double td_mse_y;
 double td_mse_cb;
 double td_mse_cr;
 double blk_pwr_y;
 double blk_pwr_cb;
 double blk_pwr_cr;
 double td_mse_sum;
 double td_mse_sum2 = 0;

 for (ypos=0;ypos<Yimage1;ypos+=8) {
  for (xpos=0;xpos<Ximage1;xpos+=8)
   {
    load_data_units_from_RGB_buffer_cmp(xpos,ypos);

    td_mse_sum = 0;
    for (int j=0;j<64;j++) {
      td_mse_y = (YDU1[j]-YDU2[j])*(YDU1[j]-YDU2[j]);
      td_mse_cr = (CrDU1[j]-CrDU2[j])*(CrDU1[j]-CrDU2[j]);
      td_mse_cb = (CbDU1[j]-CbDU2[j])*(CbDU1[j]-CbDU2[j]);
      td_mse_sum += (td_mse_y + td_mse_cr + td_mse_cb)/3.0;
    }

    td_mse_sum2 += td_mse_sum/64.0;
    k=k+1;
   }
  }
  double psnr_cal = 10 * log10( 255.0*255.0 / (td_mse_sum2/(float)k));

  //cout << "PSNR_CAL: " << psnr_cal << endl;

  return psnr_cal;
}

void comparebmp(char *BmpFile, char *BmpFile_ref)
{
  char BMP_filename[64];
  char REF_filename[64];
   strcpy(BMP_filename, BmpFile);
  strcpy(REF_filename, BmpFile_ref);

  WORD Ximage_original,Yimage_original;  //the original image dimensions,
                      // before we made them divisible by 8
  WORD Ximage_original_ref,Yimage_original_ref;  //the original image dimensions,

  bitstring fillbits; //filling bitstring for the bit alignment of the EOI marker

  init_all1();

  load_bitmap1(BMP_filename, &Ximage_original, &Yimage_original);
  load_bitmap2(REF_filename, &Ximage_original, &Yimage_original);

  main_compare();

  free(RGB_buffer1);
  free(RGB_buffer2);
}

float comparebmp_ret(char *BmpFile, char *BmpFile_ref)
{
  char BMP_filename[64];
  char REF_filename[64];
   strcpy(BMP_filename, BmpFile);
  strcpy(REF_filename, BmpFile_ref);

  WORD Ximage_original,Yimage_original;  //the original image dimensions,
                      // before we made them divisible by 8
  WORD Ximage_original_ref,Yimage_original_ref;  //the original image dimensions,

  bitstring fillbits; //filling bitstring for the bit alignment of the EOI marker

  init_all1();

  load_bitmap1(BMP_filename, &Ximage_original, &Yimage_original);
  load_bitmap2(REF_filename, &Ximage_original, &Yimage_original);

  //main_compare();
  float psnr_cal = main_compare_ret();

  free(RGB_buffer1);
  free(RGB_buffer2);

  return psnr_cal;
}


