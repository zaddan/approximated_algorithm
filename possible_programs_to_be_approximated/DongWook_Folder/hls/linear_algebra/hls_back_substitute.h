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

#ifndef HLS_BACK_SUBSTITUTE_H
#define HLS_BACK_SUBSTITUTE_H

#include "ap_fixed.h"
#include "hls/linear_algebra/utils/x_hls_complex.h"

namespace hls {

  // ===================================================================================================================
  // Default traits struct defining the internal variable types for the Back Substitution function
  template<
    int RowsColsA,
    typename InputType,
    typename OutputType>
  struct back_substitute_traits {
    typedef InputType RECIP_T;
    typedef InputType MULT_T;
    typedef InputType ADD_T;
    typedef InputType MULTNEG_T;
  };

  // Specialization for ap_fixed
  template<
    int RowsColsA,
    int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1,
    int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
  struct back_substitute_traits
    <RowsColsA, ap_fixed<W1, I1, Q1, O1, N1>, ap_fixed<W2, I2, Q2, O2, N2> > {
    static const int W = W1+(W1-I1)+(W1-I1); // Fractional growth is denominator fraction width + numerator full width
    static const int I = I1+(W1-I1); // Integer growth is denominator int width + numerator fraction width
    typedef  ap_fixed<W, I, AP_TRN, AP_WRAP, 0>  RECIP_T;
    typedef  ap_fixed<2*W, 2*I, AP_TRN, AP_WRAP, 0>  MULT_T;
    typedef  ap_fixed<(2*W)+1, (2*I)+1, AP_TRN, AP_WRAP, 0>  ADD_T;
    typedef  ap_fixed<2*W2, 2*I2, AP_TRN, AP_WRAP, 0>  MULTNEG_T;
  };

  // Further specialization for x_complex<ap_fixed>
  template<
    int RowsColsA,
    int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1,
    int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
  struct back_substitute_traits
    <RowsColsA, x_complex<ap_fixed<W1, I1, Q1, O1, N1> >, x_complex< ap_fixed<W2, I2, Q2, O2, N2> > > {
    static const int W = W1+(W1-I1)+(W1-I1); // Fractional growth is denominator fraction width + numerator full width
    static const int I = I1+(W1-I1); // Integer growth is denominator int width + numerator fraction width
    typedef x_complex< ap_fixed<W, I, AP_TRN, AP_WRAP, 0> > RECIP_T;
    typedef x_complex< ap_fixed<(2*W)+1, (2*I)+1, AP_TRN, AP_WRAP, 0> > MULT_T;
    typedef x_complex< ap_fixed<(2*W)+2, (2*I)+2, AP_TRN, AP_WRAP, 0> > ADD_T;
    typedef x_complex< ap_fixed<(2*W2)+1,(2*I2)+1, AP_TRN, AP_WRAP, 0> > MULTNEG_T;
  };

  // ===================================================================================================================
  // Helper functions

  // IMPLEMENTATION TIP: Force the reciprocal to be implemented using a single-precision divider with the resource directive.
  // This permits sharing of operators with the factorization function preceding the back-substitution
  // HLS will use a reciprocal operator by default if a directive is not used.
  template <typename T> void back_substitute_recip(T x, T &one_over_x) {
    #pragma HLS resource variable=one_over_x core=FDiv
    const T ONE = 1.0;
    one_over_x = ONE/x;
  }

  // All diagonal elements of the factorization (Cholesky or QR) should be real.
  // We can therefore use a real-valued divider (or reciprocal operator) to compute the diagonal inverse values.
  //
  // IMPLEMENTATION TIP: Force the reciprocal to be implemented using a single-precision divider with the resource directive.
  // This permits sharing of operators with the factorization function preceding the back-substitution.
  // HLS will use a reciprocal operator by default if a directive is not used.
  template <typename T> void back_substitute_recip(x_complex<T> x, x_complex<T> &one_over_x) {
    #pragma HLS resource variable=recip core=FDiv
    T recip; // intermediate variable to allow directive to be applied
    const x_complex<T> ONE = 1.0;
    recip = ONE.real()/x.real();
    one_over_x.real() = recip;
    one_over_x.imag() = 0.0;
  }

