//
//  jpegHeader.cpp
//  jpegEncoder
//
//  Created by Dongwook Lee on 10/26/13.
//  Copyright (c) 2013 Dongwook Lee. All rights reserved.
//
#include "jpegEncoder.h"
#include <fstream>
void writeHeader(
		bool color_mode,
	  int sampling_mode,
		unsigned short imagewidth,
		unsigned short imageheight,
		unsigned char lumaQtable[64],
		unsigned char chromaQtable[64],
		unsigned char Standard_DC_Luminance_NRCodes[17],
		unsigned char Standard_DC_Luminance_Values[12],
		unsigned char Standard_AC_Luminance_NRCodes[17],
		unsigned char Standard_AC_Luminance_Values[162],
		unsigned char Standard_DC_Chromiance_NRCodes[17],
		unsigned char Standard_DC_Chromiance_Values[12],
		unsigned char Standard_AC_Chromiance_NRCodes[17],
		unsigned char Standard_AC_Chromiance_Values[162],
		byte_queue_t& outqueue)
{
  int IZigzagIndex[] =
	{0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63};
	// APP0info
  unsigned char APP0info[20]=
  {
    0xff,0xd8, //Jpeg Init 0xffd8
    0xff,0xe0, //Maker
    0   ,16, // length, no thumnail
    'J','F',
		'I','F',
		'\0',
		1,1, // version hi, version low..
    0,// xyunist // no units, nomal density
    0,1,// x density
    0,1,// y density
    0,// no thumbnail , thumnail width
    0 // no thumbnail , thumnail height
  };
	for(int i=0; i<20; i++)
	{
		outqueue.write(APP0info[i]);
	}
	//DQT 
	unsigned short length = (color_mode==true) ? 132 : 67;   // = 132
	unsigned char  QTYinfo = 0;
	// = 0:  bit 0..3: number of QT = 0 (table for Y)
	//       bit 4..7: precision of QT, 0 = 8 bit

	outqueue.write(0xff);
	outqueue.write(0XDB); //maker
	outqueue.write(0);
	outqueue.write(length);
  outqueue.write(QTYinfo);
	for(int i=0; i<64; i++)
	{
		outqueue.write(lumaQtable[IZigzagIndex[i]]);
	}
	if(color_mode==true)
	{
		unsigned char QTCbinfo = 1;
		outqueue.write(QTCbinfo);
  	for(int i=0; i<64; i++)
	  {
	  	outqueue.write(chromaQtable[IZigzagIndex[i]]);
	  }
	}
	
	//Start of Frame info
	outqueue.write(0xff);
	outqueue.write(0xc0); //maker
	unsigned char soflength= (color_mode) ? 17 : 11;
	outqueue.write(0);
	outqueue.write(soflength);
	
	unsigned char precision = 8;// Should be 8: 8 bits/sample            
	unsigned char nrofcomponents = (color_mode) ? 3 : 1;
	//Should be 3: We encode a truecolor JPG
	unsigned char IdY = 1;  // = 1
	unsigned char HVY = (sampling_mode==0) ? 0x11 : ( (sampling_mode==1) ? 0x21: 0x22);
	unsigned char QTY = 0;  // Quantization Table number for Y = 0
	unsigned char IdCb = 2; // = 2
	unsigned char HVCb = 0x11;
	unsigned char QTCb = 1; // 1
	unsigned char IdCr = 3; // = 3
	unsigned char HVCr = HVCb;
	unsigned char QTCr = (color_mode) ? 1:0 ; // Normally equal to QTCb = 1
	outqueue.write(precision);
	outqueue.write(imageheight>>8);
	outqueue.write(imageheight&0xff);
	outqueue.write(imagewidth >>8);
	outqueue.write(imagewidth &0xff);
	outqueue.write(nrofcomponents);
	outqueue.write(IdY);
	outqueue.write(HVY);
	outqueue.write(QTY);
  if(color_mode==true)
	{
    outqueue.write(IdCb);
    outqueue.write(HVCb);
    outqueue.write(QTCb);
	
    outqueue.write(IdCr);
    outqueue.write(HVCr);
    outqueue.write(QTCr);
	}
  // Huffman code 
	outqueue.write(0xff);
	outqueue.write(0xc4); //maker
	unsigned short dhtlen=  (color_mode) ? 0x01A2 : 210;
	outqueue.write(dhtlen>>8); //length info
	outqueue.write(dhtlen&0xff); //length info
	outqueue.write(0); // HTYDCinfo
	for(int i=1; i<=16; i++)
	{
		outqueue.write(Standard_DC_Luminance_NRCodes[i]);
	}
	for(int i=0; i<12; i++)
	{
		outqueue.write(Standard_DC_Luminance_Values[i]);
	}
	outqueue.write(0x10); // HTYACInfo
	for(int i=1; i<=16; i++)
	{
		outqueue.write(Standard_AC_Luminance_NRCodes[i]);
	}
	for(int i=0; i<162; i++)
	{
		outqueue.write(Standard_AC_Luminance_Values[i]);
	}
	if(color_mode==true)
	{
		unsigned char HTCbDCinfo = 0x01; // bit 0..3: number of HT (0..3), for Y =0
		unsigned char HTCbACinfo = 0x11; //  = 0x11

		outqueue.write(HTCbDCinfo);
		for(int i=1; i<=16; i++)
		{
			outqueue.write(Standard_DC_Chromiance_NRCodes[i]);
		}
		for(int i=0; i<12; i++)
		{
			outqueue.write(Standard_DC_Chromiance_Values[i]);
		}
		outqueue.write(HTCbACinfo);
		for(int i=1; i<=16; i++)
		{
			outqueue.write(Standard_AC_Chromiance_NRCodes[i]);
		}
		for(int i=0; i<162; i++)
		{
			outqueue.write(Standard_AC_Chromiance_Values[i]);
		}
	}
	
  //    unsigned char marker = 0xDA;
      unsigned short soslength = (color_mode) ? 12 : 8;
      //unsigned char  nrofcomponents = (color_mode) ? 3 :1; // Should be 3: truecolor JPG
    //  unsigned char IdY = 1;
      unsigned char HTY = 0; // bits 0..3: AC table (0..3)
                    // bits 4..7: DC table (0..3)
    //  unsigned char IdCb = 2;
      unsigned char HTCb = 0x11; 
    //  unsigned char IdCr = 3;
      unsigned char HTCr = 0x11; 
      unsigned char Ss = 0, Se = 0x3F, Bf = 0; // not interesting, they should be 0,63,0
			outqueue.write(0xff);
      outqueue.write(0xDA); // maker
			outqueue.write(0x0);
			outqueue.write(soslength);
      outqueue.write(nrofcomponents);
      outqueue.write(IdY);
      outqueue.write(HTY);
			if(color_mode==true)
			{
     		outqueue.write(IdCb);
      	outqueue.write(HTCb);
      	outqueue.write(IdCr);
      	outqueue.write(HTCr);
			}
      outqueue.write(Ss);
      outqueue.write(Se);
      outqueue.write(Bf);
		


}

