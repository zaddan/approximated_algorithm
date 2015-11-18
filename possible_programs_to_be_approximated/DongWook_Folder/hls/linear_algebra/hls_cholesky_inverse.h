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

#ifndef HLS_CHOLESKY_INVERSE_H
#define HLS_CHOLESKY_INVERSE_H

#include "ap_fixed.h"
#include "hls/linear_algebra/utils/x_hls_complex.h"

namespace hls {

  // ===================================================================================================================
  // Default traits struct defining the internal variable types for the Cholesky Inverse function
  template<
    int RowsColsA,
    typename InputType,
    typename OutputType>
  struct cholesky_inverse_traits {
    typedef InputType                                             CHOLESKY_OUT;
    typedef cholesky_traits< RowsColsA,InputType,InputType>       CHOLESKY_TRAITS;
    typedef InputType                                             BACK_SUBSTITUTE_OUT;
    typedef back_substitute_traits<RowsColsA,InputType,InputType> BACK_SUBSTITUTE_TRAITS;
    typedef matrix_multiply_traits<NoTranspose,ConjugateTranspose,RowsColsA,RowsColsA,RowsColsA,RowsColsA,InputType,OutputType> MATRIX_MULTIPLY_TRAITS;
  };

  // Specialization for ap_fixed
  template<
    int RowsColsA,
    int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1,
    int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
  struct cholesky_inverse_traits
    <RowsColsA, ap_fixed<W1, I1, Q1, O1, N1>, ap_fixed<W2, I2, Q2, O2, N2> > {
    // Cholesky decomposition output precision
    static const int       CholeskyOutputW = W1;
    static const int       CholeskyOutputI = I1;
    static const ap_q_mode CholeskyOutputQ = Q1;
    static const ap_o_mode CholeskyOutputO = O1;
    static const int       CholeskyOutputN = N1;
    typedef ap_fixed<CholeskyOutputW, CholeskyOutputI, CholeskyOutputQ, CholeskyOutputO, CholeskyOutputN> CHOLESKY_OUT;
    typedef cholesky_traits< RowsColsA, ap_fixed<W1, I1, Q1, O1, N1>, CHOLESKY_OUT>                       CHOLESKY_TRAITS;
    // Back substitution output precision
    static const int       BackSubstitutionOutW = W2;
    static const int       BackSubstitutionOutI = I2;
    static const ap_q_mode BackSubstitutionOutQ = Q2;
    static const ap_o_mode BackSubstitutionOutO = O2;
    static const int       BackSubstitutionOutN = N2;
    typedef ap_fixed<BackSubstitutionOutW, BackSubstitutionOutI, BackSubstitutionOutQ, BackSubstitutionOutO, BackSubstitutionOutN> BACK_SUBSTITUTE_OUT;
    typedef back_substitute_traits<RowsColsA, CHOLESKY_OUT, BACK_SUBSTITUTE_OUT >                                                  BACK_SUBSTITUTE_TRAITS;
    typedef matrix_multiply_traits<NoTranspose,ConjugateTranspose,RowsColsA,RowsColsA,RowsColsA,RowsColsA,BACK_SUBSTITUTE_OUT, ap_fixed<W2, I2, Q2, O2, N2> > MATRIX_MULTIPLY_TRAITS;
  };

  // Further specialization for hls::complex<ap_fixed>
  template<
    int RowsColsA,
    int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1,
    int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
  struct cholesky_inverse_traits
    <RowsColsA, x_complex<ap_fixed<W1, I1, Q1, O1, N1> >, x_complex< ap_fixed<W2, I2, Q2, O2, N2> > > {
    // Cholesky decomposition output precision
    static const int       CholeskyOutputW = W1;
    static const int       CholeskyOutputI = I1;
    static const ap_q_mode CholeskyOutputQ = Q1;
    static const ap_o_mode CholeskyOutputO = O1;
    static const int       CholeskyOutputN = N1;
    typedef x_complex< ap_fixed<CholeskyOutputW, CholeskyOutputI, CholeskyOutputQ, CholeskyOutputO, CholeskyOutputN> > CHOLESKY_OUT;
    typedef cholesky_traits< RowsColsA, x_complex< ap_fixed<W1, I1, Q1, O1, N1> >, CHOLESKY_OUT >                      CHOLESKY_TRAITS;
    // Back substitution output precision
    static const int       BackSubstitutionOutW = W2;
    static const int       BackSubstitutionOutI = I2;
    static const ap_q_mode BackSubstitutionOutQ = Q2;
    static const ap_o_mode BackSubstitutionOutO = O2;
    static const int       BackSubstitutionOutN = N2;
    typedef x_complex< ap_fixed<BackSubstitutionOutW, BackSubstitutionOutI, BackSubstitutionOutQ, BackSubstitutionOutO, BackSubstitutionOutN> > BACK_SUBSTITUTE_OUT;
    typedef back_substitute_traits<RowsColsA, CHOLESKY_OUT, BACK_SUBSTITUTE_OUT >                                                               BACK_SUBSTITUTE_TRAITS;
    typedef matrix_multiply_traits<NoTranspose,ConjugateTranspose,RowsColsA,RowsColsA,RowsColsA,RowsColsA,BACK_SUBSTITUTE_OUT, x_complex< ap_fixed<W2, I2, Q2, O2, N2> > > MATRIX_MULTIPLY_TRAITS;
  };
  
