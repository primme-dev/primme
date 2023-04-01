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
 * File: lanczos.c
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
#include "sketch.h"
#endif

#ifdef SUPPORTED_TYPE

/* Insert into array x of length a random integers between 1 and n */
STATIC void rand_rows_iprimme(PRIMME_INT *x, PRIMME_INT a, PRIMME_INT n) {
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

STATIC void compute_residuals_RR(HSCALAR *evecs, PRIMME_INT ldevecs, HEVAL *evals, PRIMME_INT numEvals, HREAL *resNorms, primme_context ctx)
{
   primme_params *primme = ctx.primme;
   
   HSCALAR *evecs_ortho;
   SCALAR *Aevecs_ortho;
   SCALAR *H_ortho;
   SCALAR *new_hVecs;
   SCALAR *new_evecs;
   SCALAR *new_Aevecs;
   SCALAR *resVecs; 
   HREAL *new_hVals;
   PRIMME_INT i;
   
   Num_malloc_SHprimme(primme->nLocal*numEvals, &evecs_ortho, ctx);
   Num_malloc_Sprimme(primme->nLocal*numEvals, &Aevecs_ortho, ctx);
   Num_malloc_Sprimme(numEvals*numEvals, &H_ortho, ctx);
   Num_malloc_Sprimme(numEvals*numEvals, &new_hVecs, ctx);
   Num_malloc_Sprimme(primme->nLocal*numEvals, &new_evecs, ctx);
   Num_malloc_Sprimme(primme->nLocal*numEvals, &new_Aevecs, ctx);
   Num_malloc_Sprimme(primme->nLocal*numEvals, &resVecs, ctx);
   Num_malloc_RHprimme(numEvals, &new_hVals, ctx);

   Num_copy_matrix_Sprimme(evecs, primme->nLocal, numEvals, ldevecs, evecs_ortho, primme->nLocal, ctx);
   ortho_Sprimme(evecs_ortho, primme->nLocal, NULL, 0, 0, numEvals-1, NULL, 0, 0, primme->nLocal, primme->iseed, ctx);   // Ortho the Ritz vectors
   matrixMatvec_Sprimme(evecs_ortho, primme->nLocal, primme->nLocal, Aevecs_ortho, primme->nLocal, 0, numEvals, ctx);    // Projected orthogonal eigenvectors

   // Our "new" banded matrix H 
   Num_gemm_Sprimme("C", "N", numEvals, numEvals, primme->nLocal, 1.0, evecs_ortho, primme->nLocal, Aevecs_ortho, primme->nLocal, 0.0, H_ortho, numEvals, ctx); 
   globalSum_Sprimme(H_ortho, numEvals*numEvals, ctx);  

   solve_H_Sprimme(&H_ortho[0], numEvals, numEvals, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, new_hVecs, numEvals, new_hVals, NULL, 0, ctx);

   // Compute residuals
   Num_gemm_Sprimme("N", "N", primme->nLocal, numEvals, numEvals, 1.0, evecs_ortho, primme->nLocal, new_hVecs, numEvals, 0.0, new_evecs, primme->nLocal, ctx); 
   for(i = 0; i < numEvals; i++) evals[i] = new_hVals[i];
   matrixMatvec_Sprimme(new_evecs, primme->nLocal, primme->nLocal, new_Aevecs, primme->nLocal, 0, numEvals, ctx);    // Projected new eigenvectors
   Num_compute_residuals_Sprimme(primme->nLocal, numEvals, evals, new_evecs, primme->nLocal, new_Aevecs, primme->nLocal, resVecs, primme->nLocal, ctx); 

   for(i = 0; i < numEvals; i++)
      resNorms[i] = Num_dot_Sprimme(primme->nLocal, &resVecs[i*primme->nLocal], 1, &resVecs[i*primme->nLocal], 1, ctx); 
   globalSum_Rprimme(resNorms, numEvals, ctx);  
   for(i = 0; i < numEvals; i++)
      resNorms[i] = sqrt(resNorms[i]); 

   Num_free_SHprimme(evecs_ortho, ctx);
   Num_free_Sprimme(Aevecs_ortho, ctx);
   Num_free_Sprimme(H_ortho, ctx);
   Num_free_Sprimme(new_hVecs, ctx);
   Num_free_Sprimme(new_evecs, ctx);
   Num_free_Sprimme(new_Aevecs, ctx);
   Num_free_Sprimme(resVecs, ctx);
   Num_free_RHprimme(new_hVals, ctx);
  
   return; 
}

/******************************************************************************
 * Subroutine lanczos - This routine implements a more general, parallel, 
 *    block Lanczos outer iteration
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
      HREAL *resNorms, int *ret, int *numRet, int fullOrtho,
      primme_context ctx) {

#ifdef USE_HERMITIAN

   /* Default error is something is wrong in this function */
   *ret = PRIMME_MAIN_ITER_FAILURE;

   primme_params *primme = ctx.primme;

                            /* primme parameters */
   SCALAR *V;                 /* Basis vectors                                 */
   SCALAR *AVhVecs;           /* A*V*hVecs                                     */
   SCALAR *rwork;             /* temporary work vector                         */
   SCALAR *identity;          /* Used to copy beta from the lower triangular to the upper  */

   HSCALAR *H;                /* Upper triangular portion of V'*A*V            */
   HSCALAR *hVecs;            /* Eigenvectors of H                             */
   HREAL *hVals;

   PRIMME_INT ldV;            /* The leading dimension of V                    */
   PRIMME_INT ldAVhVecs;      /* The leading dimension of AVhVecs              */
   PRIMME_INT ldidentity;     /* The leading dimension of identity             */
   PRIMME_INT ldH;            /* The leading dimension of H                    */
   PRIMME_INT ldhVecs;        /* The leading dimension of hVecs                */
   PRIMME_INT ldrwork;        /* The leading dimension of rwork                */

   int *global_start;         /* nLocals of all processes                      */
   PRIMME_INT myGlobalStart = 0;     /* What the the global starting index of this process */
   PRIMME_INT i, j;                  /* Loop variable                                 */
   int blockSize;             /* Current block size                            */
   int numConverged;          /* Number of converged Ritz pairs                */
   int *flags;                /* Indicates which Ritz values have converged    */
   int *eval_perm;            /* For sorting the returned Ritz pairs           */
   int reset = 0;             /* reset variable for check_convergence          */

   double t0 = 0.0;           /* To use for timing                             */

   PRIMME_INT nLocal = primme->nLocal;
   PRIMME_INT maxBasisSize = primme->maxBasisSize;
   int maxBlockSize = blockSize = min(primme->maxBlockSize, maxBasisSize);    /* In case user enters in too big a block size */

   /* Sketching Variables */
   SCALAR *V_temp;            /* Copy of basis vectors for sketching           */
   SCALAR *SV;                /* The sketched basis                            */
   SCALAR *SW;                /* The projected sketched basis                  */
   SCALAR *SVhVecs;           /* The sketched Ritz vectors                     */
   SCALAR *SWhVecs;           /* The projected sketched Ritz vectors           */
   SCALAR *S_vals;                                       /* CSC Formatted Values */
   PRIMME_INT *S_rows;                                          /* CSC Formatted Rows */
   PRIMME_INT sketchSize, nnzPerCol, last_sketch, ldSV;  /* Size and nnz of the sketching matrix */
   REAL *normalize_evecs;

   /* -------------------------------------------------------------- */
   /* Allocate objects                                               */
   /* -------------------------------------------------------------- */

   /* Setting up leading dimensions for matrices */
   ldV = ldAVhVecs = ldrwork = primme->ldOPs;
   ldhVecs = maxBasisSize;
   ldidentity = maxBlockSize;

   if(primme->projectionParams.projection == primme_proj_sketched)
   {
      ldH = maxBlockSize + maxBasisSize;

      CHKERR(Num_malloc_SHprimme((maxBasisSize+blockSize)*maxBasisSize, &H, ctx));
      CHKERR(Num_zero_matrix_SHprimme(H, maxBasisSize+maxBlockSize, maxBasisSize, ldH, ctx));
   }else{
      ldH = maxBasisSize;

      CHKERR(Num_malloc_SHprimme(maxBasisSize*maxBasisSize, &H, ctx));
      CHKERR(Num_zero_matrix_SHprimme(H, maxBasisSize, maxBasisSize, ldH, ctx));
   }

   CHKERR(Num_malloc_Sprimme(ldV*(maxBasisSize+maxBlockSize), &V, ctx));
   CHKERR(Num_malloc_Sprimme(ldAVhVecs*primme->numEvals, &AVhVecs, ctx));
   CHKERR(Num_malloc_Sprimme(ldrwork*maxBasisSize, &rwork, ctx));
   CHKERR(Num_malloc_Sprimme(maxBlockSize*maxBlockSize, &identity, ctx));

   CHKERR(Num_malloc_SHprimme(maxBasisSize*maxBasisSize, &hVecs, ctx));
   
   CHKERR(Num_malloc_RHprimme(maxBasisSize, &hVals, ctx));

   CHKERR(Num_malloc_iprimme(primme->numProcs, &global_start, ctx));
   CHKERR(Num_malloc_iprimme(maxBasisSize, &eval_perm, ctx));
   CHKERR(Num_malloc_iprimme(maxBasisSize, &flags, ctx));

   CHKERR(Num_zero_matrix_SHprimme(identity, maxBlockSize, maxBlockSize, ldidentity, ctx));

   for(i = 0; i < maxBasisSize; i++) flags[i] = UNCONVERGED;
   for(i = 0; i < maxBlockSize; i++) identity[i*ldidentity+i] = 1.0;
  
   /* Find this process's global starting index */
   for(i = 0; i < primme->numProcs; i++) global_start[0] = 0; 
   global_start[primme->procID] = nLocal;
   CHKERR(globalSum_Tprimme(global_start, primme_op_int, primme->numProcs, ctx));  
   for(i = 0; i < primme->procID; i++) myGlobalStart += global_start[i];

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
   primme->stats.timeSketching                 = 0.0;
   primme->stats.timeResiduals                 = 0.0;
   primme->stats.timeKrylov                    = 0.0;
   primme->stats.estimateMinEVal               = HUGE_VAL;
   primme->stats.estimateMaxEVal               = -HUGE_VAL;
   primme->stats.estimateLargestSVal           = -HUGE_VAL;
   primme->stats.estimateBNorm                 = primme->massMatrixMatvec ? -HUGE_VAL : 1.0;
   primme->stats.estimateInvBNorm              = primme->massMatrixMatvec ? -HUGE_VAL : 1.0;
   primme->stats.maxConvTol                    = 0.0;
   primme->stats.estimateResidualError         = 0.0;
   primme->stats.lockingIssue                  = 0;

   /* -------------------------------------------------------------- */
   /* Lanczos algorithm - Build basis (no restarting)                */
   /* -------------------------------------------------------------- */

   /* Quick return for matrix of dimension 1 */
   if (primme->numEvals == 0) goto clean;

   /* Build the sketching matrix first if needed ------------------- */
   if(primme->projectionParams.projection == primme_proj_sketched)
   {
      PRIMME_INT *rand_rows;
      PRIMME_INT iseed;

      sketchSize = ldSV = 4*maxBasisSize;
      nnzPerCol = (int)(ceil(2*log(maxBasisSize+1)));   
      last_sketch = 0;

      //CHKERR(Num_malloc_iprimme(nnzPerCol, &rand_rows, ctx));
      //CHKERR(Num_malloc_iprimme(nnzPerCol*nLocal, &S_rows, ctx));
      CHKERR(Num_malloc_Sprimme(nnzPerCol*nLocal, &S_vals, ctx));
      CHKERR(Num_malloc_Sprimme(ldSV*maxBasisSize, &SW, ctx));
      CHKERR(Num_malloc_Sprimme(ldSV*(maxBasisSize+blockSize), &SV, ctx));
      CHKERR(Num_malloc_Rprimme(primme->numEvals, &normalize_evecs, ctx));
      CHKERR(Num_malloc_Sprimme(ldSV*primme->numEvals, &SVhVecs, ctx));
      CHKERR(Num_malloc_Sprimme(ldSV*primme->numEvals, &SWhVecs, ctx));
      CHKERR(Num_malloc_Sprimme(ldV*(maxBasisSize+blockSize), &V_temp, ctx));

      S_rows = (PRIMME_INT*)malloc(nnzPerCol*nLocal*sizeof(PRIMME_INT));
      rand_rows = (PRIMME_INT*)malloc(nnzPerCol*sizeof(PRIMME_INT));

      /* Start building the CSR Locally */
      for(i = 0; i < nLocal; i++)
      {
         iseed = myGlobalStart+i+1;
         srand((unsigned int)iseed);
         
         /* For each column, randomly select which rows will be nonzero */
         rand_rows_iprimme(&S_rows[i*nnzPerCol], nnzPerCol, sketchSize);

         /* Insert random variables into the nonzeros of the skecthing matrix (Uniform on the complex unit circle or -1 and 1) */
#ifdef USE_COMPLEX
         CHKERR(Num_larnv_Sprimme(3, &iseed, nnzPerCol, &S_vals[i*nnzPerCol], ctx));
         for(j = i*nnzPerCol; j < (i+1)*nnzPerCol; j++) S_vals[j] /= cabs(S_vals[j]);
#else
         for(j = i*nnzPerCol; j < (i+1)*nnzPerCol; j++) S_vals[j] = (rand() % 2)*2 - 1;
#endif
      }

      CHKERR(Num_scal_Sprimme(nLocal*nnzPerCol, 1/sqrt(sketchSize), S_vals, 1, ctx));
      free(rand_rows);

   } /* End sketching matrix build */
   
   /* Let's insert random vectors into the basis ------------------------------------------ */

   blockSize = min(blockSize, maxBasisSize - maxBlockSize); /* Adjust block size first */

   SCALAR *temp_Vrow;
   CHKERR(Num_malloc_Sprimme(blockSize, &temp_Vrow, ctx));
   for(i = 0; i < nLocal; i++)
   {
      srand(myGlobalStart+i+1);
      CHKERR(Num_larnv_Sprimme(3, primme->iseed, blockSize, &temp_Vrow[0], ctx));
      CHKERR(Num_copy_matrix_Sprimme(&temp_Vrow[0], 1, blockSize, 1, &V[i], ldV, ctx));
   }
   CHKERR(Num_free_Sprimme(temp_Vrow, ctx));

   CHKERR(ortho_Sprimme(V, ldV, NULL, 0, 0, blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));

   /* Initial iteration before for loop --------------------------------------------------- */

   CHKERR(matrixMatvec_Sprimme(&V[0], ldV, nLocal, &V[blockSize*ldV], ldV, 0, blockSize, ctx)); /* W = A*V_0 */

   /* Compute and subtract first alpha (W = W - V*(W'V))*/
   CHKERR(update_projection_Sprimme(&V[0], ldV, &V[blockSize*ldV], ldV, &H[0], ldH, nLocal, 0, blockSize, KIND(1 /*symmetric*/, 0 /* unsymmetric */), ctx)); 
   CHKERR(Num_gemm_Sprimme("N", "N", ldrwork, blockSize, blockSize, 1.0, &V[0], ldV, &H[0], ldH, 0.0, &rwork[0], ldrwork, ctx)); 
   CHKERR(Num_axpy_Sprimme(ldV*blockSize, -1.0, &rwork[0], 1, &V[ldV*blockSize], 1, ctx)); 

   t0 = primme_wTimer(); /* Start timing our basis build */

   for(i = maxBlockSize; i < maxBasisSize; i += blockSize) {
      blockSize = min(blockSize, maxBasisSize - i); /* Adjust block size if needed */

      CHKERR(ortho_Sprimme(&V[i*ldV], ldV, &H[(i-blockSize)*ldH + i], ldH, 0, blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));   /* [V_i, b_i] = qr(V_i) */
      if(fullOrtho)
         CHKERR(ortho_Sprimme(&V[0], ldV, NULL, 0, i, i+blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));   /* V_i = cgs(V(:, 0:i-1), V_i) */

      CHKERR(Num_gemm_Sprimme("N", "C", blockSize, blockSize, blockSize, 1.0, &identity[0], ldidentity, &H[(i-blockSize)*ldH + i], ldH, 0.0, &H[i*ldH + (i-blockSize)], ldH, ctx));  

      CHKERR(matrixMatvec_Sprimme(&V[i*ldV], nLocal, ldV, &V[(i+blockSize)*ldV], ldV, 0, blockSize, ctx));                             /* V_{i+1} = AV_i */

      /* Subtract beta term from new chunk of vectors (W_new = W - V_{i-1}*b_i')*/
      CHKERR(Num_gemm_Sprimme("N", "N", ldrwork, blockSize, blockSize, 1.0, &V[(i-blockSize)*ldV], ldV, &H[i*ldH + (i-blockSize)], ldH, 0.0, &rwork[0], ldrwork, ctx));  
      CHKERR(Num_axpy_Sprimme(ldV*blockSize, -1.0, &rwork[0], 1, &V[ldV*(i+blockSize)], 1, ctx)); 

      /* Find and subtract alpha term (W_new = W_new - V_i*a_i)*/
      CHKERR(update_projection_Sprimme(&V[i*ldV], ldV, &V[(i+blockSize)*ldV], ldV, &H[i*ldH+i], ldH, nLocal, 0, blockSize, KIND(1 /*symmetric*/, 0 /* unsymmetric */), ctx)); /* a_i = W'*V_i */
      CHKERR(Num_gemm_Sprimme("N", "N", ldrwork, blockSize, blockSize, 1.0, &V[i*ldV], ldV, &H[i*ldH+i], ldH, 0.0, &rwork[0], ldrwork, ctx)); /* rwork = V_i*a_i */
      CHKERR(Num_axpy_Sprimme(ldV*blockSize, -1.0, &rwork[0], 1, &V[ldV*(i+blockSize)], 1, ctx)); /* W = W - rwork */

      if(primme->printLevel == 4 && (PRIMME_INT)(i+blockSize) % 10 == 0)
      {
         /* Moving on to the eigenvalue problem */
         primme->initSize = 0;

         PRIMME_INT numEvals = min(i+blockSize, primme->numEvals);

         if(primme->projectionParams.projection == primme_proj_sketched)
         {

            CHKERR(Num_copy_matrix_Sprimme(V, ldV, i+2*blockSize, ldV, V_temp, ldV, ctx));
            /* Adding a row to H */
            CHKERR(ortho_Sprimme(&V_temp[ldV*(i+blockSize)], ldV, &H[i*ldH + (i+blockSize)], ldH, 0, blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));   /* [V_i, b_i] = qr(V_i) */

            /* Getting our sketched basis and projected sketched basis */
            CHKERR(apply_sketching_Sprimme(H, ldH, V_temp, ldV, SV, ldSV, hVecs, ldhVecs, hVals, last_sketch, i+blockSize, blockSize, S_rows, S_vals, nnzPerCol, ctx));
            last_sketch = i;            

            /* Eigenvectors that will be returns to the user - make sure they are at least normal (These are the Ritz vectors from the sketched problem) */
            CHKERR(Num_gemm_Sprimme("N", "N", nLocal, numEvals, i+blockSize, 1.0, V_temp, ldV, hVecs, ldhVecs, 0.0, evecs, ldevecs, ctx));    /* evecs = V*hVecs */
            for(j = 0; j < numEvals; j++)
            {
               evals[j] = hVals[j];
               normalize_evecs[j] = Num_dot_Sprimme(nLocal, &evecs[j*ldevecs], 1, &evecs[j*ldevecs], 1, ctx);
            }
            CHKERR(globalSum_Rprimme(normalize_evecs, numEvals, ctx));
            for(j = 0; j < numEvals; j++) CHKERR(Num_scal_Sprimme(nLocal, 1/sqrt(normalize_evecs[j]), &evecs[j*ldevecs], 1, ctx));

            if(primme->residualParams.residual == primme_residual_sketched)
            {
               /* TEST 1 - SKETCHED RESIDUALS */
               /* Get the projected basis */
               CHKERR(Num_gemm_Sprimme("N", "N", ldSV, i+blockSize, i+2*blockSize, 1.0, SV, ldSV, H, ldH, 0.0, SW, ldSV, ctx));           // SW = A*SV = SV*H
               CHKERR(Num_gemm_Sprimme("N", "N", ldSV, numEvals, i+blockSize, 1.0, SV, ldSV, hVecs, ldhVecs, 0.0, SVhVecs, ldSV, ctx));   // SV*hVecs
               CHKERR(Num_gemm_Sprimme("N", "N", ldSV, numEvals, i+blockSize, 1.0, SW, ldSV, hVecs, ldhVecs, 0.0, SWhVecs, ldSV, ctx));   // SW*hVecs

               CHKERR(Num_compute_residuals_Sprimme(sketchSize, numEvals, evals, SVhVecs, ldSV, SWhVecs, ldSV, rwork, ldrwork, ctx));     // rwork = SW*hVecs - evals*SV*hVecs 

               for(j = 0; j < numEvals; j++) resNorms[j] = Num_dot_Sprimme(sketchSize, &rwork[j*ldrwork], 1, &rwork[j*ldrwork], 1, ctx) / Num_dot_Sprimme(sketchSize, &SVhVecs[j*ldSV], 1, &SVhVecs[j*ldSV], 1, ctx);
               CHKERR(globalSum_Rprimme(resNorms, numEvals, ctx));  
               for(j = 0; j < numEvals; j++) resNorms[j] = sqrt(resNorms[j]);

               if(primme->procID == 0){
                  for(j = 0; j < primme->numEvals; j++)
                  {
                     if(j < numEvals)
                     {
                        printf("Iteration %ld SR Eval[%ld] = %lf, ResNorm[%ld] = %.6E\n", i, j, evals[j], j, resNorms[j]);
                     } else {
                        printf("Iteration %ld SR Eval[%ld] = %lf, ResNorm[%ld] = %.6E\n", i, j, 0.0, j, 0.0);
                     }
                  }
               }
            } else if(primme->residualParams.residual == primme_residual_RQ)
            {
               /* TEST 2 - RAYLEIGH QUOTIENT -------------------------------------------------- */
               /* Compute Rayleigh Quotient */
               CHKERR(matrixMatvec_Sprimme(evecs, nLocal, ldevecs, AVhVecs, ldAVhVecs, 0, numEvals, ctx)); /* AVhVecs = A*V*hVecs = A * evecs */
               for(j = 0; j < numEvals; j++)
               evals[j] =  Num_dot_Sprimme(nLocal, &evecs[j*ldevecs], 1, &AVhVecs[j*ldAVhVecs], 1, ctx);      /* eval[i] = evecs'*A*evecs*/
               CHKERR(globalSum_Rprimme(evals, numEvals, ctx));  
            
               /* RQ Residual Vectors */
               CHKERR(Num_compute_residuals_Sprimme(nLocal, numEvals, evals, evecs, ldevecs, AVhVecs, ldAVhVecs, rwork, ldrwork, ctx));
               for(j = 0; j < numEvals; j++) resNorms[j] = Num_dot_Sprimme(nLocal, &rwork[j*ldrwork], 1, &rwork[j*ldrwork], 1, ctx);
               CHKERR(globalSum_Rprimme(resNorms, numEvals, ctx));  

               for(j = 0; j < numEvals; j++) resNorms[j] = sqrt(resNorms[j]);

               if(primme->procID == 0){
                  for(j = 0; j < primme->numEvals; j++)
                  {
                     if(j < numEvals)
                     {
                        printf("Iteration %ld RQ Eval[%ld] = %lf, ResNorm[%ld] = %.6E\n", i, j, evals[j], j, resNorms[j]);
                     } else {
                        printf("Iteration %ld RQ Eval[%ld] = %lf, ResNorm[%ld] = %.6E\n", i, j, 0.0, j, 0.0);
                     }
                  }
               }
            } else if(primme->residualParams.residual == primme_residual_RR)
            {
               /* TEST 3 - RAYLEIGH-RITZ ON THE RITZ VECTORS ----------------------------------- */

               compute_residuals_RR(evecs, ldevecs, evals, numEvals, resNorms, ctx);

               if(primme->procID == 0){
                  for(j = 0; j < primme->numEvals; j++)
                  {
                     if(j < numEvals)
                     {
                        printf("Iteration %ld RR Eval[%ld] = %lf, ResNorm[%ld] = %.6E\n", i, j, evals[j], j, resNorms[j]);
                     } else {
                        printf("Iteration %ld RR Eval[%ld] = %lf, ResNorm[%ld] = %.6E\n", i, j, 0.0, j, 0.0);
                     }
                  }
               }
            }
         } else { /* End sketching */ 
            CHKERR(solve_H_Sprimme(&H[0], i+blockSize, ldH, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, hVecs, ldhVecs, hVals, NULL, 0, ctx));

            /* Acquiring residuals and residual norms */
            for(j = 0; j < min(primme->numEvals, i); j++) evals[j] = hVals[j];
            CHKERR(Num_gemm_Sprimme("N", "N", nLocal, numEvals, i+blockSize, 1.0, V, ldV, hVecs, ldhVecs, 0.0, evecs, ldevecs, ctx));    /* evecs = V*hVecs */
            
            CHKERR(matrixMatvec_Sprimme(evecs, nLocal, ldevecs, AVhVecs, ldAVhVecs, 0, numEvals, ctx)); /* AVhVecs = A*V*hVecs */
            CHKERR(Num_compute_residuals_Sprimme(nLocal, numEvals, evals, evecs, ldevecs, AVhVecs, ldAVhVecs, rwork, ldrwork, ctx));
           for(j = 0; j < numEvals; j++)
              resNorms[j] = Num_dot_Sprimme(ldrwork, &rwork[j*ldrwork], 1, &rwork[j*ldrwork], 1, ctx);
   
           CHKERR(globalSum_Rprimme(resNorms, numEvals, ctx));  
           for(j = 0; j < numEvals; j++) resNorms[j] = sqrt(resNorms[j]);

            if(primme->procID == 0)
               for(j = 0; j < primme->numEvals; j++) printf("Iteration %ld. Eval[%ld] = %lf, ResNorm[%ld] = %.3E\n", i, j, evals[j], j, resNorms[j]);

         } /* End non-sketching */

         /* Check the convergence of the Ritz vectors */
         CHKERR(check_convergence_Sprimme(evecs, ldevecs, 1 /* given X */, NULL, 0, 0 /* not given R */, NULL, 0, 0, NULL, 0, NULL, 0, 0, numEvals, flags, resNorms, hVals, &reset, -1, ctx));

         /* Find number of converged eigenpairs */
         numConverged = 0;
         for(j = 0; j < numEvals && flags[j] == CONVERGED; j++) numConverged = j+1;

         if(numConverged == primme->numEvals)
         {
            primme->initSize = numConverged;
            *numRet = numConverged;
            break;
         } 

      } /* End check residuals */

   } /* End basis build */

   /*XXX: LANCZOS TIMER END */
   primme->stats.timeKrylov = primme_wTimer() - t0;

   /**************************************************************************************
   * BASIS BUILD DONE.
   **************************************************************************************/

   if(numConverged != primme->numEvals)
   {
      /* Moving on to the eigenvalue problem */
      primme->initSize = 0;

      if(primme->projectionParams.projection == primme_proj_sketched)
      {

         t0 = primme_wTimer();
         /* Adding a row to H */
         CHKERR(ortho_Sprimme(&V[ldV*maxBasisSize], ldV, &H[(maxBasisSize-blockSize)*ldH + maxBasisSize], ldH, 0, blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));   /* [V_i, b_i] = qr(V_i) */
         if(fullOrtho) CHKERR(ortho_Sprimme(V, ldV, NULL, 0, maxBasisSize, maxBasisSize+blockSize-1, NULL, 0, 0, nLocal, primme->iseed, ctx));   /* Orthogonalized the last block of V against the rest of the basis */

         CHKERR(apply_sketching_Sprimme(H, ldH, V, ldV, SV, ldSV, hVecs, ldhVecs, hVals, last_sketch, maxBasisSize, blockSize, S_rows, S_vals, nnzPerCol, ctx));

         /* Eigenvectors that will be returns to the user - make sure they are at least normal (These are the Ritz vectors from the sketched problem) */
         CHKERR(Num_gemm_Sprimme("N", "N", nLocal, primme->numEvals, maxBasisSize, 1.0, V, ldV, hVecs, ldhVecs, 0.0, evecs, ldevecs, ctx));    /* evecs = V*hVecs */
         for(j = 0; j < primme->numEvals; j++)
         {
            evals[j] = hVals[j];
            normalize_evecs[j] = Num_dot_Sprimme(nLocal, &evecs[j*ldevecs], 1, &evecs[j*ldevecs], 1, ctx);
         }
         CHKERR(globalSum_Rprimme(normalize_evecs, primme->numEvals, ctx));
         for(j = 0; j < primme->numEvals; j++) CHKERR(Num_scal_Sprimme(nLocal, 1/sqrt(normalize_evecs[j]), &evecs[j*ldevecs], 1, ctx));

         primme->stats.timeSketching = primme_wTimer() - t0;   /* Timing how long sketching took */
         t0 = primme_wTimer();                                 /* To time the residual computation */

         if(primme->residualParams.residual == primme_residual_sketched)   /* Compute sketched residuals */
         {

            /* Get the projected basis */
            CHKERR(Num_gemm_Sprimme("N", "N", ldSV, maxBasisSize, maxBasisSize+blockSize, 1.0, SV, ldSV, H, ldH, 0.0, SW, ldSV, ctx));           // SW = A*SV = SV*H
            CHKERR(Num_gemm_Sprimme("N", "N", ldSV, primme->numEvals, maxBasisSize, 1.0, SV, ldSV, hVecs, ldhVecs, 0.0, SVhVecs, ldSV, ctx));   // SV*hVecs
            CHKERR(Num_gemm_Sprimme("N", "N", ldSV, primme->numEvals, maxBasisSize, 1.0, SW, ldSV, hVecs, ldhVecs, 0.0, SWhVecs, ldSV, ctx));   // SW*hVecs

            CHKERR(Num_compute_residuals_Sprimme(sketchSize, primme->numEvals, evals, SVhVecs, ldSV, SWhVecs, ldSV, rwork, ldrwork, ctx));     // rwork = SW*hVecs - evals*SV*hVecs 

            for(j = 0; j < primme->numEvals; j++) resNorms[j] = Num_dot_Sprimme(sketchSize, &rwork[j*ldrwork], 1, &rwork[j*ldrwork], 1, ctx) / Num_dot_Sprimme(sketchSize, &SVhVecs[j*ldSV], 1, &SVhVecs[j*ldSV], 1, ctx);
            CHKERR(globalSum_Rprimme(resNorms, primme->numEvals, ctx));  
            for(j = 0; j < primme->numEvals; j++) resNorms[j] = sqrt(resNorms[j]);

         } else if(primme->residualParams.residual == primme_residual_RQ)     /* Rayleigh Quotient residuals */
         {
            /* Compute Rayleigh Quotient */
            CHKERR(matrixMatvec_Sprimme(evecs, nLocal, ldevecs, AVhVecs, ldAVhVecs, 0, primme->numEvals, ctx)); /* AVhVecs = A*V*hVecs = A * evecs */
            for(j = 0; j < primme->numEvals; j++)
            evals[j] =  Num_dot_Sprimme(nLocal, &evecs[j*ldevecs], 1, &AVhVecs[j*ldAVhVecs], 1, ctx);      /* eval[i] = evecs'*A*evecs*/
            CHKERR(globalSum_Rprimme(evals, primme->numEvals, ctx));  
         
            /* RQ Residual Vectors */
            CHKERR(Num_compute_residuals_Sprimme(nLocal, primme->numEvals, evals, evecs, ldevecs, AVhVecs, ldAVhVecs, rwork, ldrwork, ctx));
            for(j = 0; j < primme->numEvals; j++) resNorms[j] = Num_dot_Sprimme(nLocal, &rwork[j*ldrwork], 1, &rwork[j*ldrwork], 1, ctx);
            CHKERR(globalSum_Rprimme(resNorms, primme->numEvals, ctx));  

            for(j = 0; j < primme->numEvals; j++) resNorms[j] = sqrt(resNorms[j]);

         } else if(primme->residualParams.residual == primme_residual_RR) compute_residuals_RR(evecs, ldevecs, evals, primme->numEvals, resNorms, ctx); /* Rayleigh Ritz residuals */

      } else { /* End sketched */

         CHKERR(solve_H_Sprimme(&H[0], maxBasisSize, ldH, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, hVecs, ldhVecs, hVals, NULL, 0, ctx));

         t0 = primme_wTimer();                                 /* To time the residual computation */
         for(i = 0; i < primme->numEvals; i++) evals[i] = hVals[i];

         /* Find Ritz Vectors */
         CHKERR(Num_gemm_Sprimme("N", "N", nLocal, primme->numEvals, maxBasisSize, 1.0, V, ldV, hVecs, ldhVecs, 0.0, evecs, ldevecs, ctx));    /* evecs = V*hVecs */
         if(primme->projectionParams.projection == primme_proj_sketched)
         {
            for(j = 0; j < primme->numEvals; j++)
            {
               evals[j] = hVals[j];
               normalize_evecs[j] = Num_dot_Sprimme(nLocal, &evecs[j*ldevecs], 1, &evecs[j*ldevecs], 1, ctx);
            }
            CHKERR(globalSum_Rprimme(normalize_evecs, primme->numEvals, ctx));
            for(j = 0; j < primme->numEvals; j++) CHKERR(Num_scal_Sprimme(nLocal, 1/sqrt(normalize_evecs[j]), &evecs[j*ldevecs], 1, ctx));
         }

         /* Find residual norms */
         CHKERR(matrixMatvec_Sprimme(evecs, nLocal, ldevecs, AVhVecs, ldAVhVecs, 0, primme->numEvals, ctx)); /* AVhVecs = A*V*hVecs */
         CHKERR(Num_compute_residuals_Sprimme(nLocal, primme->numEvals, evals, evecs, ldevecs, AVhVecs, ldAVhVecs, rwork, ldrwork, ctx));
   
         for(j = 0; j < primme->numEvals; j++) resNorms[j] = sqrt(Num_dot_Sprimme(ldrwork, &rwork[j*ldrwork], 1, &rwork[j*ldrwork], 1, ctx))/primme->numProcs;
         CHKERR(globalSum_Rprimme(resNorms, primme->numEvals, ctx));  

      } /* End nonsketched */

      primme->stats.timeResiduals = primme_wTimer() - t0;
      if(primme->procID == 0) for(j = 0; j < primme->numEvals; j++) printf("Eval[%ld] = %lf, ResNorm[%ld] = %.12E\n", j, evals[j], j, resNorms[j]);

      /* Check the convergence of the Ritz vectors */
      CHKERR(check_convergence_Sprimme(evecs, ldevecs, 1 /* given X */, NULL, 0, 0 /* not given R */, NULL, 0, 0, NULL, 0, NULL, 0, 0, primme->numEvals, flags, resNorms, hVals, &reset, -1, ctx));

      /* Find number of converged eigenpairs */
      numConverged = 0;
      for(i = 0; i < primme->numEvals && flags[i] == CONVERGED; i++) numConverged = i+1;

   } /* end if numConverged != primme->numEvals */

   primme->initSize = numConverged;
   *numRet = numConverged;

   /* ---------------------------------------------------------- */
   /* Deallocate arrays                                          */
   /* ---------------------------------------------------------- */
   clean:

   CHKERR(Num_free_Sprimme(V, ctx));
   CHKERR(Num_free_Sprimme(AVhVecs, ctx));
   CHKERR(Num_free_Sprimme(rwork, ctx));
   CHKERR(Num_free_Sprimme(identity, ctx));
   CHKERR(Num_free_Sprimme(hVecs, ctx));

   CHKERR(Num_free_SHprimme(H, ctx));

   CHKERR(Num_free_RHprimme(hVals, ctx));

   CHKERR(Num_free_iprimme(flags, ctx));
   CHKERR(Num_free_iprimme(eval_perm, ctx));
   CHKERR(Num_free_iprimme(global_start, ctx));

   if(primme->projectionParams.projection == primme_proj_sketched)
   {
      CHKERR(Num_free_Sprimme(S_vals, ctx));
      CHKERR(Num_free_Sprimme(SV, ctx));
      CHKERR(Num_free_Sprimme(SW, ctx));
      CHKERR(Num_free_Rprimme(normalize_evecs, ctx));
      CHKERR(Num_free_Sprimme(SVhVecs, ctx));
      CHKERR(Num_free_Sprimme(SWhVecs, ctx));
      CHKERR(Num_free_Sprimme(V_temp, ctx));
      free(S_rows);
   }

   *ret = 0;

return 0;

#else
   (void)evals;
   (void)evecs;
   (void)ldevecs;
   (void)resNorms;
   (void)ret;
   (void)numRet;
   (void)fullOrtho;
   (void)ctx;

   CHKERR(PRIMME_FUNCTION_UNAVAILABLE);
   return 0;
#endif
}

#endif   // SUPPORT_TYPE