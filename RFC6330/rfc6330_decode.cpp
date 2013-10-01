#include "rfc6330_func.h"

int rfc6330_decode_block(unsigned char *Result, unsigned int NumResultBytes,  
						  unsigned char *Source, unsigned int BytesPerSymbol, 
						  unsigned int *ESIs, unsigned int NumSymbols)
{
	rfc6330_params_t Params;
	unsigned int K, S, H, L, K_prime;
//	unsigned int B, P, W, U, P1;
	unsigned int *ISIs, *AESIs;
	unsigned char *A;
	unsigned char *Symbols;
	unsigned int ROWS, COLS, ANS;
	int ret = -1;

	K = (NumResultBytes + (BytesPerSymbol >> 1))/BytesPerSymbol;
	if ( (K == 0) || (K > NumSymbols)) return -1;

	rfc6330_parameters(K, &Params);
	K_prime = Params.K_prime;
	S = Params.S;
	H = Params.H;
//	B = Params.B;
//	P = Params.P;
//	W = Params.W;
	L = Params.L;
//	U = Params.U;
//	P1 = Params.P1;
	
	// We know that the symbols from K to K_prime are zero-filled
	ANS /* Actual Number of Symbols */ = NumSymbols + (K_prime - K);

	ROWS = S + H + ANS;	// LDPC + HDPC + Data
	COLS = L;			// Number of intermediate symbols
	// Zero pad the sourse symbols (K' - K) at the end to create an Extended block
	// Additionally, allocate S + H (or L - K_Prime) zero symbols at the beginning
	Symbols = (unsigned char *) malloc(ROWS * BytesPerSymbol);
	memset(Symbols, 0, ROWS * BytesPerSymbol);
	memcpy(Symbols + (S + H) * BytesPerSymbol, Source, NumSymbols * BytesPerSymbol);

	// Generate AESI (Actual ESI) for all the symbols (systemic + repair + ZeroFill).
	AESIs = (unsigned int *) malloc(ANS * sizeof(unsigned int));
	memcpy(AESIs, ESIs, NumSymbols * sizeof(unsigned int)); 
	for (unsigned int i = NumSymbols; i < ANS; i++) AESIs[i] = K + (i - NumSymbols) ;

	// Create ISI array - it will be used to generate the original symbols
	ISIs = (unsigned int *) malloc(K * sizeof(unsigned int));
	for (unsigned int i=0; i < K; i++) ISIs[i] = i;

	// Calculate the A matrix for intermediate symbols calculations
	A = (unsigned char *) malloc( ROWS * COLS);
	rfc6330_A(A, &Params, AESIs, ANS);

	// Now calculate all COLS intermediate symbols
	if( rfc6330_gf_gauss(0, &Params, A, Symbols, BytesPerSymbol, ROWS) == 0)
	{
		// Generate the original symbols
		rfc6330_encode(Result, &Params, Symbols, BytesPerSymbol, ISIs, K);
        ret = 0;
	}

	free(A);
	free(ISIs);
	free(AESIs);
	free(Symbols);
	return ret;
}
