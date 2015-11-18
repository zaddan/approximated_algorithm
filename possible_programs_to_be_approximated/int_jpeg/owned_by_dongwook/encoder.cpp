//
//  encoder.cpp
//  jpegEncoder
//
//  Created by Dongwook Lee on 10/26/13.
//  Copyright (c) 2013 Dongwook Lee. All rights reserved.
//

#include "jpegEncoder.h"
#include <stdio.h>

void blockEncoder(
		bool init,
		bool color_mode,
		int  sampling,
		unsigned short imagewidth,
		unsigned short imageheight,
		unsigned char Standard_Luminance_Quantization_Table[64] ,
		unsigned char Standard_Chromiance_Quantization_Table0[64] ,
		unsigned char Standard_Chromiance_Quantization_Table1[64] ,
		const unsigned char buffer[MAX_ROWBUF_WIDTH*3*8],

		pixel_queue_t& queue,bool flush

		)
{


	yuv_t yQ[64];
	yuv_t uQ[64];
	yuv_t vQ[64];
	freq_t yfrqQ[64];
	freq_t ufrqQ[64];
	freq_t vfrqQ[64];
	freq_t yquantQ[64];
	freq_t uquantQ[64];
	freq_t vquantQ[64];
	bits_t yBitQ[MAX_HUFF];
	bits_t uBitQ[MAX_HUFF];
	bits_t vBitQ[MAX_HUFF];
	bits_t emptyBitQ[1];

	bitlen_t yBitLenQ[MAX_HUFF];
	bitlen_t uBitLenQ[MAX_HUFF];
	bitlen_t vBitLenQ[MAX_HUFF];
	bitlen_t emptyBitLenQ[1];
	unsigned char yLen, uLen, vLen, emptyLen;
	emptyLen=1;
	bool ydcten, udcten, vdcten, yqen, vqen, uqen, yhen, uhen, vhen;
	colorBuffer( color_mode, sampling, imagewidth, buffer, yQ, uQ, vQ, ydcten, udcten, vdcten);
	dct(ydcten, yQ,yfrqQ, yqen);
	dct(udcten, uQ,ufrqQ, vqen);
	dct(vdcten, vQ,vfrqQ, uqen);
	quant_y(yqen, Standard_Luminance_Quantization_Table, yfrqQ, yquantQ  , yhen);
	quant_u(vqen, Standard_Chromiance_Quantization_Table0, ufrqQ, uquantQ, uhen);
	quant_v(uqen, Standard_Chromiance_Quantization_Table1, vfrqQ, vquantQ, vhen);

	huffman_Y (yhen, yquantQ, yBitQ, yBitLenQ, yLen);
	huffman_UV(uhen, uquantQ, uBitQ, uBitLenQ,U_DATA,uLen);
	huffman_UV(vhen, vquantQ, vBitQ, vBitLenQ,V_DATA,vLen);

	bitWriter(yBitQ, yBitLenQ,yLen,false, queue);
	bitWriter(uBitQ, uBitLenQ,uLen,false, queue);
	bitWriter(vBitQ, vBitLenQ,vLen,false, queue);
	bitWriter(emptyBitQ, emptyBitLenQ,emptyLen,false, queue);
}

void encoder(bool init, bool color_mode, int sampling, unsigned short imagewidth, unsigned short imageheight, 
int mx, int my, int totalMBs,
unsigned char lumaQtable[64],
unsigned char chromaQtable0[64],
unsigned char chromaQtable1[64],
unsigned char DC_Luminance_NRCodes[17],
unsigned char DC_Luminance_Values[12],
unsigned char AC_Luminance_NRCodes[17],
unsigned char AC_Luminance_Values[162],
unsigned char DC_Chromiance_NRCodes[17],
unsigned char DC_Chromiance_Values[12],
unsigned char AC_Chromiance_NRCodes[17],
unsigned char AC_Chromiance_Values[162],
const unsigned char buffer[MAX_ROWBUF_WIDTH*3*16], pixel_queue_t& queue,bool flush_all)
{



	static unsigned int mbCounter=0;
	static unsigned int totalMB=0;
	bool flush=false;

	if(init==true)
	{
		totalMBs=totalMB;
		writeHeader(
				color_mode,
				sampling,
				imagewidth,
				imageheight,
				lumaQtable,
				chromaQtable0,
				DC_Luminance_NRCodes,
				DC_Luminance_Values,
				AC_Luminance_NRCodes,
				AC_Luminance_Values,
				DC_Chromiance_NRCodes,
				DC_Chromiance_Values,
				AC_Chromiance_NRCodes,
				AC_Chromiance_Values,
				queue);
	}
	else {

		for(int i=0; i<mx ;i++)
		{
			mbCounter++;
			if(totalMB==mbCounter)
			{
				flush=true;
			}
			blockEncoder(
					init,
					color_mode,sampling,
					imagewidth,
					imageheight,
					lumaQtable,
					chromaQtable0,
					chromaQtable1,
					buffer,
					queue,
					flush
					);

		}
	}
}
