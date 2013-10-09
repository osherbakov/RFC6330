#ifndef RFC6330_H
#define RFC6330_H
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
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
} rfc6330_params_t;

typedef struct 
{
	unsigned int d;
	unsigned int a;
	unsigned int b;
	unsigned int d1;
	unsigned int a1;
	unsigned int b1;

} rfc6330_tuple_t;

void rfc6330_parameters(unsigned int K, rfc6330_params_t *params);

unsigned int rfc6330_deg(uint32_t v, unsigned int W);



void rfc6330_tuple( rfc6330_tuple_t *tuple, rfc6330_params_t *param, unsigned int ISI);

void rfc6330_A(unsigned char *Target, rfc6330_params_t *Params, unsigned int *ISIs, unsigned int NumSymbols);

int rfc6330_gf_gauss(unsigned char *Result, 
					  rfc6330_params_t * Params, 
					  unsigned char *A, 
					  unsigned char *Symbols, unsigned int BytesPerSymbol,
					  unsigned int NumSymbols);

void rfc6330_encode(unsigned  char *Result, rfc6330_params_t *Params, 
					unsigned char *IntermediateSymbols, unsigned int BytesPerSymbol, 
					unsigned int *ESIs, unsigned int Size);

void rfc6330_encode_block(unsigned char *Result, unsigned int *ESIs, 
						  unsigned int NumSymbols,
						  unsigned char *Source,  unsigned int BytesPerSymbol,
						  unsigned int NumSrcBytes);

int rfc6330_decode_block(unsigned char *Result, unsigned int NumResultBytes,  
						  unsigned char *Source, unsigned int BytesPerSymbol, 
						  unsigned int *ESIs, unsigned int NumSymbols);

//  Operations on the GF(256) Field 
unsigned char rfc6330_gf_power(unsigned int ii);

unsigned char rfc6330_gf_mult(unsigned char u, unsigned char v);

unsigned char rfc6330_gf_div(unsigned char u, unsigned char v);

void rfc6330_gf_mult_vec(unsigned char *result, unsigned char *v1, unsigned char *v2,  unsigned int len);

void rfc6330_gf_scale_vec(unsigned char *result, unsigned char *v1, unsigned char scale,  unsigned int len);

void rfc6330_gf_scale_xor_vec(unsigned char *result, unsigned char *v1, unsigned char scale,  unsigned int len);

void rfc6330_gf_mult_mat(unsigned char *Result,
						 unsigned char *H, unsigned int H_row, unsigned int H_col, 
						 unsigned char *G, unsigned int G_row, unsigned int G_col 
						  );
void rfc6330_gf_gamma(unsigned char *Target, unsigned Size );
//--------------------------

// Simple natrix operations - Identity and Zero
void rfc6330_eye(unsigned char *Target, unsigned int nStride, unsigned int Size);

void rfc6330_zero(unsigned char *Target, unsigned int nStride, unsigned int numRows, unsigned int numCols);

void rfc6330_copy_mat(unsigned char *Result, unsigned int nStride,
						 unsigned char *H, unsigned int H_stride,
						 unsigned int Rows, unsigned int Cols);

void rfc6330_mult_mat(unsigned char *Result,
						 unsigned char *H, unsigned int H_row, unsigned int H_col, 
						 unsigned char *G, unsigned int G_row, unsigned int G_col 
						  );

unsigned int rfc6330_count_nonzeroes(unsigned char *Vector, unsigned int Size);

void rfc6330_copy_vec(unsigned char *Result, unsigned char *Src, unsigned int Size);

void rfc6330_xor_vec(unsigned char *Result, unsigned char *v1, unsigned char *v2, unsigned int Size);

void rfc6330_swap_vec(unsigned char *v1, unsigned char *v2, unsigned int Size);

//--------------------------

uint32_t rfc6330_rand(uint32_t x, unsigned int ii, uint32_t m);

#endif
