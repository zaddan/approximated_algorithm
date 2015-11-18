/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2012-2013 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/*
 * HLS Video I/O Function Header File
 */

#ifndef ___HLS__VIDEO_IO__
#define ___HLS__VIDEO_IO__

#include "hls_axi_io.h"

namespace hls {

#define ERROR_IO_EOL_EARLY  (1 << 0)
#define ERROR_IO_EOL_LATE   (1 << 1)

// Unpack a AXI video stream into a hls::Mat<> object

template<int W, int ROWS, int COLS, int T>
int AXIvideo2Mat(stream<ap_axiu<W,1,1,1> >& AXI_video_strm,
                 Mat<ROWS, COLS, T>& img)
{
    int res = 0;
    ap_axiu<W,1,1,1> axi;
    Scalar<HLS_MAT_CN(T), HLS_TNAME(T)> pix;
    int depth = HLS_TBITDEPTH(T);
    // std::cout << W << " " << depth << " " << HLS_MAT_CN(T) << "\n";
    assert(W >= depth*HLS_MAT_CN(T) && "Width of AXI stream must be greater the total number of bits in a pixel");
    HLS_SIZE_T rows = img.rows;
    HLS_SIZE_T cols = img.cols;
    bool sof = 0;
 loop_wait_for_start: while (!sof) {
#pragma HLS pipeline II=1
#pragma HLS loop_tripcount avg=0 max=0
        AXI_video_strm >> axi;
        sof = axi.user.to_int();
    }
 loop_height: for (HLS_SIZE_T i = 0; i < rows; i++) {
#pragma HLS loop_tripcount max=ROWS
        bool eol = 0;
    loop_width: for (HLS_SIZE_T j = 0; j < cols; j++) {
#pragma HLS loop_tripcount max=COLS
#pragma HLS loop_flatten off
#pragma HLS pipeline II=1
            if (sof) {
                sof = 0;
                eol = axi.last.to_int();
            } else {
                if (!eol) {
                    // If we didn't reach EOL, then read the next pixel
                    AXI_video_strm >> axi;
                    eol = axi.last.to_int();
                } else {
                    // otherwise we'll have a blank pixel.
                    ap_axiu<W,1,1,1> axiblack = {};
                    axi = axiblack;
                }
            }
            if (eol && (j != cols-1)) {
                res |= ERROR_IO_EOL_EARLY;
            }
        loop_channels: for (HLS_CHANNEL_T k = 0; k < HLS_MAT_CN(T); k++) {
                AXIGetBitFields(axi, k*depth, depth, pix.val[k]);
            }
            img << pix;
        }
    loop_wait_for_eol: while (!eol) {
#pragma HLS pipeline II=1
#pragma HLS loop_tripcount avg=0 max=0
            // Keep reading until we get to EOL
            AXI_video_strm >> axi;
            eol = axi.last.to_int();
            res |= ERROR_IO_EOL_LATE;
        }
    }
    return res;
}

//Pack the data of a hls::Mat<> object into an AXI Video stream

template<int W, int ROWS, int COLS, int T>
int Mat2AXIvideo(Mat<ROWS, COLS, T>& img,
                 stream<ap_axiu<W,1,1,1> >& AXI_video_strm)
{
    int res = 0;
    Scalar<HLS_MAT_CN(T), HLS_TNAME(T)> pix;
    ap_axiu<W,1,1,1> axi;
    int depth = HLS_TBITDEPTH(T);
    // std::cout << W << " " << depth << " " << HLS_MAT_CN(T) << "\n";
    assert(W >= depth*HLS_MAT_CN(T) && "Width of AXI stream must be greater the total number of bits in a pixel");
    HLS_SIZE_T rows = img.rows;
    HLS_SIZE_T cols = img.cols;
    bool sof = 1;
 loop_height: for (HLS_SIZE_T i = 0; i < rows; i++) {
#pragma HLS loop_tripcount max=ROWS
    loop_width: for (HLS_SIZE_T j = 0; j < cols; j++) {
#pragma HLS loop_tripcount max=COLS
#pragma HLS loop_flatten off
#pragma HLS pipeline II=1
            if (sof) {
                axi.user = 1;
                sof = 0;
            } else {
                axi.user = 0;
            }
            if (j == (cols-1)) {
                axi.last = 1;
            } else {
                axi.last = 0;
            }
            img >> pix;
            axi.data = -1;
        loop_channels: for (HLS_CHANNEL_T k = 0; k < HLS_MAT_CN(T); k++) {
                AXISetBitFields(axi, k*depth, depth, pix.val[k]);
            }
            axi.keep = -1;
            AXI_video_strm << axi;
        }
    }
    return res;
}

} // namespace hls

#endif
