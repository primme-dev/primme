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
 * File: projection.c
 *
 * Purpose - This builds a random subspace embedding
 *
 ******************************************************************************/

#ifndef THIS_FILE
#define THIS_FILE "../eigs/sketch.c"
#endif

#include "numerical.h"
#include "template_normal.h"
#include "common_eigs.h"
/* Keep automatically generated headers under this section  */
#ifndef CHECK_TEMPLATE
#include "sketch.h"
#include "convergence.h"
#include "correction.h"
#include "init.h"
#include "ortho.h"
#include "restart.h"
#include "solve_projection.h"
#include "update_projection.h"
#include "update_W.h"
#include "auxiliary_eigs.h"
#include "auxiliary_eigs_normal.h"
#endif

#ifdef SUPPORTED_TYPE


/* Insert into array x of length a random integers between 1 and n */
STATIC void rand_iprimme(PRIMME_INT *x, PRIMME_INT a, PRIMME_INT n) {
   PRIMME_INT i, j;
   int flag;
   i = 0;
   while(i < a)
   {
      flag = 0;
      x[i] = rand() % n;
      for(j = 0; j < i; j++) 
         if(x[j] == x[i]) 
         {
            flag = 1;
            break;
         }
      if(flag == 0) i++;
   }
}

/******************************************************************************
 * Subroutine build_sketch - This routine builds the subspace embedding matrix
 * in CSC format that can be applied to a basis for sketching. This is adapted
 * from the Sparse Maps method. Because each column contains the same number 
 * of nonzeros, we do not need to store the index pointers. 
 ******************************************************************************/
TEMPLATE_PLEASE
int build_sketch_Sprimme(PRIMME_INT *S_rows, SCALAR *S_vals, primme_context ctx) {

  primme_params *primme = ctx.primme;

  PRIMME_INT i, j;         /* loop variables */
  PRIMME_INT my_start = 0; /* Use to determine the local columns assigned to this process */
  int *global_start;       /* nLocals of all processes */
  PRIMME_INT seed;       /* seed for the random number generator */
  PRIMME_INT nnzPerCol = primme->sketchingParams.nnzPerCol;
  PRIMME_INT ldS = primme->sketchingParams.sketchSize;
   
  SCALAR scaling_factor = 1/sqrt(ldS);


  /* Determine which columns of S belong to this process */
  CHKERR(Num_malloc_iprimme(primme->numProcs, &global_start, ctx));
  for(i = 0; i < primme->numProcs; i++) global_start[i] = 0;
  global_start[primme->procID] = primme->nLocal;
  CHKERR(globalSum_Tprimme(global_start, primme_op_int, primme->numProcs, ctx));  
  for(i = 0; i < primme->procID; i++) my_start += global_start[i];
  CHKERR(Num_free_iprimme(global_start, ctx));

  /* Build the CSR matrix locally */
  for(i = 0; i < primme->nLocal; i++)
  {
     /* Set rng seed */
     seed = my_start+i+1;
     srand((unsigned int)seed);

     /* determine which rows will be nonzero in this column */
     rand_iprimme(&S_rows[i*nnzPerCol], nnzPerCol, ldS);

     /* Insert entries corresponding to the Hadamard distribution (complex) or -1/+1 (real) */
#ifdef USE_COMPLEX
  CHKERR(Num_larnv_Sprimme(3, &seed, nnzPerCol, &S_vals[i*nnzPerCol], ctx));
  for(j = i*nnzPerCol; j < (i+1)*nnzPerCol; j++) S_vals[j] /= cabs(S_vals[j]);
#else
  for(j = i*nnzPerCol; j < (i+1)*nnzPerCol; j++) S_vals[j] = (rand() % 2)*2 - 1;
#endif
  }
  
  CHKERR(Num_scal_Sprimme(primme->nLocal*nnzPerCol, scaling_factor, S_vals, 1, ctx));

  return 0;
}

/******************************************************************************
 * Subroutine sketch_basis - This routine multiplies the basis by the random
 * subspace embedding and returns the result in SV
 ******************************************************************************/
