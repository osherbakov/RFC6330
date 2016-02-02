#ifndef __RFC6330_FUNC_H__
#define __RFC6330_FUNC_H__
#include "stdlib.h"
#include "string.h"
#include <stdint.h>

/**********************
	%   S - number of LDPC symbols
	%   H - number of HDPC symbols
	%   P - number of permanently inactive symbols (HDPC + set of K_prime)
	%   W - number of LT symbols (LDPC + set of K_prime)
S H B U L W P P1
***********************/
typedef struct  
{
	unsigned int K_prime;
	unsigned int J; 
	unsigned int S;		// number of LDPC symbols
	unsigned int H;		// number of HDPC symbols
	unsigned int W;		// number of LT symbols
	unsigned int B;		// 
	unsigned int U;
	unsigned int L;
	unsigned int P;
	unsigned int P1;
	unsigned int tupleA;
	unsigned int tupleB;
} params_t;

typedef struct 
{
	unsigned int d;
	unsigned int a;
	unsigned int b;
	unsigned int d1;
	unsigned int a1;
	unsigned int b1;

} tuple_t;

void calc_parameters(unsigned int K, params_t *params);

unsigned int degree(uint32_t v, unsigned int W);

void calc_tuple( tuple_t *tuple, params_t *param, unsigned int ISI);

void create_A(unsigned char *Target, params_t *Params, unsigned int *ISIs, unsigned int NumSymbols);

void encode_run(unsigned  char *Result, params_t *Params, 
					unsigned char *IntermediateSymbols, unsigned int BytesPerSymbol, 
					unsigned int *ESIs, unsigned int Size);

//  Operations on the GF(256) Field 
#define gf_add(a, b)  ((a) ^ (b))

#define gf_sub(a, b)  ((a) ^ (b))

unsigned char gf_power(unsigned int ii);

unsigned char gf_mult(unsigned char u, unsigned char v);

unsigned char gf_div(unsigned char u, unsigned char v);

void gf_vec_mult(unsigned char *result, unsigned char *v1, unsigned char *v2,  unsigned int len);

void gf_vec_scale(unsigned char *result, unsigned char *v1, unsigned char scale,  unsigned int len);

void gf_vec_scale_xor(unsigned char *result, unsigned char *v1, unsigned char scale,  unsigned int len);

#define gf_vec_scale_add gf_vec_scale_xor
#define gf_vec_scale_sub gf_vec_scale_xor

void gf_mat_mult(unsigned char *Result,
						 unsigned char *H, unsigned int H_row, unsigned int H_col, 
						 unsigned char *G, unsigned int G_row, unsigned int G_col 
						  );
void gf_gamma(unsigned char *Target, unsigned Size );

int gf_gauss(unsigned char *Result, 
					  params_t * Params, 
					  unsigned char *A, 
					  unsigned char *Symbols, unsigned int BytesPerSymbol,
					  unsigned int NumSymbols);
//--------------------------

// Simple matrix operations - Identity and Zero
void mat_eye(unsigned char *Target, unsigned int nStride, unsigned int Size);

void mat_zero(unsigned char *Target, unsigned int nStride, unsigned int numRows, unsigned int numCols);

void mat_copy(unsigned char *Result, unsigned int nStride,
						 unsigned char *H, unsigned int H_stride,
						 unsigned int Rows, unsigned int Cols);

void mat_mult(unsigned char *Result,
						 unsigned char *H, unsigned int H_row, unsigned int H_col, 
						 unsigned char *G, unsigned int G_row, unsigned int G_col 
						  );

unsigned int count_nonzeroes(unsigned char *Vector, unsigned int Size);

void vec_copy(unsigned char *Result, unsigned char *Src, unsigned int Size);

void vec_xor(unsigned char *Result, unsigned char *v1, unsigned char *v2, unsigned int Size);

void vec_swap(unsigned char *v1, unsigned char *v2, unsigned int Size);

//--------------------------

uint32_t rfc6330_rand(uint32_t x, unsigned int ii, uint32_t m);

// Target-specific function for memory allocation and de-allocation
void osHeapInit(void);
void *osAlloc(unsigned int size);
void osFree(void *pMem);

#endif