void test_writeHeader()
{
	unsigned char Standard_Luminance_Quantization_Table[64] = 
	{
		16,  11,  10,  16,  24,  40,  51,  61,
		12,  12,  14,  19,  26,  58,  60,  55,
		14,  13,  16,  24,  40,  57,  69,  56,
		14,  17,  22,  29,  51,  87,  80,  62,
		18,  22,  37,  56,  68, 109, 103,  77,
		24,  35,  55,  64,  81, 104, 113,  92,
		49,  64,  78,  87, 103, 121, 120, 101,
		72,  92,  95,  98, 112, 100, 103,  99
	};
	unsigned char Standard_Chromiance_Quantization_Table[64] = 
	{
		17,  18,  24,  47,  99,  99,  99,  99,
		18,  21,  26,  66,  99,  99,  99,  99,
		24,  26,  56,  99,  99,  99,  99,  99,
		47,  66,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99
	};
	unsigned char Standard_DC_Luminance_NRCodes[] = { 
		0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char Standard_DC_Luminance_Values[] = { 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	unsigned char Standard_DC_Chromiance_NRCodes[] = { 
		0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
	unsigned char Standard_DC_Chromiance_Values[] = { 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	unsigned char Standard_AC_Luminance_NRCodes[] = { 
		0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d 
	};
	unsigned char Standard_AC_Luminance_Values[] = 
	{
		0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
		0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
		0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
		0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
		0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
		0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
		0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
		0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
		0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
		0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
		0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
		0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
		0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
		0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
		0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
		0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
		0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
		0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
		0xf9, 0xfa
	};

	unsigned char Standard_AC_Chromiance_NRCodes []= { 
		0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
	unsigned char Standard_AC_Chromiance_Values []=
	{
		0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
		0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
		0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
		0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
		0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
		0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
		0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
		0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
		0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
		0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
		0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
		0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
		0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
		0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
		0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
		0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
		0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
		0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
		0xf9, 0xfa
	};
	byte_queue_t outqueue;
	writeHeader(
		true,0,
		1920,
		1080,
		Standard_Luminance_Quantization_Table,
		Standard_Chromiance_Quantization_Table,
		Standard_DC_Luminance_NRCodes,
		Standard_DC_Luminance_Values,
		Standard_AC_Luminance_NRCodes,
		Standard_AC_Luminance_Values,
		Standard_DC_Chromiance_NRCodes,
		Standard_DC_Chromiance_Values,
		Standard_AC_Chromiance_NRCodes,
		Standard_AC_Chromiance_Values,
              outqueue);
  outqueue.write(0xff);
	std::fstream outfile;
	outfile.open("/Users/dongwook/Dropbox/jpegH.bin",std::fstream::out | std::fstream::binary);
	while(!outqueue.empty())
	{
    outfile<<outqueue.read();
	}
	outfile.close();
}