  // ===================================================================================================================
  // CHOLESKY_INVERSE_TOP: Top level function taking a CholeskyInverseTraits template parameter which defines internal types
  template <
    int RowsColsA, 
    typename CholeskyInverseTraits, 
    typename InputType, 
    typename OutputType> 
  int cholesky_inverse_top(const InputType A[RowsColsA][RowsColsA], 
                           OutputType InverseA[RowsColsA][RowsColsA]) {
    
    // Bring the lower level functions up to the same level of hierarchy to permit optimizations
    #pragma HLS inline recursive
    
    // Define intermediate buffers
    typename CholeskyInverseTraits::CHOLESKY_OUT U[RowsColsA][RowsColsA];
    typename CholeskyInverseTraits::BACK_SUBSTITUTE_OUT U_inv[RowsColsA][RowsColsA];
    int cholesky_success;

    // Run Cholesky, get upper-triangular result
    const bool LOWER_TRIANGULAR = false;
    cholesky_success = cholesky<LOWER_TRIANGULAR, RowsColsA, InputType, typename CholeskyInverseTraits::CHOLESKY_OUT>(A, U);

    // Initialize U_inv to ensure off-diagonal coefficients are zero - this can be done while the Cholesky is running.
    // The right-hand side value of I is assumed, and implicit in the back-substitution implementation.
    cholesky_inverse_init_zero_rows : for(int r=0; r<RowsColsA; r++){
      cholesky_inverse_init_zero_cols : for(int c=0; c<RowsColsA; c++) {
        if (r != c) {
          // Only elements not on the diagonal need to be initialized
          U_inv[r][c] = 0;
        }
      }
    }

    // Run back-substitution to compute U^-1
    // This doesn't work in-place, so use an additional array U_inv
    back_substitute<RowsColsA, typename CholeskyInverseTraits::CHOLESKY_OUT, typename CholeskyInverseTraits::BACK_SUBSTITUTE_OUT>(U, U_inv);

    // A^-1 = U^-1*U^-t (equivalent to L-t*L-1)
    matrix_multiply<NoTranspose, ConjugateTranspose, RowsColsA, RowsColsA, RowsColsA, RowsColsA, RowsColsA, RowsColsA, typename CholeskyInverseTraits::BACK_SUBSTITUTE_OUT, OutputType >(U_inv, U_inv, InverseA);

    return cholesky_success;
  }
  // ===================================================================================================================
  // CHOLESKY_INVERSE: Entry point function.
  // o Template parameters:
  //  - RowsColsA        : Defines the matrix dimensions
  //  - InputType        : Input data type
  //  - OutputType       : Output data type
  // o Arguments:
  //  - A                : Square Hermitian/symmetric positive definite input matrix
  //  - InverseA         : Inverse of input matrix
  // o Return values:
  //  -	0                : Success
  //  -	1                : Failure. The function attempted to find the square root of a negative number i.e. the input 
  //                       matrix A was not Hermitian/symmetric positive definite.
  template <
    int RowsColsA, 
    typename InputType, 
    typename OutputType> 
  int cholesky_inverse(const InputType A[RowsColsA][RowsColsA], 
                       OutputType InverseA[RowsColsA][RowsColsA]) {
    typedef cholesky_inverse_traits<RowsColsA, InputType, OutputType> DEFAULT_CHOLESKY_INVERSE_TRAITS;
    return cholesky_inverse_top<RowsColsA, DEFAULT_CHOLESKY_INVERSE_TRAITS, InputType, OutputType>(A, InverseA);
  }

} // end namespace hls

#endif