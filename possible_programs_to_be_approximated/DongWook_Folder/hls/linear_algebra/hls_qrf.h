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
 *     (c) Copyright 2013-2014 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

#ifndef HLS_QRF_H
#define HLS_QRF_H

#include "hls/linear_algebra/utils/x_hls_complex.h"
#ifndef __SYNTHESIS__
#include <stdio.h>
#endif

namespace hls {

  // ===================================================================================================================
  // Helper functions

  template <typename T> int qrf_require_extra_pass(const int rows, const int cols, T x) {
    qrf_require_extra_pass_real:;
    return 0;
  }

  template <typename T> int qrf_require_extra_pass(const int rows, const int cols, x_complex<T> x) {
    qrf_require_extra_pass_complex:;
    if (rows == cols) {
      // Unitary transformation only required for square matrices
      return 1;
    } else {
      return 0;
    }
  }

  // Vector multiply
  template <typename T> T qrf_vm(T a, T b, T c, T d) {
    return a*c + b*d;
  }

  // Matrix-vector multiply
  template <typename T> void qrf_mm(const T G[2][2], T &a, T &b) {
    T x, y;
    x = qrf_vm(G[0][0], G[0][1], a, b);
    y = qrf_vm(G[1][0], G[1][1], a, b);
    a = x;
    b = y;
  }

  // Magnitude computation for real Givens rotation, guarding against under-/overflow
  // - Returns real-valued magnitude
  template <typename T> T qrf_magnitude(T a, T b)  {
    Function_qrf_magnitude_real:;
    const T ONE = 1.0;
    const T ZERO = 0.0;
    T abs_a, abs_b;
    T y, mag;

    abs_a = abs(a); // declared in x_hls_utils.h
    abs_b = abs(b);
    if (abs_a > abs_b) {
      y = b/a;
      mag = a*x_sqrt(ONE+y*y);

    } else if (abs_b == ZERO) {
      mag = ZERO;
    } else {
      y = a/b;
      mag = b*x_sqrt(ONE+y*y);
    }
    return mag;
  }

  // Magnitude computation for complex Givens rotation, avoiding squaring values which have just been square-rooted
  // and guarding against under-/overflow
  // - Returns real-valued magnitude
  template <typename T> T qrf_magnitude(x_complex<T> a, x_complex<T> b) {
    Function_qrf_magnitude_complex:;
    const T ONE = 1.0;
    const T ZERO = 0.0;
    T ar, ai, br, bi, d1, d2, d3, d3a, d3b, x1, x2, x3;
    T a_largest, b_largest, largest;

    ar = abs(a.real());
    ai = abs(a.imag());
    br = abs(b.real());
    bi = abs(b.imag());

    // Comparator network determines largest of the four components
    if (ar > ai) {
      a_largest = ar;
      d1 = ai;
      d3a = ar;
    } else {
      a_largest = ai;
      d1 = ar;
      d3a = ai;
    }

    if (br > bi) {
      b_largest = br;
      d2 = bi;
      d3b = br;
    } else {
      b_largest = bi;
      d2 = br;
      d3b = bi;
    }

    if (a_largest > b_largest) {
      largest = a_largest;
      d3 = d3b;
    } else {
      largest = b_largest;
      d3 = d3a;
    }

    // Avoid divide-by-zero and scale three components by
    // the largest one
    if (largest == ZERO) {
      x1 = ZERO;
      x2 = ZERO;
      x3 = ZERO;
    } else {
      x1 = d1/largest;
      x2 = d2/largest;
      x3 = d3/largest;
    }

    // Square root of sum of the squares
    return largest*x_sqrt(ONE+x1*x1+x2*x2+x3*x3);

  }

