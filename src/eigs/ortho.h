/*******************************************************************************
 * Copyright (c) 2018, College of William & Mary
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the College of William & Mary nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COLLEGE OF WILLIAM & MARY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * PRIMME: https://github.com/primme/primme
 * Contact: Andreas Stathopoulos, a n d r e a s _at_ c s . w m . e d u
 *******************************************************************************
 *   NOTE: THIS FILE IS AUTOMATICALLY GENERATED. PLEASE DON'T MODIFY
 ******************************************************************************/


#ifndef ortho_H
#define ortho_H
int Bortho_gen_hprimme(dummy_type_hprimme *V, PRIMME_INT ldV, dummy_type_sprimme *R, int ldR,
      int b1, int b2, dummy_type_hprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_sprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_hprimme *, PRIMME_INT, dummy_type_hprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_hprimme(dummy_type_hprimme *V, PRIMME_INT ldV, dummy_type_sprimme *R, int ldR, int b1, int b2,
                  dummy_type_hprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_local_hprimme(dummy_type_sprimme *V, int ldV, dummy_type_sprimme *R,
      int ldR, int b1, int b2, dummy_type_sprimme *locked, int ldLocked,
      int numLocked, PRIMME_INT nLocal, dummy_type_sprimme *B, int ldB, PRIMME_INT *iseed,
      primme_context ctx);
int Bortho_block_hprimme(dummy_type_hprimme *V, PRIMME_INT ldV, dummy_type_sprimme *VLtBVL,
      int ldVLtBVL, dummy_type_sprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_hprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_hprimme *BV, PRIMME_INT ldBV,
      dummy_type_sprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_hprimme(dummy_type_hprimme *V, PRIMME_INT ldV, dummy_type_sprimme *VLtBVL,
      int ldVLtBVL, dummy_type_sprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_hprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_sprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_hprimme(dummy_type_hprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_hprimme *BQ, PRIMME_INT ldBQ, dummy_type_sprimme *QtBQ, int ldQtBQ, dummy_type_hprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_sprimme *norms, primme_context ctx);
int Bortho_gen_kprimme(dummy_type_kprimme *V, PRIMME_INT ldV, dummy_type_cprimme *R, int ldR,
      int b1, int b2, dummy_type_kprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_cprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_kprimme *, PRIMME_INT, dummy_type_kprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_kprimme(dummy_type_kprimme *V, PRIMME_INT ldV, dummy_type_cprimme *R, int ldR, int b1, int b2,
                  dummy_type_kprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_local_kprimme(dummy_type_cprimme *V, int ldV, dummy_type_cprimme *R,
      int ldR, int b1, int b2, dummy_type_cprimme *locked, int ldLocked,
      int numLocked, PRIMME_INT nLocal, dummy_type_cprimme *B, int ldB, PRIMME_INT *iseed,
      primme_context ctx);
int Bortho_block_kprimme(dummy_type_kprimme *V, PRIMME_INT ldV, dummy_type_cprimme *VLtBVL,
      int ldVLtBVL, dummy_type_cprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_kprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_kprimme *BV, PRIMME_INT ldBV,
      dummy_type_cprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_kprimme(dummy_type_kprimme *V, PRIMME_INT ldV, dummy_type_cprimme *VLtBVL,
      int ldVLtBVL, dummy_type_cprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_kprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_cprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_kprimme(dummy_type_kprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_kprimme *BQ, PRIMME_INT ldBQ, dummy_type_cprimme *QtBQ, int ldQtBQ, dummy_type_kprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_sprimme *norms, primme_context ctx);
int Bortho_gen_sprimme(dummy_type_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *R, int ldR,
      int b1, int b2, dummy_type_sprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_sprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_sprimme *, PRIMME_INT, dummy_type_sprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_sprimme(dummy_type_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *R, int ldR, int b1, int b2,
                  dummy_type_sprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_local_sprimme(dummy_type_sprimme *V, int ldV, dummy_type_sprimme *R,
      int ldR, int b1, int b2, dummy_type_sprimme *locked, int ldLocked,
      int numLocked, PRIMME_INT nLocal, dummy_type_sprimme *B, int ldB, PRIMME_INT *iseed,
      primme_context ctx);
int Bortho_block_sprimme(dummy_type_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *VLtBVL,
      int ldVLtBVL, dummy_type_sprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_sprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_sprimme *BV, PRIMME_INT ldBV,
      dummy_type_sprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_sprimme(dummy_type_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *VLtBVL,
      int ldVLtBVL, dummy_type_sprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_sprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_sprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_sprimme(dummy_type_sprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_sprimme *BQ, PRIMME_INT ldBQ, dummy_type_sprimme *QtBQ, int ldQtBQ, dummy_type_sprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_sprimme *norms, primme_context ctx);
int Bortho_gen_cprimme(dummy_type_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *R, int ldR,
      int b1, int b2, dummy_type_cprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_cprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_cprimme *, PRIMME_INT, dummy_type_cprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_cprimme(dummy_type_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *R, int ldR, int b1, int b2,
                  dummy_type_cprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_local_cprimme(dummy_type_cprimme *V, int ldV, dummy_type_cprimme *R,
      int ldR, int b1, int b2, dummy_type_cprimme *locked, int ldLocked,
      int numLocked, PRIMME_INT nLocal, dummy_type_cprimme *B, int ldB, PRIMME_INT *iseed,
      primme_context ctx);
int Bortho_block_cprimme(dummy_type_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *VLtBVL,
      int ldVLtBVL, dummy_type_cprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_cprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_cprimme *BV, PRIMME_INT ldBV,
      dummy_type_cprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_cprimme(dummy_type_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *VLtBVL,
      int ldVLtBVL, dummy_type_cprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_cprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_cprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_cprimme(dummy_type_cprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_cprimme *BQ, PRIMME_INT ldBQ, dummy_type_cprimme *QtBQ, int ldQtBQ, dummy_type_cprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_sprimme *norms, primme_context ctx);
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_gen_Sprimme)
#  define Bortho_gen_Sprimme CONCAT(Bortho_gen_,SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_gen_Rprimme)
#  define Bortho_gen_Rprimme CONCAT(Bortho_gen_,REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_gen_SHprimme)
#  define Bortho_gen_SHprimme CONCAT(Bortho_gen_,HOST_SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_gen_RHprimme)
#  define Bortho_gen_RHprimme CONCAT(Bortho_gen_,HOST_REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_gen_SXprimme)
#  define Bortho_gen_SXprimme CONCAT(Bortho_gen_,XSCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_gen_RXprimme)
#  define Bortho_gen_RXprimme CONCAT(Bortho_gen_,XREAL_SUF)
#endif
int Bortho_gen_dprimme(dummy_type_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *R, int ldR,
      int b1, int b2, dummy_type_dprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_dprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_dprimme *, PRIMME_INT, dummy_type_dprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
#if !defined(CHECK_TEMPLATE) && !defined(ortho_Sprimme)
#  define ortho_Sprimme CONCAT(ortho_,SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_Rprimme)
#  define ortho_Rprimme CONCAT(ortho_,REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_SHprimme)
#  define ortho_SHprimme CONCAT(ortho_,HOST_SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_RHprimme)
#  define ortho_RHprimme CONCAT(ortho_,HOST_REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_SXprimme)
#  define ortho_SXprimme CONCAT(ortho_,XSCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_RXprimme)
#  define ortho_RXprimme CONCAT(ortho_,XREAL_SUF)
#endif
int ortho_dprimme(dummy_type_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *R, int ldR, int b1, int b2,
                  dummy_type_dprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_local_Sprimme)
#  define Bortho_local_Sprimme CONCAT(Bortho_local_,SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_local_Rprimme)
#  define Bortho_local_Rprimme CONCAT(Bortho_local_,REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_local_SHprimme)
#  define Bortho_local_SHprimme CONCAT(Bortho_local_,HOST_SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_local_RHprimme)
#  define Bortho_local_RHprimme CONCAT(Bortho_local_,HOST_REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_local_SXprimme)
#  define Bortho_local_SXprimme CONCAT(Bortho_local_,XSCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_local_RXprimme)
#  define Bortho_local_RXprimme CONCAT(Bortho_local_,XREAL_SUF)
#endif
int Bortho_local_dprimme(dummy_type_dprimme *V, int ldV, dummy_type_dprimme *R,
      int ldR, int b1, int b2, dummy_type_dprimme *locked, int ldLocked,
      int numLocked, PRIMME_INT nLocal, dummy_type_dprimme *B, int ldB, PRIMME_INT *iseed,
      primme_context ctx);
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_block_Sprimme)
#  define Bortho_block_Sprimme CONCAT(Bortho_block_,SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_block_Rprimme)
#  define Bortho_block_Rprimme CONCAT(Bortho_block_,REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_block_SHprimme)
#  define Bortho_block_SHprimme CONCAT(Bortho_block_,HOST_SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_block_RHprimme)
#  define Bortho_block_RHprimme CONCAT(Bortho_block_,HOST_REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_block_SXprimme)
#  define Bortho_block_SXprimme CONCAT(Bortho_block_,XSCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(Bortho_block_RXprimme)
#  define Bortho_block_RXprimme CONCAT(Bortho_block_,XREAL_SUF)
#endif
int Bortho_block_dprimme(dummy_type_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *VLtBVL,
      int ldVLtBVL, dummy_type_dprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_dprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_dprimme *BV, PRIMME_INT ldBV,
      dummy_type_dprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
#if !defined(CHECK_TEMPLATE) && !defined(ortho_block_Sprimme)
#  define ortho_block_Sprimme CONCAT(ortho_block_,SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_block_Rprimme)
#  define ortho_block_Rprimme CONCAT(ortho_block_,REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_block_SHprimme)
#  define ortho_block_SHprimme CONCAT(ortho_block_,HOST_SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_block_RHprimme)
#  define ortho_block_RHprimme CONCAT(ortho_block_,HOST_REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_block_SXprimme)
#  define ortho_block_SXprimme CONCAT(ortho_block_,XSCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_block_RXprimme)
#  define ortho_block_RXprimme CONCAT(ortho_block_,XREAL_SUF)
#endif
int ortho_block_dprimme(dummy_type_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *VLtBVL,
      int ldVLtBVL, dummy_type_dprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_dprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_dprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
#if !defined(CHECK_TEMPLATE) && !defined(ortho_single_iteration_Sprimme)
#  define ortho_single_iteration_Sprimme CONCAT(ortho_single_iteration_,SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_single_iteration_Rprimme)
#  define ortho_single_iteration_Rprimme CONCAT(ortho_single_iteration_,REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_single_iteration_SHprimme)
#  define ortho_single_iteration_SHprimme CONCAT(ortho_single_iteration_,HOST_SCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_single_iteration_RHprimme)
#  define ortho_single_iteration_RHprimme CONCAT(ortho_single_iteration_,HOST_REAL_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_single_iteration_SXprimme)
#  define ortho_single_iteration_SXprimme CONCAT(ortho_single_iteration_,XSCALAR_SUF)
#endif
#if !defined(CHECK_TEMPLATE) && !defined(ortho_single_iteration_RXprimme)
#  define ortho_single_iteration_RXprimme CONCAT(ortho_single_iteration_,XREAL_SUF)
#endif
int ortho_single_iteration_dprimme(dummy_type_dprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_dprimme *BQ, PRIMME_INT ldBQ, dummy_type_dprimme *QtBQ, int ldQtBQ, dummy_type_dprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_dprimme *norms, primme_context ctx);
int Bortho_gen_zprimme(dummy_type_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *R, int ldR,
      int b1, int b2, dummy_type_zprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_zprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_zprimme *, PRIMME_INT, dummy_type_zprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_zprimme(dummy_type_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *R, int ldR, int b1, int b2,
                  dummy_type_zprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_local_zprimme(dummy_type_zprimme *V, int ldV, dummy_type_zprimme *R,
      int ldR, int b1, int b2, dummy_type_zprimme *locked, int ldLocked,
      int numLocked, PRIMME_INT nLocal, dummy_type_zprimme *B, int ldB, PRIMME_INT *iseed,
      primme_context ctx);
int Bortho_block_zprimme(dummy_type_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *VLtBVL,
      int ldVLtBVL, dummy_type_zprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_zprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_zprimme *BV, PRIMME_INT ldBV,
      dummy_type_zprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_zprimme(dummy_type_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *VLtBVL,
      int ldVLtBVL, dummy_type_zprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_zprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_zprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_zprimme(dummy_type_zprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_zprimme *BQ, PRIMME_INT ldBQ, dummy_type_zprimme *QtBQ, int ldQtBQ, dummy_type_zprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_dprimme *norms, primme_context ctx);
int Bortho_gen_magma_sprimme(dummy_type_magma_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *R, int ldR,
      int b1, int b2, dummy_type_magma_sprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_sprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_magma_sprimme *, PRIMME_INT, dummy_type_magma_sprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_magma_sprimme(dummy_type_magma_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *R, int ldR, int b1, int b2,
                  dummy_type_magma_sprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_block_magma_sprimme(dummy_type_magma_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *VLtBVL,
      int ldVLtBVL, dummy_type_sprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_sprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_magma_sprimme *BV, PRIMME_INT ldBV,
      dummy_type_sprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_magma_sprimme(dummy_type_magma_sprimme *V, PRIMME_INT ldV, dummy_type_sprimme *VLtBVL,
      int ldVLtBVL, dummy_type_sprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_sprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_sprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_magma_sprimme(dummy_type_magma_sprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_magma_sprimme *BQ, PRIMME_INT ldBQ, dummy_type_sprimme *QtBQ, int ldQtBQ, dummy_type_magma_sprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_sprimme *norms, primme_context ctx);
int Bortho_gen_magma_cprimme(dummy_type_magma_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *R, int ldR,
      int b1, int b2, dummy_type_magma_cprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_cprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_magma_cprimme *, PRIMME_INT, dummy_type_magma_cprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_magma_cprimme(dummy_type_magma_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *R, int ldR, int b1, int b2,
                  dummy_type_magma_cprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_block_magma_cprimme(dummy_type_magma_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *VLtBVL,
      int ldVLtBVL, dummy_type_cprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_cprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_magma_cprimme *BV, PRIMME_INT ldBV,
      dummy_type_cprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_magma_cprimme(dummy_type_magma_cprimme *V, PRIMME_INT ldV, dummy_type_cprimme *VLtBVL,
      int ldVLtBVL, dummy_type_cprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_cprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_cprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_magma_cprimme(dummy_type_magma_cprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_magma_cprimme *BQ, PRIMME_INT ldBQ, dummy_type_cprimme *QtBQ, int ldQtBQ, dummy_type_magma_cprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_sprimme *norms, primme_context ctx);
int Bortho_gen_magma_dprimme(dummy_type_magma_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *R, int ldR,
      int b1, int b2, dummy_type_magma_dprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_dprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_magma_dprimme *, PRIMME_INT, dummy_type_magma_dprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_magma_dprimme(dummy_type_magma_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *R, int ldR, int b1, int b2,
                  dummy_type_magma_dprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_block_magma_dprimme(dummy_type_magma_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *VLtBVL,
      int ldVLtBVL, dummy_type_dprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_dprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_magma_dprimme *BV, PRIMME_INT ldBV,
      dummy_type_dprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_magma_dprimme(dummy_type_magma_dprimme *V, PRIMME_INT ldV, dummy_type_dprimme *VLtBVL,
      int ldVLtBVL, dummy_type_dprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_dprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_dprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_magma_dprimme(dummy_type_magma_dprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_magma_dprimme *BQ, PRIMME_INT ldBQ, dummy_type_dprimme *QtBQ, int ldQtBQ, dummy_type_magma_dprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_dprimme *norms, primme_context ctx);
int Bortho_gen_magma_zprimme(dummy_type_magma_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *R, int ldR,
      int b1, int b2, dummy_type_magma_zprimme *locked, PRIMME_INT ldLocked, int numLocked,
      dummy_type_zprimme *RLocked, int ldRLocked, PRIMME_INT nLocal,
      int (*B)(dummy_type_magma_zprimme *, PRIMME_INT, dummy_type_magma_zprimme *, PRIMME_INT, int, void *),
      void *Bctx, PRIMME_INT *iseed, int *b2_out, primme_context ctx);
int ortho_magma_zprimme(dummy_type_magma_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *R, int ldR, int b1, int b2,
                  dummy_type_magma_zprimme *locked, PRIMME_INT ldLocked, int numLocked,
                  PRIMME_INT nLocal, PRIMME_INT *iseed, primme_context ctx);
int Bortho_block_magma_zprimme(dummy_type_magma_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *VLtBVL,
      int ldVLtBVL, dummy_type_zprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_zprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_magma_zprimme *BV, PRIMME_INT ldBV,
      dummy_type_zprimme *RLocked, int ldRLocked, PRIMME_INT nLocal, int maxRank,
      int *b2_out, primme_context ctx);
int ortho_block_magma_zprimme(dummy_type_magma_zprimme *V, PRIMME_INT ldV, dummy_type_zprimme *VLtBVL,
      int ldVLtBVL, dummy_type_zprimme *R, PRIMME_INT ldR, int b1, int b2, dummy_type_magma_zprimme *locked,
      PRIMME_INT ldLocked, int numLocked, dummy_type_zprimme *RLocked, int ldRLocked,
      PRIMME_INT nLocal, int maxRank, int *b2_out, primme_context ctx);
int ortho_single_iteration_magma_zprimme(dummy_type_magma_zprimme *Q, int nQ, PRIMME_INT ldQ,
      dummy_type_magma_zprimme *BQ, PRIMME_INT ldBQ, dummy_type_zprimme *QtBQ, int ldQtBQ, dummy_type_magma_zprimme *X,
      int *inX, int nX, PRIMME_INT ldX, dummy_type_dprimme *norms, primme_context ctx);
#endif
