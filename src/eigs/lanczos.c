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
 * File: main_iter.c
 *
 * Purpose - This is the main Lanczos-type iteration 
 *
 ******************************************************************************/

#ifndef THIS_FILE
#define THIS_FILE "../eigs/lanczos.c"
#endif


#include "numerical.h"
#include "template_normal.h"
#include "common_eigs.h"
/* Keep automatically generated headers under this section  */
#ifndef CHECK_TEMPLATE
#include "lanczos.h"
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

/******************************************************************************
 * Subroutine lanczos - This routine implements a more general, parallel, 
 *    block Lanczos outer iteration
 *
 * A coarse outline of the algorithm performed is as follows:
 *
 *  1. Initialize basis V_1
 *     Initial iteration step:
 *  2. W_1' = A * V_1
 *  3. a_1 = W_1' * V_1
 *  4. W_1 = W_1' - a_1 * V_1
 *  2. for j = 2, 3, ...., basisSize do
 *  3.    b_j = ||W_{j-1}||
 *  4.    V_j = W_{j-1} / b_j
 *  5.    W_j' = A * V_j
 *  6.    a_1 = W_j' * V_j
 *  7.    W_j = W_j' - a_j * V_j - b_j * V_{j-1}
 *  8. endfor
 *  9. Let T = [a_1   b_2                              0  ]
 *             [b_2   a_2   b_3                           ]
 *             [      b_3   a_3   b_4                     ]
 *             [            ...   ...   ...               ]
 *             [                  b_{m-1}   a_{m-1}   b_m ]
 *             [  0                         b_m       a_m}]
 *
 * 
 * INPUT/OUTPUT arrays and parameters
 * ----------------------------------
 * evecs    Stores initial guesses. Upon return, it contains the converged Ritz
 *          vectors.  If locking is not engaged, then converged Ritz vectors 
 *          are copied to this array just before return.  
 *
 * primme.initSize: On output, it stores the number of converged eigenvectors. 
 *           If smaller than numEvals and locking is used, there are
 *              only primme.initSize vectors in evecs.
 *           Without locking all numEvals approximations are in evecs
 *              but only the initSize ones are converged.
 *           During the execution, access to primme.initSize gives 
 *              the number of converged pairs up to that point. The pairs
 *              are available in evals and evecs, but only when locking is used
 * ret      successful state
 * numRet   number of returned pairs in evals, evecs, and resNorms
 *
 * Return Value
 * ------------
 * error code      
 ******************************************************************************/