  // ===================================================================================================================
  // Real Givens rotation guarding against under-/overflow situations.
  //
  // Returns matrix G =  | c  s  |
  //                     | ss cc |
  //
  // Note: argument 'extra_pass' is not used for the real case, but is present only to permit function overloading
  //
  template <typename T> void qrf_givens(int extra_pass, T a, T b, T &c, T &s, T &ss, T &cc, T &r){
    Function_qrf_givens_real:;
    const T ONE = 1.0;
    const T ZERO = 0.0;
    T mag;

    mag = qrf_magnitude(a, b);

    if (abs(a) == ZERO && abs(b) == ZERO) { // more efficient than  "if (mag == ZERO)"
      c = x_copysign(ONE,a);
      s = ZERO;
    } else {
      c = a / mag;
      s = b / mag;
    }
    cc = c;
    ss = -s;

    r = mag;
  }

  // ===================================================================================================================
  // Complex Givens rotation
  //
  // This implements a modified Givens rotation of the form:
  //
  // G = | c*  s* |
  //     | -s  c  |
  //
  // to produce real diagonal elements suitable for subsquent computation of the inverse of input matrix A.
  //
  // Returns matrix G =  | c  s  |
  //                     | ss cc |
  //
  // This implementation does not use the same approach as the version of qrf_givens() for real data, as that
  // would require that a divider for complex data be implemented, which is expensive.
  //
  // When argument 'extra_pass' is set to 1, the function computes a unitary transformation rather than a standard Givens matrix.
  // This is required to ensure that the bottom-rightmost element of the R matrix is real.  This transformation matrix has the form:
  //
  // G(trans) = | 1     0   |
  //            | 0  e^-j*T |
  // where T = Theta for the bottom-rightmost element
  //
  template <typename T> void qrf_givens(int extra_pass, x_complex<T> a, x_complex<T> b, x_complex<T> &c, x_complex<T> &s, x_complex<T> &ss, x_complex<T> &cc, x_complex<T> &r){
    Function_qrf_givens_complex:;
    const T ONE = 1.0;
    const T ZERO = 0.0;
    const x_complex<T> CZERO = ZERO;
    T sqrt_mag_a_mag_b;
    x_complex<T> c_tmp, s_tmp;

    if (extra_pass == 0) {
      // Standard modified Givens matrix, guarding against over-/underflow

      sqrt_mag_a_mag_b = qrf_magnitude(a, b);

      if (abs(a.real()) == ZERO && abs(a.imag()) == ZERO && abs(b.real()) == ZERO && abs(b.imag()) == ZERO) { // more efficient than "if (sqrt_mag_a_mag_b == ZERO)"
        c_tmp = x_copysign(ONE,a.real());
        s_tmp = ZERO;
      } else {
        c_tmp = a / sqrt_mag_a_mag_b;
        s_tmp = b / sqrt_mag_a_mag_b;
      }
      c = x_conj(c_tmp);
      cc = c_tmp;
      s = x_conj(s_tmp);
      ss = -s_tmp;

      r.real() = sqrt_mag_a_mag_b;

    } else {
      // Transformation matrix to ensure real diagonal in R, guarding against over-/underflow

      sqrt_mag_a_mag_b = qrf_magnitude(CZERO, b);

      c_tmp = ONE;

      if (abs(b.real()) == ZERO && abs(b.imag()) == ZERO) { // more efficient than "if (sqrt_mag_a_mag_b == ZERO)"
        s_tmp = ONE;
      } else {
        s_tmp = b / sqrt_mag_a_mag_b;
      }

      c = c_tmp;
      cc = x_conj(s_tmp);
      s = ZERO;
      ss = ZERO;

      r.real() = sqrt_mag_a_mag_b;

    }

  }