  template<
    int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1,
    int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
    void back_substitute_recip(ap_fixed<W1, I1, Q1, O1, N1> x,
                               ap_fixed<W2, I2, Q2, O2, N2> &one_over_x) {
    ap_fixed<W2, I2, Q2, O2, N2> ONE = 1; // Size to the output precision
    one_over_x = ONE/x; // Infers a divider
  }

  template<
    int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1,
    int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
    void back_substitute_recip(x_complex< ap_fixed<W1, I1, Q1, O1, N1> > x,
                               x_complex< ap_fixed<W2, I2, Q2, O2, N2> > &one_over_x) {
    x_complex< ap_fixed<W2, I2, Q2, O2, N2> > ONE; // Size to the output precision
    ONE.real() = 1;
    ONE.imag() = 0;
    one_over_x.real() = ONE.real()/x.real(); // Infers a real-valued divider
    one_over_x.imag() = 0;
  }

  // ===================================================================================================================
  // BACK_SUBSTITUTE_TOP: Top level function taking a BackSubstituteTraits template parameter which defines internal types
  // o Call this function directly if you wish to override the default internal types
  // o It is assumed that argument B is an internal buffer created in the calling function
  //   and not an external interface, since the back substitution process below reads and writes
  //   from/to that buffer.
  template<
    int RowsColsA,
    typename BackSubstituteTraits,
    typename InputType,
    typename OutputType>
  void back_substitute_top(const InputType A[RowsColsA][RowsColsA],
                                OutputType B[RowsColsA][RowsColsA]) {

    //

    typename BackSubstituteTraits::RECIP_T diag_recip;
    OutputType subst_prod_m1;
    OutputType subst_prod_m2;
    typename BackSubstituteTraits::MULT_T subst_prod;
    typename BackSubstituteTraits::MULT_T subst_addend;
    typename BackSubstituteTraits::ADD_T subst_sum;
    typename BackSubstituteTraits::ADD_T diag_recip_low;
    typename BackSubstituteTraits::MULTNEG_T neg_diag_prod;

    // Because we know that many entries in A are zero, we can use fewer operations
    // (at the cost of inner-loop dependencies on the outer loop)
    back_substitute_j : for (int j=0; j<RowsColsA; j++) {
      back_substitute_recip(A[j][j],diag_recip);
      B[j][j] = diag_recip;
      back_substitute_i : for (int i=0; i<j; i++) {
        back_substitute_k : for (int k=0; k<j; k++){
          subst_prod_m1 = A[k][j]; // For fixed-point, re-size to higher precision of B
          subst_prod_m2 = B[i][k];
          subst_prod = subst_prod_m1 * subst_prod_m2;
          subst_addend = B[i][j]; // For fixed-point, re-size to multiplier output precision
          subst_sum = subst_addend + subst_prod;
          B[i][j] = subst_sum;
        }
        diag_recip_low = diag_recip; // For fixed-point, reduce precision to match subst_sum for multiplication
        // IMPLEMENTATION TIP: Use last subst_sum value below rather than reading from B to reduce error for fixed-point implementations.
        // Implementing as "neg_diag_prod = -B[i][j] * B[j][j];" for fixed-point increases error by ~10%, but halves
        // the DSP48 usage.
        neg_diag_prod = -subst_sum * diag_recip_low;
        B[i][j] = neg_diag_prod;
      }
    }
  } // end template back_substitute_top

  // ===================================================================================================================
  // BACK_SUBSTITUTE: Entry point function
  //  - RowsColsA  : Row and column dimensions
  //  - InputType  : Input data type
  //  - OutputType : Output data type
  template<
    int RowsColsA,
    typename InputType,
    typename OutputType>
  void back_substitute(const InputType A[RowsColsA][RowsColsA],
                            OutputType B[RowsColsA][RowsColsA]) {
    typedef back_substitute_traits<RowsColsA, InputType, OutputType> DEFAULT_BACK_SUBSTITUTE_TRAITS;
    back_substitute_top<RowsColsA, DEFAULT_BACK_SUBSTITUTE_TRAITS, InputType, OutputType>(A, B);
  }

} // end namespace hls

#endif
