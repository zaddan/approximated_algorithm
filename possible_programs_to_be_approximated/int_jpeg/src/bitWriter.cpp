//
//  bitWriter.cpp
//  jpegEncoder
//
//  Created by Dongwook Lee on 10/26/13.
//  Copyright (c) 2013 Dongwook Lee. All rights reserved.
//

#include "jpegEncoder.h"
#include <stdio.h>
#include <stdlib.h>

void bitWriter(bits_t bitqueue[MAX_HUFF], bitlen_t len_queue[MAX_HUFF], unsigned char& len, bool flush, byte_queue_t& memq)
{
	unsigned int lmask[] = {
		0x0000,
		0x0001, 0x0003, 0x0007, 0x000f,
		0x001f, 0x003f, 0x007f, 0x00ff,
		0x01ff, 0x03ff, 0x07ff, 0x0fff,
		0x1fff, 0x3fff, 0x7fff, 0xffff
	};
	//static ap_uint<32> buf;
	static unsigned int buf;
	static unsigned int buf_flush;
	static unsigned int left_bits=32;
	static unsigned int memPtr=0;
	if(flush==false)
	{
	for(int i=0; i<MAX_HUFF && i<len; i++)
	{
#pragma HLS_LOOP_PIPELINE
		bits_t code=bitqueue[i];
		bitlen_t n=len_queue[i];
		//if(n==0) continue;
		code &= lmask[n];
		if(left_bits<24)
		{
			//memq.write(buf.range(31,24));
			unsigned char tmp=(buf>>24)&0xff;
			memq.write(tmp);
			if(tmp==0xff)
			{
				memq.write(0);
			}
			left_bits+=8;
			buf<<=8;
		}

		if(left_bits<24)
		{
			//memq.write(buf.range(31,24));
			unsigned char tmp=(buf>>24)&0xff;
			memq.write(tmp);
			if(tmp==0xff)
			{
				memq.write(0);
			}
			left_bits+=8;
			buf<<=8;
		}
		buf|=(code<<(left_bits-n));
		left_bits-=n;
		//if(left_bits<0) {printf("%d",left_bits);exit(0);}
	}
	}
	else
	{
		for(int i=0;i<left_bits;i+=8)
		{
#pragma HLS LOOP_TRIPCOUNT max=4
#pragma HLS_LOOP_PIPELINE
			unsigned char tmp=(buf>>24)&0xff;
			memq.write(tmp);
			if(tmp==0xff)
				memq.write(tmp);
			buf<<=8;
		}
		memq.write(0xff);
		memq.write(0xd9);
		left_bits=32;
		return;
	}

}

/*

	 void test_bitWrite()
	 {
	 bits_queue_t bitqueue;
	 bitlen_queue_t len_queue;
	 bool flush=true;
	 byte_queue_t memq;
	 bitqueue.write(0xff);
	 len_queue.write(16);
	 bitqueue.write(0x00);
	 len_queue.write(15);
	 bitqueue.write(0x01);
	 len_queue.write(16);

	 bitWriter(bitqueue, len_queue, flush, memq);

	 }*/