  // ===================================================================================================================
  // QRF: Entry point function.
  // o Template parameters:
  //  - TransposedQ      : Selects whether Q is output in transposed form
  //  - RowsA            : Number of rows in input matrix A
  //  - ColsA            : Number of columns in input matrix A
  //  - InputType        : Input data type
  //  - OutputType       : Output data type
  // o Arguments:
  //  - A                : Input matrix
  //  - Q                : Orthogonal output matrix
  //  - R                : Upper triangular output matrix
  template <
    bool TransposedQ,
    int RowsA,
    int ColsA,
    typename InputType,
    typename OutputType>
    void qrf(const InputType A[RowsA][ColsA],
             OutputType Q[RowsA][RowsA],
             OutputType R[RowsA][ColsA]) {

      // Verify that template parameters are correct in simulation
      if (RowsA < ColsA) {
#ifndef __SYNTHESIS__
        printf("ERROR: hls_qrf.h: Template parameter error - RowsA must be greater than ColsA; currently RowsA = %d ColsA = %d\n",RowsA,ColsA);
#endif
        exit(1);
      }

      // Buffers
      OutputType Qi[RowsA][RowsA];
      OutputType Ri[RowsA][ColsA];
      InputType G[2][2];

      // Magnitude from Givens computation
      InputType mag;

      // Flags for complex-valued case
      const int DO_UNITARY_TF = qrf_require_extra_pass(RowsA, ColsA, mag);
      int extra_pass = 0;

      // Initialize Qi and initialize/load Ri
      qrf_in_row_assign : for(int r=0; r<RowsA; r++){
        qrf_in_col_assign_Qi : for(int c=0; c<RowsA; c++) {
          if (r == c) {
            Qi[r][c] = 1.0;
          } else {
            Qi[r][c] = 0.0;
          }
        }
        qrf_in_col_assign_Ri : for(int c=0; c<ColsA; c++) {
          Ri[r][c] = A[r][c];
        }
      }

      qrf_col_loop : for(int j=0; j<ColsA; j++) {
        // For complex data and square matrices, we perform an additional pass to ensure that the diagonal of R is real
        // For non-square matrices, the modified Givens rotation ensures that the diagonal will be real-valued
        if (DO_UNITARY_TF == 1) {
          if (j == ColsA-1) {
            extra_pass = 1;
          } else {
            extra_pass = 0;
          }
        } else {
          extra_pass = 0;
        }
        qrf_row_loop : for(int i=RowsA-1; i>j-extra_pass; i--) {
          // Compute Givens values
          qrf_givens(extra_pass, Ri[i-1][j], Ri[i][j], G[0][0], G[0][1], G[1][0], G[1][1], mag);

          if (!extra_pass) {
            Ri[i-1][j] = x_real(mag);
          } else {
            Ri[i][j] = x_real(mag);
          }

          qrf_r_update : for(int k=j+1; k<ColsA; k++) {
            qrf_mm(G, Ri[i-1][k], Ri[i][k]);
          }

          if (j == 0 && i == RowsA-1) {
            // Write Givens matrix values directly into Q for first pair of rows
            Qi[i-1][i-1] = G[0][0];
            Qi[i-1][i]   = G[0][1];
            Qi[i][i-1]   = G[1][0];
            Qi[i][i]     = G[1][1];
          } else {
            // Apply rotations only to element pairs which are non-zero
            qrf_q_update : for(int k=i-(1+j)+extra_pass; k<RowsA; k++) {
              qrf_mm(G, Qi[i-1][k], Qi[i][k]);
            }
          }

        } // end qrf_row_loop
      } // end qrf_col_loop

        // Assign final outputs
      qrf_out_row_assign : for(int r=0; r<RowsA; r++){
        qrf_out_col_assign_Q : for(int c=0; c<RowsA; c++) {
          if (TransposedQ == true) {
            Q[r][c] = Qi[r][c];
          } else {
            Q[c][r] = x_conj(Qi[r][c]);
          }
        }
        qrf_out_col_assign_R : for(int c=0; c<ColsA; c++) {
          if (r <= c) {
            // Values in the lower triangle are undefined
            R[r][c] = Ri[r][c];
          }
        }
      }

    } // end template qrf

} // end namespace hls

#endif