TEMPLATE_PLEASE
int lanczos_Sprimme(HEVAL *evals, SCALAR *evecs, PRIMME_INT ldevecs,
      HREAL *resNorms, double startTime, int *ret, int *numRet,
      primme_context ctx) {

   /* Default error is something is wrong in this function */
   *ret = PRIMME_MAIN_ITER_FAILURE;

   primme_params *primme = ctx.primme;

                            /* primme parameters */
   SCALAR *V;                 /* Basis vectors                                 */
   SCALAR *W;                 /* Work space storing A*V                        */
   SCALAR *Va;                /* Temporary work array of size nLocalxblockSize */
   SCALAR *a;                 /* Inner product of W'V                          */
   SCALAR *B;                 /* R factor of W                                 */
   SCALAR *Q;                 /* QR decompositions for harmonic or refined     */

   HSCALAR *T;                /* Holds the tridiagonal matrix from Lanczos     */
   HSCALAR *H;                /* Upper triangular portion of V'*A*V            */
   HSCALAR *hVecs;            /* Eigenvectors of H                             */
   HSCALAR *prevhVecs;        /* hVecs from previous iteration                 */
   HSCALAR *R;                /* projection: (A-target[i])*V = QR              */
   HSCALAR *QtV;              /* Q'*V                                          */
   HSCALAR *hVecsRot;         /* transformation of hVecs in arbitrary vectors  */
   HSCALAR *hU;               /* Left singular vectors of R                    */

   PRIMME_INT ldV;            /* The leading dimension of V                    */
   PRIMME_INT ldW;            /* The leading dimension of W                    */
   PRIMME_INT ldVa;           /* The leading dimension of Va                   */
   PRIMME_INT lda;            /* The leading dimension of a                    */
   PRIMME_INT ldB;            /* The leading dimension of B                    */
   PRIMME_INT ldQ;            /* The leading dimension of Q                    */
   PRIMME_INT ldT;            /* The leading dimension of T                    */
   PRIMME_INT ldH;            /* The leading dimension of H                    */
   PRIMME_INT ldQtV;          /* The leading dimension of QtV                  */
   PRIMME_INT ldhVecs;        /* The leading dimension of hVecs                */

   int i;                     /* Loop variable                                 */
   int basisSize;             /* Current size of the basis V                   */
   int blockSize;             /* Current block size                            */
   int numQR;                 /* Maximum number of QR factorizations           */
   int maxEvecsSize;          /* Maximum capacity of evecs array               */
   int numConverged;          /* Number of converged Ritz pairs                */
   int nprevhVecs;            /* Number of vectors stored in prevhVecs         */

   double smallestResNorm;    /* the smallest residual norm in the block       */
   HEVAL *hVals;              /* Eigenvalues of H                              */
   HEVAL *prevRitzVals;       /* Eigenvalues of H at previous outer iteration  */
                              /* by robust shifting algorithm in correction.c  */

   HREAL *hSVals;             /* Singular values of R                          */
   HREAL *basisNorms;         /* Residual norms of basis at pairs              */
   HREAL *blockNorms;         /* Residual norms corresponding to current block */
                              /* vectors.                                      */

   PRIMME_INT nLocal = primme->nLocal;
   PRIMME_INT maxBasisSize = primme->maxBasisSize;
   int maxBlockSize = min(primme->maxBlockSize, maxBasisSize);    /* In case user enters in too big a block size */
   blockSize = maxBlockSize;

   if (primme->projectionParams.projection != primme_proj_RR) 
      numQR = 1;
   else 
      numQR = 0;

   /* -------------------------------------------------------------- */
   /* Allocate objects                                               */
   /* -------------------------------------------------------------- */

   /* Setting up leading dimensions for matrices */
   ldV = ldW = ldVa = ldQ = ldT = primme->ldOPs;
   ldH = ldhVecs = ldQtV = maxBasisSize;
   lda = ldB = blockSize;

   CHKERR(Num_malloc_Sprimme(ldV*maxBasisSize, &V, ctx));
   CHKERR(Num_malloc_Sprimme(ldW*blockSize, &W, ctx));
   CHKERR(Num_malloc_Sprimme(ldVa*blockSize, &Va, ctx));
   CHKERR(Num_malloc_Sprimme(maxBasisSize*blockSize, &a, ctx));
   CHKERR(Num_malloc_Sprimme((maxBasisSize-blockSize)*blockSize, &B, ctx));

   CHKERR(Num_malloc_SHprimme(maxBasisSize*maxBasisSize, &T, ctx));
   CHKERR(Num_malloc_SHprimme(primme->maxBasisSize * primme->maxBasisSize, &H, ctx));
   CHKERR(Num_malloc_SHprimme(primme->maxBasisSize * primme->maxBasisSize, &hVecs, ctx));
   CHKERR(Num_malloc_SHprimme(primme->maxBasisSize * primme->maxBasisSize, &prevhVecs, ctx));
   CHKERR(Num_malloc_SHprimme(primme->maxBasisSize * primme->maxBasisSize, &hVecsRot, ctx));

   if (numQR > 0) {
      CHKERR(Num_malloc_Sprimme(primme->ldOPs*primme->maxBasisSize*numQR, &Q, ctx));
      CHKERR(Num_malloc_Sprimme(primme->ldOPs*primme->maxBasisSize*numQR, &QtV, ctx));
      CHKERR(Num_malloc_SHprimme(primme->maxBasisSize * primme->maxBasisSize * numQR, &R, ctx));
      CHKERR(Num_malloc_SHprimme(primme->maxBasisSize * primme->maxBasisSize * numQR, &hU, ctx));
      CHKERR(Num_malloc_RHprimme(primme->maxBasisSize, &hSVals, ctx));
   }

   CHKERR(KIND(Num_malloc_RHprimme, Num_malloc_SHprimme)(primme->maxBasisSize, &hVals, ctx)); //XXX: What does 'KIND' do?
   CHKERR(KIND(Num_malloc_RHprimme, Num_malloc_SHprimme)(primme->maxBasisSize + primme->numEvals, &prevRitzVals, ctx));
   CHKERR(Num_malloc_RHprimme(primme->maxBlockSize, &blockNorms, ctx));
   CHKERR(Num_malloc_RHprimme(primme->maxBasisSize, &basisNorms, ctx));
   CHKERR(Num_zero_matrix_RHprimme(basisNorms, 1, primme->maxBasisSize, 1, ctx));

 
   /* -------------------------------------------------------------- */
   /* Lanczos algorithm                                              */
   /* -------------------------------------------------------------- */

   /* Setup Lanczis ------------------------------------------------ */
   for( i=0; i < blockSize; i++)
      CHKERR(Num_larnv_Sprimme(2, primme->iseed, nLocal, &V[i], ctx));
   CHKERR(ortho_Sprimme(V, ldV, NULL, 0, 0, blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));

   /* Adjust block size if needed */
   blockSize = min(maxBlockSize, maxBasisSize-blockSize);

   /* Main loop of Lanczos ----------------------------------------- */
   for(i = maxBlockSize; i < maxBasisSize; i += blockSize) {
      
      blockSize = min(blockSize, maxBasisSize-i);                                                                                                           /* Adjust block size             */
      CHKERR(matrixMatvec_Sprimme(&V[ldV*(i-blockSize)], nLocal, ldV, &W[0], ldW, maxBasisSize, blockSize, ctx));                                           /* W = A*V_{i-1}                 */
      
      if(i > maxBlockSize) {
         CHKERR(Num_gemm_Sprimme("N", "T", nLocal, blockSize, 0, 1.0, &V[ldV*(i-2*blockSize)], ldV, &B[ldB*(i-blockSize)], ldB, 1.0, &Va[0], ldVa, ctx));   /* Va = V_{i-2}*B_{i-1}'         */
         CHKERR(Num_axpy_Sprimme(nLocal, -1.0, &Va[0], 1, &W[0], 1, ctx));                                                                                  /*  W = W - (V_{i-2}*B{i-1})     */
      }

      CHKERR(Num_gemm_Sprimme("T", "N", blockSize, blockSize, 0, 1.0, &W[0], ldW, &V[ldV*(i-blockSize)], ldV, 1.0, &a[lda*(i-blockSize)], lda, ctx));       /* a = W' * V_{i-1}              */
      CHKERR(Num_gemm_Sprimme("N", "N", nLocal, blockSize, 0, 1.0, &V[ldV*(i-blockSize)], ldV, &a[lda*(i-blockSize)], lda, 1.0, &Va[0], ldVa, ctx));        /* Va = V_{i-1}*a_{i}'           */
      CHKERR(Num_axpy_Sprimme(nLocal, -1.0, &Va[0], 1, &W[0], 1, ctx));                                                                                     /* W = W - (V_{i-1}*a_i)         */
      CHKERR(ortho_Sprimme(&W[0], ldW, &B[ldB*(i-blockSize)], ldB, 0, blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));                                /* [W, B_i] = QR(W)              */
      CHKERR(Num_copy_matrix_Sprimme(&W[0], nLocal, blockSize, ldW, &V[i], ldV, ctx));                                                                      /* V_{j+1} = W                   */
   }

   /* ---------------------------------------------------------- */
   /* Basis Built - Moving on to Eigenproblem solve              */
   /* ---------------------------------------------------------- */

   /* Allocate objects ------------------------------------------ */

  /* Initialize counters and flags ---------------------------- */

   primme->stats.numOuterIterations            = 0; 
   primme->stats.numRestarts                   = 0;
   primme->stats.numMatvecs                    = 0;
   primme->stats.numPreconds                   = 0;
   primme->stats.numGlobalSum                  = 0;
   primme->stats.numBroadcast                  = 0;
   primme->stats.volumeGlobalSum               = 0;
   primme->stats.volumeBroadcast               = 0;
   primme->stats.flopsDense                    = 0;
   primme->stats.numOrthoInnerProds            = 0.0;
   primme->stats.elapsedTime                   = 0.0;
   primme->stats.timeMatvec                    = 0.0;
   primme->stats.timePrecond                   = 0.0;
   primme->stats.timeOrtho                     = 0.0;
   primme->stats.timeGlobalSum                 = 0.0;
   primme->stats.timeBroadcast                 = 0.0;
   primme->stats.timeDense                     = 0.0;
   primme->stats.estimateMinEVal               = HUGE_VAL;
   primme->stats.estimateMaxEVal               = -HUGE_VAL;
   primme->stats.estimateLargestSVal           = -HUGE_VAL;
   primme->stats.estimateBNorm                 = primme->massMatrixMatvec ? -HUGE_VAL : 1.0;
   primme->stats.estimateInvBNorm              = primme->massMatrixMatvec ? -HUGE_VAL : 1.0;
   primme->stats.maxConvTol                    = 0.0;
   primme->stats.estimateResidualError         = 0.0;
   primme->stats.lockingIssue                  = 0;

   /* -------------------------------------- */
   /* Quick return for matrix of dimension 1 */
   /* -------------------------------------- */

   if (primme->numEvals == 0) {
      primme->initSize = 0;
      *ret = 0;
      goto clean;
   }

   /* Now initSize will store the number of converged pairs */
   primme->initSize = 0;


   /* ---------------------------------------------------------- */
   /* Deallocate arrays                                          */
   /* ---------------------------------------------------------- */
clean:
   CHKERR(Num_free_Sprimme(V, ctx));
   CHKERR(Num_free_Sprimme(Va, ctx));
   CHKERR(Num_free_Sprimme(W, ctx));
   CHKERR(Num_free_SHprimme(T, ctx));
   CHKERR(Num_free_Sprimme(a, ctx));
   CHKERR(Num_free_Sprimme(B, ctx));

   *ret = 0;      /* Set return value to PASSED */

 }

#endif   // SUPPORT_TYPE
