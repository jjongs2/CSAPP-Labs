/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

#define BLOCK_SIZE 8
#define BLOCK_SIZE_HALF 4
#define BLOCK_SIZE_61x67 23

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

static void transpose_32x32(int M, int N, int A[N][M], int B[M][N]);
static void transpose_64x64(int M, int N, int A[N][M], int B[M][N]);
static void transpose_61x67(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32 && N == 32)
        transpose_32x32(M, N, A, B);
    else if (M == 64 && N == 64)
        transpose_64x64(M, N, A, B);
    else if (M == 61 && N == 67)
        transpose_61x67(M, N, A, B);
}

static void transpose_32x32(int M, int N, int A[N][M], int B[M][N]) {
    int r0, c0, r;
    int t0, t1, t2, t3, t4, t5, t6, t7;

    for (r0 = 0; r0 < N; r0 += BLOCK_SIZE) {
        for (c0 = 0; c0 < M; c0 += BLOCK_SIZE) {
            for (r = r0; r < r0 + BLOCK_SIZE; ++r) {
                t0 = A[r][c0];
                t1 = A[r][c0 + 1];
                t2 = A[r][c0 + 2];
                t3 = A[r][c0 + 3];
                t4 = A[r][c0 + 4];
                t5 = A[r][c0 + 5];
                t6 = A[r][c0 + 6];
                t7 = A[r][c0 + 7];
                B[c0][r] = t0;
                B[c0 + 1][r] = t1;
                B[c0 + 2][r] = t2;
                B[c0 + 3][r] = t3;
                B[c0 + 4][r] = t4;
                B[c0 + 5][r] = t5;
                B[c0 + 6][r] = t6;
                B[c0 + 7][r] = t7;
            }
        }
    }
}

static void transpose_64x64(int M, int N, int A[N][M], int B[M][N]) {
    int r0, c0, r, l;
    int t0, t1, t2, t3, t4, t5, t6, t7;

    for (r0 = 0; r0 < N; r0 += BLOCK_SIZE) {
        for (c0 = 0; c0 < M; c0 += BLOCK_SIZE) {
            for (r = r0; r < r0 + BLOCK_SIZE_HALF; ++r) {
                t0 = A[r][c0];
                t1 = A[r][c0 + 1];
                t2 = A[r][c0 + 2];
                t3 = A[r][c0 + 3];
                t4 = A[r][c0 + 4];
                t5 = A[r][c0 + 5];
                t6 = A[r][c0 + 6];
                t7 = A[r][c0 + 7];
                B[c0][r] = t0;
                B[c0 + 1][r] = t1;
                B[c0 + 2][r] = t2;
                B[c0 + 3][r] = t3;
                B[c0 + 3][r + 4] = t4;
                B[c0 + 2][r + 4] = t5;
                B[c0 + 1][r + 4] = t6;
                B[c0][r + 4] = t7;
            }
            /*
             * r: 4 → 5 → 6 → 7 (rightward for A, downward for B)
             * l: 3 → 2 → 1 → 0 (leftward for A, upward for B)
             */
            for (r = BLOCK_SIZE_HALF; r < BLOCK_SIZE; ++r) {
                l = 7 - r;
                t0 = A[r0 + 4][c0 + l];
                t1 = A[r0 + 5][c0 + l];
                t2 = A[r0 + 6][c0 + l];
                t3 = A[r0 + 7][c0 + l];
                t4 = A[r0 + 4][c0 + r];
                t5 = A[r0 + 5][c0 + r];
                t6 = A[r0 + 6][c0 + r];
                t7 = A[r0 + 7][c0 + r];
                B[c0 + r][r0 + 0] = B[c0 + l][r0 + 4];
                B[c0 + r][r0 + 1] = B[c0 + l][r0 + 5];
                B[c0 + r][r0 + 2] = B[c0 + l][r0 + 6];
                B[c0 + r][r0 + 3] = B[c0 + l][r0 + 7];
                B[c0 + l][r0 + 4] = t0;
                B[c0 + l][r0 + 5] = t1;
                B[c0 + l][r0 + 6] = t2;
                B[c0 + l][r0 + 7] = t3;
                B[c0 + r][r0 + 4] = t4;
                B[c0 + r][r0 + 5] = t5;
                B[c0 + r][r0 + 6] = t6;
                B[c0 + r][r0 + 7] = t7;
            }
        }
    }
}

static void transpose_61x67(int M, int N, int A[N][M], int B[M][N]) {
    int r0, c0, r, c;

    for (r0 = 0; r0 < N; r0 += BLOCK_SIZE_61x67)
        for (c0 = 0; c0 < M; c0 += BLOCK_SIZE_61x67)
            for (r = r0; r < r0 + BLOCK_SIZE_61x67 && r < N; ++r)
                for (c = c0; c < c0 + BLOCK_SIZE_61x67 && c < M; ++c)
                    B[c][r] = A[r][c];
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

