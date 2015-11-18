//
//  huffman.cpp
//  jpegEncoder
//
//  Created by Dongwook Lee on 10/25/13.
//  Copyright (c) 2013 Dongwook Lee. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include "jpegEncoder.h"
void huffman_Y(bool hen,
		freq_t inqueue[64], bits_t bitqueue[MAX_HUFF], bitlen_t bitlenqueue[MAX_HUFF], unsigned char& len)
{
	len =0;
	
#pragma HLS function_instantiate variable=datatype
	static freq_t lastYDC=0;


	unsigned short lumaACEhuff_ehufco[257]={10,0,1,4,11,26,120,248,1014,65410,65411,0,0,0,0,0,0,12,27,121,502,2038,65412,65413,65414,65415,65416,0,0,0,0,0,0,28,249,1015,4084,65417,65418,65419,65420,65421,65422,0,0,0,0,0,0,58,503,4085,65423,65424,65425,65426,65427,65428,65429,0,0,0,0,0,0,59,1016,65430,65431,65432,65433,65434,65435,65436,65437,0,0,0,0,0,0,122,2039,65438,65439,65440,65441,65442,65443,65444,65445,0,0,0,0,0,0,123,4086,65446,65447,65448,65449,65450,65451,65452,65453,0,0,0,0,0,0,250,4087,65454,65455,65456,65457,65458,65459,65460,65461,0,0,0,0,0,0,504,32704,65462,65463,65464,65465,65466,65467,65468,65469,0,0,0,0,0,0,505,65470,65471,65472,65473,65474,65475,65476,65477,65478,0,0,0,0,0,0,506,65479,65480,65481,65482,65483,65484,65485,65486,65487,0,0,0,0,0,0,1017,65488,65489,65490,65491,65492,65493,65494,65495,65496,0,0,0,0,0,0,1018,65497,65498,65499,65500,65501,65502,65503,65504,65505,0,0,0,0,0,0,2040,65506,65507,65508,65509,65510,65511,65512,65513,65514,0,0,0,0,0,0,65515,65516,65517,65518,65519,65520,65521,65522,65523,65524,0,0,0,0,0,2041,65525,65526,65527,65528,65529,65530,65531,65532,65533,65534,0,0,0,0,0,0};
	unsigned short lumaACEhuff_ehufsi[257]={4,2,2,3,4,5,7,8,10,16,16,0,0,0,0,0,0,4,5,7,9,11,16,16,16,16,16,0,0,0,0,0,0,5,8,10,12,16,16,16,16,16,16,0,0,0,0,0,0,6,9,12,16,16,16,16,16,16,16,0,0,0,0,0,0,6,10,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,11,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,12,16,16,16,16,16,16,16,16,0,0,0,0,0,0,8,12,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,15,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,10,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,10,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,11,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,16,16,16,16,16,16,16,16,16,16,0,0,0,0,0,11,16,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0};
	unsigned short lumaDCEhuff_ehufsi[257]={2,3,3,3,3,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned short lumaDCEhuff_ehufco[257]={0,2,3,4,5,6,14,30,62,126,254,510,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int csize[] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
	int diff=0;
	int cofac;
	int s;
	int lastDC;
	bitlen_t blen;
	bits_t bits;

	if(hen==false) return;
	//DC Encoding
	lastDC=lastYDC;



	freq_t dc=inqueue[0];
	lastYDC=dc;


	diff=dc-lastDC;
	if(diff<0)
		cofac = -diff;
	else
		cofac = diff;
	if (cofac < 256) {
		s = csize[cofac];
	}
	else {
		cofac = cofac >> 8;
		s = csize[cofac] + 8;
	}
	//FILE_LOG(logDEBUG)<<"DCEhuff_ehufsi: "<<DCEhuff_ehufsi[s];

	bits=lumaDCEhuff_ehufco[s];
	blen=lumaDCEhuff_ehufsi[s];
	if(blen!=0)
	{
		bitlenqueue[len]=(blen);
		bitqueue[len]=(bits);
		len++;if(len>MAX_HUFF) printf("error");
	}
	//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
	if (diff < 0) {
		diff--;
	}
	if(s!=0)
	{
		bitlenqueue[len]=(s);
		bitqueue[len]=((diff&0xffff));
		len++;if(len>MAX_HUFF) printf("error");
	}
	//if(datatype!=Y_DATA) printf("%d %d\n",s,diff&0xffff);
	//AC Encoding
	//int i, k, r, ssss, cofac;
	int i,r,k,inputAC;
	r=0;
	for (k=1; k<64; k++) {
#pragma HLS PIPELINE II=1
		inputAC=inqueue[k];
		if(inputAC<0)
			cofac = -inputAC;
		else
			cofac = inputAC;
		if (cofac < 256)
			s = csize[cofac];
		else {
			cofac = cofac >> 8;
			s = csize[cofac] + 8;
		}
		if (inputAC == 0) {
			if (k == 63) {
				bits=lumaACEhuff_ehufco[0];
				blen=lumaACEhuff_ehufsi[0];
				bitlenqueue[len]=(blen);
				bitqueue[len]=(bits);
				len++;if(len>MAX_HUFF) printf("error");
				//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
			}
			else
			{
				r++;
				if(r%16==0)
				{
					bits=lumaACEhuff_ehufco[240];
					blen=lumaACEhuff_ehufsi[240];
					bitlenqueue[len]=(blen);
					bitqueue[len]=(bits);
					len++;if(len>MAX_HUFF) printf("error");
					//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
					r = 0;
				}
			}
		}
		else {
			i = 16 * r + s;
			r = 0;
			bits=lumaACEhuff_ehufco[i];
			blen=lumaACEhuff_ehufsi[i];

			if(blen!=0)
			{
				bitlenqueue[len]=(blen);
				bitqueue[len]=(bits);
				len++;if(len>MAX_HUFF) printf("error");
			}
			//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
			if (inputAC < 0)
				inputAC=inputAC-1;

			if(s!=0)
			{
				bitlenqueue[len]=(s);
				bitqueue[len]=(inputAC&0xffff);
				len++;if(len>MAX_HUFF) printf("error");
			}
			//if(datatype!=Y_DATA) printf("%d %d\n",s,inputAC&0xffff);
		}
	}
}
void huffman_UV(bool hen,
		freq_t inqueue[64], bits_t bitqueue[MAX_HUFF], bitlen_t bitlenqueue[MAX_HUFF], yuv_t datatype, unsigned char& len)
{

#pragma HLS function_instantiate variable=datatype

	static freq_t lastUDC=0;
	static freq_t lastVDC=0;



	unsigned short chromaDCEhuff_ehufsi[257]={2,2,2,3,4,5,6,7,8,9,10,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	unsigned short chromaDCEhuff_ehufco[257]={0,1,2,6,14,30,62,126,254,510,1022,2046,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	unsigned short chromaACEhuff_ehufsi[257]={2,2,3,4,5,5,6,7,9,10,12,0,0,0,0,0,0,4,6,8,9,11,12,16,16,16,16,0,0,0,0,0,0,5,8,10,12,15,16,16,16,16,16,0,0,0,0,0,0,5,8,10,12,16,16,16,16,16,16,0,0,0,0,0,0,6,9,16,16,16,16,16,16,16,16,0,0,0,0,0,0,6,10,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,11,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,11,16,16,16,16,16,16,16,16,0,0,0,0,0,0,8,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,11,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,14,16,16,16,16,16,16,16,16,16,0,0,0,0,0,10,15,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0};

	unsigned short chromaACEhuff_ehufco[257]={0,1,4,10,24,25,56,120,500,1014,4084,0,0,0,0,0,0,11,57,246,501,2038,4085,65416,65417,65418,65419,0,0,0,0,0,0,26,247,1015,4086,32706,65420,65421,65422,65423,65424,0,0,0,0,0,0,27,248,1016,4087,65425,65426,65427,65428,65429,65430,0,0,0,0,0,0,58,502,65431,65432,65433,65434,65435,65436,65437,65438,0,0,0,0,0,0,59,1017,65439,65440,65441,65442,65443,65444,65445,65446,0,0,0,0,0,0,121,2039,65447,65448,65449,65450,65451,65452,65453,65454,0,0,0,0,0,0,122,2040,65455,65456,65457,65458,65459,65460,65461,65462,0,0,0,0,0,0,249,65463,65464,65465,65466,65467,65468,65469,65470,65471,0,0,0,0,0,0,503,65472,65473,65474,65475,65476,65477,65478,65479,65480,0,0,0,0,0,0,504,65481,65482,65483,65484,65485,65486,65487,65488,65489,0,0,0,0,0,0,505,65490,65491,65492,65493,65494,65495,65496,65497,65498,0,0,0,0,0,0,506,65499,65500,65501,65502,65503,65504,65505,65506,65507,0,0,0,0,0,0,2041,65508,65509,65510,65511,65512,65513,65514,65515,65516,0,0,0,0,0,0,16352,65517,65518,65519,65520,65521,65522,65523,65524,65525,0,0,0,0,0,1018,32707,65526,65527,65528,65529,65530,65531,65532,65533,65534,0,0,0,0,0,0};
	int csize[] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
	int diff=0;
	int cofac;
	int s;
	int lastDC;
	bitlen_t blen;
	bits_t bits;
	//DC Encoding
  len=0;
	if(hen==false) return;
	freq_t dc=inqueue[0];
	if(datatype==U_DATA)
	{
		lastDC=lastUDC;
		lastUDC=dc;
	}
	else
	{
		lastDC=lastVDC;
		lastVDC=dc;
	}
	diff=dc-lastDC;
	if(diff<0)
		cofac = -diff;
	else
		cofac = diff;
	if (cofac < 256) {
		s = csize[cofac];
	}
	else {
		cofac = cofac >> 8;
		s = csize[cofac] + 8;
	}
	//FILE_LOG(logDEBUG)<<"DCEhuff_ehufsi: "<<DCEhuff_ehufsi[s];

	bits=chromaDCEhuff_ehufco[s];
	blen=chromaDCEhuff_ehufsi[s];
	if(blen!=0)
	{
		bitlenqueue[len]=(blen);
		bitqueue[len]=(bits);
		len++;if(len>MAX_HUFF) printf("error");
	}
	//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);

	if (diff < 0) {
		diff--;
	}
	if(s!=0)
	{
		bitlenqueue[len]=(s);
		bitqueue[len]=((diff&0xffff));
		len++;if(len>MAX_HUFF) printf("error");
	}
	//if(datatype!=Y_DATA) printf("%d %d\n",s,diff&0xffff);
	//AC Encoding
	//int i, k, r, ssss, cofac;
	int i,r,k,inputAC;
	r=0;
	for (k=1; k<64; k++) {
#pragma HLS pipeline II=1
		inputAC=inqueue[k];
		if(inputAC<0)
			cofac = -inputAC;
		else
			cofac = inputAC;
		if (cofac < 256)
			s = csize[cofac];
		else {
			cofac = cofac >> 8;
			s = csize[cofac] + 8;
		}
		if (inputAC == 0) {
			if (k == 63) {
				bits=chromaACEhuff_ehufco[0];
				blen=chromaACEhuff_ehufsi[0];
				bitlenqueue[len]=(blen);
				bitqueue[len]=(bits);
				len++;if(len>MAX_HUFF) printf("error");
				//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
			}
			else
			{
				r++;
				if(r%16==0)
				{
					bits=chromaACEhuff_ehufco[240];
					blen=chromaACEhuff_ehufsi[240];
					bitlenqueue[len]=(blen);
					bitqueue[len]=(bits);
					len++;if(len>MAX_HUFF) printf("error");
					//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
					r = 0;
				}
			}
		}
		else {
			i = 16 * r + s;
			r = 0;
			bits=chromaACEhuff_ehufco[i];
			blen=chromaACEhuff_ehufsi[i];
			if(blen!=0)
			{
				bitlenqueue[len]=(blen);
				bitqueue[len]=(bits);
				len++;if(len>MAX_HUFF) printf("error");
			}
			//if(datatype!=Y_DATA) printf("%d %d\n",blen,bits);
			if (inputAC < 0)
				inputAC=inputAC-1;
			if(s!=0)
			{
				bitlenqueue[len]=(s);
				bitqueue[len]=(inputAC&0xffff);
				len++;if(len>MAX_HUFF) printf("error");
			}
			//if(datatype!=Y_DATA) printf("%d %d\n",s,inputAC&0xffff);
		}
	}
//	if(len>64) printf("ERERER");
}
/*
void test_huffman()
{
	freq_queue_t inqueue;
	bits_queue_t bitqueue;
	bitlen_queue_t bitlenqueue;
	inqueue.write(31);
	for(int i=1; i<64; i++)
	{
		inqueue.write(0);
	}
	inqueue.write(31);
	for(int i=1; i<64; i++)
	{
		inqueue.write(0);
	}
	//FILE_LOG(logDEBUG)<<"test";
	huffman_Y(Y_DATA,
			inqueue,
			bitqueue,
			bitlenqueue);
	huffman_Y(Y_DATA,
			inqueue,
			bitqueue,
			bitlenqueue);


}*/
