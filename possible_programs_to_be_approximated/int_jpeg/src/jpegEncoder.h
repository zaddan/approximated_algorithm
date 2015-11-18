#ifndef __JPEG_ENCODER_H__
#define __JPEG_ENCODER_H__
//#include "log.h"
//#define SMALL_IMAGE
#define MAX_HUFF 128
#define MAX_ROWBUF_WIDTH 1920
//#define __HLS__
enum YUV {Y_DATA, U_DATA, V_DATA};
#ifndef __HLS__
#include "c_queue.hpp"
//! type for 8bit pixel data
typedef unsigned char   pixel_t;
//! type for 8bit yuv data
typedef char            yuv_t;
//! type for 12bit dct output data  
typedef short           freq_t;
//! type for 16bit huffman data
typedef unsigned short  bits_t;
//! type for 5 bit bit length data
typedef unsigned char   bitlen_t; 
//! queue for pixel data
typedef hls::stream<pixel_t>  pixel_queue_t;
//! queue for yuv data  
typedef hls::stream<yuv_t>    yuv_queue_t;
//! queue for dct data
typedef hls::stream<freq_t>   freq_queue_t;
//! queue for bits data
typedef hls::stream<bits_t>   bits_queue_t;
//! queue for bit length data
typedef hls::stream<bitlen_t> bitlen_queue_t;
//! general 32 bit queue data
typedef hls::stream<unsigned char>    byte_queue_t;
#else 
//for vivado HLS
//#include "ap_int.h"
#include "hls_stream.h"

//! type for 8bit pixel data
typedef unsigned char  pixel_t;
//! type for 8bit yuv data
typedef char   yuv_t;
//! type for 12bit dct output data  
typedef short  freq_t;
//! type for 16bit huffman data
typedef unsigned short bits_t;
//! type for 5 bit bit length data
typedef unsigned char  bitlen_t; 
//! queue for pixel data
typedef hls::stream<pixel_t>  pixel_queue_t;
//! queue for yuv data  
typedef hls::stream<yuv_t>    yuv_queue_t;
//! queue for dct data
typedef hls::stream<freq_t>   freq_queue_t;
//! queue for bits data
typedef hls::stream<bits_t>   bits_queue_t;
//! queue for bit length data
typedef hls::stream<bitlen_t> bitlen_queue_t;
//! general 8 bit queue data
typedef hls::stream<pixel_t>  byte_queue_t;

#endif


void colorBuffer(bool color_mode, int sampling,
		unsigned int width, 
		const unsigned char buf[MAX_ROWBUF_WIDTH*3*8], 
		yuv_t yQ[64], 
		yuv_t uQ[64],
		yuv_t vQ[64], bool &yen, bool& uen, bool& ven);
void dct(bool dcten, yuv_t inqueue[64], freq_t outqueue[64], bool& qen);
void quant_y(bool qen, unsigned char* qtable, freq_t inqueue[64], freq_t outqueue[64], bool& hen);
void quant_u(bool qen, unsigned char* qtable, freq_t inqueue[64], freq_t outqueue[64], bool& hen);
void quant_v(bool qen, unsigned char* qtable, freq_t inqueue[64], freq_t outqueue[64], bool& hen);

void huffman_Y(bool hen,
             freq_t inqueue[64], bits_t bitqueue[80], bitlen_t bitlen_queue[80], unsigned char& len);
void huffman_UV(bool hen,
             freq_t inqueue[64], bits_t bitqueue[80], bitlen_t bitlen_queue[80], yuv_t datatype, unsigned char& len);

void bitWriter(
	bits_t ybitqueue[80], bitlen_t ylen_queue[80], unsigned char& len,
		bool flush, byte_queue_t& memq);
void writeHeader(
                 bool color_mode,
								 int  sampling,
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
                 byte_queue_t& outqueue);

void blockEncoder(
		bool init,
		bool color_mode, int sampling,
		unsigned short imagewidth,
		unsigned short imageheight,
    unsigned char lumaQtable[64],
    unsigned char chromaQtable0[64],
    unsigned char chromaQtable1[64],
		const unsigned char buffer[MAX_ROWBUF_WIDTH*3*8],
		pixel_queue_t& queue,
		bool flush
		);
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
const unsigned char buffer[MAX_ROWBUF_WIDTH*3*16], pixel_queue_t& queue,bool flush_all);
#define SAMPLE_444 0		
#define SAMPLE_422 1
#define SAMPLE_420 3		
#endif            		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                  		
                      
                  		