TEMPLATE_PLEASE
int sketch_basis_Sprimme(SCALAR *V, PRIMME_INT ldV, SCALAR *SV, PRIMME_INT ldSV, PRIMME_INT basisSize, PRIMME_INT blockSize, PRIMME_INT *S_rows, SCALAR *S_vals, primme_context ctx) {

   primme_params *primme = ctx.primme;
 
   SCALAR *V_row;    /* Used to temporarily store a row in V to avoid numberous memory accesses */
   PRIMME_INT i, j;
   PRIMME_INT nnzPerCol = primme->sketchingParams.nnzPerCol;

   CHKERR(Num_malloc_Sprimme(blockSize, &V_row, ctx));
   CHKERR(Num_zero_matrix_Sprimme(&SV[ldSV*basisSize], ldSV, blockSize, ldSV, ctx));

   /* Sparse MM */
   for(i = 0; i < primme->nLocal; i++) /* Traverse the rows of the basis V */
   {
      CHKERR(Num_copy_Sprimme(blockSize, &V[basisSize*ldV+i], ldV, V_row, 1, ctx));
      for(j = 0; j < nnzPerCol; j++) CHKERR(Num_axpy_Sprimme(blockSize, S_vals[i*nnzPerCol+j], V_row, 1, &SV[basisSize*ldSV + S_rows[i*nnzPerCol+j]], ldSV, ctx));
   }
   CHKERR(Num_free_Sprimme(V_row, ctx)); 

   /* Find the sketched basis */
   CHKERR(globalSum_Sprimme(&SV[basisSize*ldSV], blockSize*ldSV, ctx));  
 
   return 0;
}

/******************************************************************************
 * Subroutine sketched_residuals - This routine finds the residuals of the 
 * sketched Ritz pairs along with the residual norms. 
 * \| SW*x - SV*x*lambda \|_2 / \|SV*x\|_2
 * SAV = SV*H
 ******************************************************************************/
TEMPLATE_PLEASE
int sketched_residuals_Sprimme(SCALAR *SV, PRIMME_INT ldSV, SCALAR *SW, PRIMME_INT ldSW, HSCALAR *hVecs, PRIMME_INT ldhVecs, HEVAL *evals, PRIMME_INT basisSize, SCALAR *residuals, PRIMME_INT ldresiduals, HREAL *resNorms, primme_context ctx) {

   primme_params *primme = ctx.primme;

   SCALAR *SVhVecs, *SWhVecs;                               /* Temporary arrays used to compute residuals */
   PRIMME_INT numEvals = min(primme->numEvals, basisSize);
   PRIMME_INT i;  /* Loop variable */

   CHKERR(Num_malloc_Sprimme(ldSV*numEvals, &SVhVecs, ctx));   
   CHKERR(Num_malloc_Sprimme(ldSW*numEvals, &SWhVecs, ctx));   

   CHKERR(Num_gemm_Sprimme("N", "N", ldSV, numEvals, basisSize, 1.0, SV, ldSV, hVecs, ldhVecs, 0.0, SVhVecs, ldSV, ctx));   // SVhVecs
   CHKERR(Num_gemm_Sprimme("N", "N", ldSW, numEvals, basisSize, 1.0, SW, ldSW, hVecs, ldhVecs, 0.0, SWhVecs, ldSW, ctx));   // SWhVecs

   /* Compute residual vectors */
   CHKERR(Num_compute_residuals_Sprimme(ldSV, numEvals, evals, SVhVecs, ldSV, SWhVecs, ldSW, residuals, ldresiduals, ctx));
   
   /* Compute residual norms */
   for(i = 0; i < numEvals; i++) resNorms[i] = sqrt(Num_dot_Sprimme(ldSV, &residuals[i*ldresiduals], 1, &residuals[i*ldresiduals], 1, ctx) / Num_dot_Sprimme(ldSV, &SVhVecs[i*ldSV], 1, &SVhVecs[i*ldSV], 1, ctx));
   CHKERR(globalSum_Rprimme(resNorms, numEvals, ctx));

   Num_free_Sprimme(SVhVecs, ctx);
   Num_free_Sprimme(SWhVecs, ctx);
  
return 0;
}

