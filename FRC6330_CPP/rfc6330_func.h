#ifndef RFC6330_H
#define RFC6330_H

unsigned int rfc6330_deg(
	unsigned int v,
	unsigned int W);

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

unsigned int rfc6330_K_prime(unsigned int K, unsigned int *J, 
	unsigned int *S, unsigned int *H, unsigned int *W);

void rfc6330_parameters(unsigned int K, rfc6330_params_t *params);


void rfc6330_tuple( rfc6330_tuple_t *tuple, rfc6330_params_t *param, unsigned int ISI);

void rfc6330_A(unsigned char *Target, rfc6330_params_t *Params, unsigned int *ISIs, unsigned int NumSymbols);

int rfc6330_gf_gauss(unsigned char *Result, 
					  unsigned char *A, 
					  unsigned char *Symbols, unsigned int Size);

//  Operations on the GF(256) Field 
unsigned char rfc6330_gf_power(unsigned int ii);

unsigned char rfc6330_gf_mult(unsigned char u, unsigned char v);

unsigned char rfc6330_gf_div(unsigned char u, unsigned char v);

void rfc6330_gf_mult_vec(unsigned char *result, unsigned char *v1, unsigned char *v2,  unsigned int len);

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
						 unsigned char *H, unsigned int H_row, unsigned int H_stride, 
						 unsigned char *G, unsigned int G_col, unsigned int G_stride,
						 unsigned int H_Col_G_Row);

//--------------------------

unsigned int rfc6330_rand(unsigned int x, unsigned int ii, unsigned int m);

#endif