#include "RFC6330func.h"


int decode_block(unsigned char *Result, unsigned int NumResultBytes,  
						  unsigned char *Source, unsigned int BytesPerSymbol, 
						  unsigned int *ESIs, unsigned int NumSymbols)
{
	params_t Params;
	unsigned int NumResultSymbols, L, S, H, K_prime;
	unsigned int ROWS, ANS;
	unsigned int i;
	unsigned int	*pISI, *pAESI;
	unsigned char	*pA, *pSymbols;
	int ret = -1;

	// NumResultSymbols is the actual number of symbols that we will need at the end  
	NumResultSymbols = (NumResultBytes + (BytesPerSymbol >> 1))/BytesPerSymbol;
	if ( (NumResultSymbols == 0) || (NumResultSymbols > NumSymbols)) return -1;

	calc_parameters(NumResultSymbols, &Params);
	K_prime = Params.K_prime;
	S = Params.S;
	H = Params.H;
	L = Params.L;
	
	// We know that the symbols from K to K_prime are zero-filled
	ANS /* Actual Number of Symbols */ = NumSymbols + (K_prime - NumResultSymbols);

	ROWS = S + H + ANS;	// LDPC + HDPC + Data
	// Zero pad the sourse symbols (K' - K) at the end to create an Extended block
	// Additionally, allocate S + H (or L - K_Prime) zero symbols at the beginning
	pSymbols = (unsigned char *)osAlloc(ROWS * BytesPerSymbol);
	memset(pSymbols, 0, ROWS * BytesPerSymbol);
	memcpy(pSymbols + (S + H) * BytesPerSymbol, Source, NumSymbols * BytesPerSymbol);

	// Generate AESI (Actual ESI) for all the symbols (systemic + repair + ZeroFill).
	pAESI = (unsigned int *) osAlloc(ANS * sizeof(unsigned int));
	memcpy(pAESI, ESIs, NumSymbols * sizeof(unsigned int)); 
	for (i = NumSymbols; i < ANS; i++) pAESI[i] = NumResultSymbols + (i - NumSymbols) ;

	// Create ISI array - it will be used to generate the original symbols
	pISI = (unsigned int *)osAlloc(NumResultSymbols * sizeof(unsigned int));
	for (i = 0; i < NumResultSymbols; i++) pISI[i] = i;

	pA = (unsigned char *)osAlloc(ROWS * L);
	// Calculate the A matrix for intermediate symbols calculations
	create_A(pA, &Params, pAESI, ANS);

	// Now calculate all COLS intermediate symbols
	if( gf_gauss(0, &Params, pA, pSymbols, BytesPerSymbol, ROWS) == 0)
	{
		// Generate the original symbols
		encode_run(Result, &Params, pSymbols, BytesPerSymbol, pISI, NumResultSymbols);
        ret = 0;
	}

	osFree(pA);
	osFree(pISI);	
	osFree(pAESI);
	osFree(pSymbols);
	return ret;
}