/******************************************************************************
 * Subroutine sketched_RR_Sprimme - This routine uses a random subspace embedding
 * to estimate the eigenpairs of the H matrix produced by Lanczos
 * I.e. [evecs, evals] = eig((SV)'(SAV)y = Ly)
 *
 * INPUT arrays and parameters
 * ----------------------------------
 * ldV      The leading dimension of V
 * 
 * ldSV     The leading dimension of SV
 *
 * ldhVecs  The leading dimension of hVecs 
 * 
 * basisSize The size of the basis
 *
 * blockSize The number of columns added to the basis per iteration in Lanczos
 * 
 * nnzPerCol Number of nonzeros per column of the sketching matrix
 * 
 * S_rows   The nonzero rows indices for all columns in the sketching matrix
 *          in CSC format
 * 
 * S_vals   The values for all nonzero spots in the sketching matrix in CSC format 
 * 
 * INPUT/OUTPUT arrays and parameters
 * ----------------------------------
 * SV       The sketched basis with dimensions ldS x ldV
 * 
 * hVecs    The eigenvectors of the sketched RR problem
 *
 * hVals    The sketched Ritz values
 * 
 * resNorms The estimated residual norms from the sketched RR
 * 
 ******************************************************************************/

TEMPLATE_PLEASE
int sketched_RR_Sprimme(SCALAR *SV, PRIMME_INT ldSV, SCALAR *SW, PRIMME_INT ldSW, HSCALAR *hVecs, PRIMME_INT ldhVecs, HREAL *hVals, PRIMME_INT basisSize, primme_context ctx) {
#ifdef USE_HERMITIAN
   primme_params *primme = ctx.primme;

   PRIMME_INT i; /* Loop variable */
   
   if(primme->procID == 0)
   {

      SCALAR *UtSW;     /* Temporary matrix to compute left hand side of the generalized eigenvalue problem */
      SCALAR *UtSWV;    /* Left hand side of the generalized eigenvalue problem */
      SCALAR *UVecs;    /* Left singular vectors of the SVD */
      SCALAR *VVecst;   /* Right singular vectors of the SVD (transposed) */
      SCALAR *ShVals;   /* The "alpha" eigenvalues returned from LaPack's GGEV */ 
      SCALAR *hVals_b;  /* The "beta" eigenvalues returned from LaPack's GGEV */ 
      SCALAR *trunc_hVecs;  /* The stabilized eigenvectors of H */ 
      SCALAR *Sigma;    /* Right hand side of the generalized eigenvalue problem */
      SCALAR *temp_SV;  /* Temporarily store SV while performing LaPack's GESVD */
      REAL *sing_vals;  /* Returned singular values from the SVD */
      int *eval_perm;   /* To sort the eigenpairs */
      PRIMME_INT ldVVecst, ldUVecs, ldUtSW, ldUtSWV, ldSigma, ldtrunc_hVecs;   /* Leading dimension of matrices */
      PRIMME_INT trunc_basisSize; /* The basis size after stabilization */

      ldUVecs = ldSV;
      ldVVecst = basisSize;

      CHKERR(Num_malloc_Sprimme(ldUVecs*basisSize, &UVecs, ctx));
      CHKERR(Num_malloc_Sprimme(ldVVecst*basisSize, &VVecst, ctx));
      CHKERR(Num_malloc_Sprimme(ldSV*basisSize, &temp_SV, ctx));
      CHKERR(Num_malloc_Rprimme(basisSize, &sing_vals, ctx));

      /* Take the SVD decomposition of SV */
      CHKERR(Num_copy_matrix_Sprimme(&SV[0], ldSV, basisSize, ldSV, &temp_SV[0], ldSV, ctx));
      CHKERR(Num_gesvd_Sprimme("S", "S", ldSV, basisSize, SV, ldSV, sing_vals, UVecs, ldUVecs, VVecst, ldVVecst, ctx));
      CHKERR(Num_copy_matrix_Sprimme(&temp_SV[0], ldSV, basisSize, ldSV, &SV[0], ldSV, ctx));
      CHKERR(Num_free_Sprimme(temp_SV, ctx)); 

      /* Truncate the basis for stabilization if needed */
      trunc_basisSize = basisSize;
      for(i = basisSize-1; i > primme->numEvals; i--)
      {
         if(sing_vals[0]/sing_vals[i] >= 1/MACHINE_EPSILON)
         {
            trunc_basisSize--;
         } else {
            break;
         }
      }

      /* Build each side of the generalized eigenvalue problem after stabilization */
      ldSigma = ldUtSW = ldUtSWV = ldtrunc_hVecs = trunc_basisSize;
      CHKERR(Num_malloc_Sprimme(ldUtSW*basisSize, &UtSW, ctx));
      CHKERR(Num_malloc_Sprimme(ldUtSWV*trunc_basisSize, &UtSWV, ctx));
      CHKERR(Num_malloc_Sprimme(ldtrunc_hVecs*trunc_basisSize, &trunc_hVecs, ctx));
      CHKERR(Num_malloc_Sprimme(trunc_basisSize, &ShVals, ctx));
      CHKERR(Num_malloc_Sprimme(trunc_basisSize, &hVals_b, ctx));
      CHKERR(Num_malloc_Sprimme(ldSigma*trunc_basisSize, &Sigma, ctx));
      CHKERR(Num_malloc_iprimme(trunc_basisSize, &eval_perm, ctx));

      /* Construct Mass Matrix (Diagonal matrix with singular values as entries) */
      CHKERR(Num_zero_matrix_Sprimme(Sigma, trunc_basisSize, trunc_basisSize, ldSigma, ctx));
      for(i = 0; i < trunc_basisSize; i++) Sigma[i*ldSigma+i] = sing_vals[i];

      /* Left hand side of the Eigenvalue problem (U'(SW)Vt'), where Vt is the transpose of the right singular vectors */
      CHKERR(Num_gemm_Sprimme("C", "N", trunc_basisSize, basisSize, ldSV, 1.0, UVecs, ldUVecs, SW, ldSW, 0.0, UtSW, ldUtSW, ctx)); 
      CHKERR(Num_gemm_Sprimme("N", "C", trunc_basisSize, trunc_basisSize, basisSize, 1.0, UtSW, ldUtSW, VVecst, ldVVecst, 0.0, UtSWV, ldUtSWV, ctx)); 
      
      /* Eigenvalue problem */
      CHKERR(Num_ggev_Sprimme("N", "V", trunc_basisSize, UtSWV, ldUtSWV, Sigma, ldSigma, ShVals, NULL, hVals_b, NULL, trunc_basisSize, trunc_hVecs, ldtrunc_hVecs, ctx)); 
      CHKERR(Num_gemm_Sprimme("C", "N", basisSize, trunc_basisSize, trunc_basisSize, 1.0, VVecst, ldVVecst, trunc_hVecs, ldtrunc_hVecs, 0.0, hVecs, ldhVecs, ctx)); 

      primme->stats.estimateMaxEVal = -INFINITY;
      primme->stats.estimateLargestSVal = -INFINITY;
      for(i = 0; i < trunc_basisSize; i++)
      {
         hVals[i] = REAL_PART(ShVals[i]/hVals_b[i]);
         eval_perm[i] = i;
         primme->stats.estimateMaxEVal = max(ctx.primme->stats.estimateMaxEVal, EVAL_REAL_PART(hVals[i])); 
         primme->stats.estimateLargestSVal = max(ctx.primme->stats.estimateLargestSVal, EVAL_ABS(hVals[i])); 
      }
      primme->aNorm = primme->stats.estimateLargestSVal;

      /* Sort the eigenpairs */
      for(i = 0; i < trunc_basisSize; i++) CHKERR(insertionSort_Rprimme(hVals[i], hVals, 0.0, NULL, 0, NULL, eval_perm, i, 0, ctx.primme));

      CHKERR(permute_vecs_Sprimme(hVecs, basisSize, trunc_basisSize, ldhVecs, eval_perm, ctx));

      CHKERR(Num_free_Sprimme(UVecs, ctx)); 
      CHKERR(Num_free_Sprimme(VVecst, ctx)); 
      CHKERR(Num_free_Sprimme(UtSW, ctx)); 
      CHKERR(Num_free_Sprimme(UtSWV, ctx)); 
      CHKERR(Num_free_Sprimme(trunc_hVecs, ctx)); 
      CHKERR(Num_free_Sprimme(ShVals, ctx)); 
      CHKERR(Num_free_Sprimme(hVals_b, ctx)); 
      CHKERR(Num_free_Sprimme(Sigma, ctx)); 
      CHKERR(Num_free_iprimme(eval_perm, ctx)); 
      CHKERR(Num_free_Rprimme(sing_vals, ctx)); 
   }

   CHKERR(broadcast_SHprimme(hVecs, ldhVecs*primme->numEvals, ctx));
   CHKERR(broadcast_RHprimme(hVals, primme->numEvals, ctx));

return 0;
#else
   (void)SV;
   (void)ldSV;
   (void)SW;
   (void)ldSW;
   (void)hVecs;
   (void)ldhVecs;
   (void)hVals;
   (void)basisSize;
   (void)ctx;

   CHKERR(PRIMME_FUNCTION_UNAVAILABLE);
   return 0;
#endif
}

#endif   // SUPPORT_TYPE
