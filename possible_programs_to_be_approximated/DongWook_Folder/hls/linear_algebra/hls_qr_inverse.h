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
 *     (c) Copyright 2014 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

#ifndef HLS_QR_INVERSE_H
#define HLS_QR_INVERSE_H

#include "hls/linear_algebra/utils/x_hls_complex.h"

namespace hls {
  // ===================================================================================================================
  // QR_INVERSE: Entry point function.
  // o Template parameters:
  //  - RowsColsA        : Defines the matrix dimensions
  //  - InputType        : Input data type
  //  - OutputType       : Output data type
  // o Arguments:
  //  - A                : Input matrix A
  //  - InverseA         : Inverse of input matrix
  // o Return values:
  //  - 0                : Success
  //  - 1                : Failure, matrix A is singular
  template <
    int RowsColsA,
    typename InputType,
    typename OutputType>
  int qr_inverse(const InputType A[RowsColsA][RowsColsA],
                 OutputType InverseA[RowsColsA][RowsColsA]) {

    // Bring the lower level functions up to the same level of hierarchy to permit optimizations
    #pragma HLS inline recursive

    // Define intermediate buffers
    OutputType Q[RowsColsA][RowsColsA];
    OutputType R[RowsColsA][RowsColsA];
    OutputType InverseR[RowsColsA][RowsColsA];
    int R_singular = 0;

    // Run QR factorization, get upper-triangular result in R, orthogonal/unitary matrix Q
    const bool TRANSPOSED_Q = true; // Q is produced in transpose form such that Q*A = R
    qrf<TRANSPOSED_Q, RowsColsA, RowsColsA, InputType, OutputType>(A, Q, R);

    // Initialize InverseR to ensure off-diagonal coefficients are zero - this can be done while QRF is running
    // The right-hand side value of I is assumed, and implicit in the back-substitution implementation.
    qr_inverse_init_zero_rows : for(int r=0; r<RowsColsA; r++){
      qr_inverse_init_zero_cols : for(int c=0; c<RowsColsA; c++) {
        if (r != c) {
          // Only elements not on the diagonal need to be initialized
          InverseR[r][c] = 0;
        }
      }
    }

    // Run back-substitution to compute R^-1
    // This doesn't work in-place, so use an additional array InverseR
    back_substitute<RowsColsA, OutputType, OutputType>(R, InverseR);

    // A^-1 = R^-1*Qt
    matrix_multiply<NoTranspose, NoTranspose, RowsColsA, RowsColsA, RowsColsA, RowsColsA, RowsColsA, RowsColsA, OutputType, OutputType>(InverseR, Q, InverseA);

    // Check whether A was singular by checking diagonal elements of R
    qr_inverse_check_singular : for (int r=0; r<RowsColsA; r++) {
      if (x_real(R[r][r]) == 0 && x_imag(R[r][r]) == 0) {
        R_singular = 1;
      }
    }

    return R_singular;

  } // end template qr_inverse

} // end namespace hls

#endif
