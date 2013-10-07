/********************************
function ES = rfc6330_gen_encoding_symbol( K_prime, IntermediateSymbols, ISIs )
%
% Implements the LT encoding symbol generator as described in the RFC6330
% Section 5.3.5.3. LT Encoding Symbol Generator
% The addition of two octets u and v defined as the XOR operation, i.e.,u + v = u ^ v.
% Input:
%              K_prime - number of extended source symbols
%  IntermediateSymbols - 
%                 ISIs - Internal Symbol IDs
% Output:
%                   ES - Final Encoding Symbols
%

% compute rfc6330 code parameters
[S H B U L W P P1] = rfc6330_parameters( K_prime );

if length(IntermediateSymbols)~=L,
   error('Invalid number of intermediate symbols');
end;

% allocate space for encoding symbols
ES = zeros(1,length(ISIs));

for ii=1:length(ISIs)
    % compute (d,a,b,d1,a1,b1) tuple
    [ d, a, b, d1, a1, b1 ] = rfc6330_tuple( K_prime, ISIs(ii) );
    % compute encoding symbol 
    ES(ii) = IntermediateSymbols(b+1);
    %disp(['Encoding symbol ',int2str(ii),' = IntSym(',int2str(b),') = ',int2str(IntermediateSymbols(b+1))])
    for jj=1:(d-1),
        b = mod(b+a,W);
        ES(ii) = bitxor(ES(ii),IntermediateSymbols(b+1));
     %   disp(['IntSym ', int2str(b),' = ',int2str(IntermediateSymbols(b+1)),' added! ES = ',int2str(ES(ii))])
    end
    while (b1 >= P)
        b1 = mod(b1+a1,P1);
    end
    ES(ii) = bitxor(ES(ii),IntermediateSymbols(W+b1+1));
    %disp(['IntSym ', int2str(W+b1),' = ',int2str(IntermediateSymbols(W+b1+1)),' added! ES = ',int2str(ES(ii))])
    for jj=1:(d1-1)
        b1 = mod(b1+a1,P1);
        while (b1>=P)
            b1 = mod(b1+a1,P1);
        end
        ES(ii) = bitxor(ES(ii),IntermediateSymbols(W+b1+1));
     %   disp(['IntSym ', int2str(W+b1),' = ',int2str(IntermediateSymbols(W+b1+1)),' added! ES = ',int2str(ES(ii))])
    end
end
*********************************/
#include "rfc6330_func.h"

void rfc6330_encode_block(rfc6330_state_t *pState,
						  unsigned char *Result, unsigned int *ESIs,  
						  unsigned int NumSymbols,
						  unsigned char *Source, unsigned int BytesPerSymbol, 
						  unsigned int NumSrcBytes)
{
	unsigned int K, S, H, L, K_prime;
	unsigned int ROWS, COLS;

	K = (NumSrcBytes + (BytesPerSymbol >> 1))/BytesPerSymbol;
	if (K == 0) return;

	rfc6330_parameters(K, &pState->Params);
	K_prime = pState->Params.K_prime;
	S = pState->Params.S;
	H = pState->Params.H;
	L = pState->Params.L;
	
	ROWS = S + H + K_prime;
	COLS = L;
	// Zero pad the sourse symbols at the end to create an Extended block
	// Additionally, allocate S + H (or L - K_Prime) zero symbols at the beginning
	// Symbols = (unsigned char *) malloc(ROWS * BytesPerSymbol);
	memset(pState->Symbols, 0, ROWS  * BytesPerSymbol);
	memcpy(pState->Symbols + (S + H) * BytesPerSymbol, Source, NumSrcBytes);
	// Create ISI array
	// ISIs = (unsigned int *) malloc(K_prime * sizeof(unsigned int));
	for (unsigned int i = 0; i < K_prime; i++) pState->ISIs[i] = i;

	// Calculate the A matrix for intermediate symbols calculations
	// A = (unsigned char *) malloc( ROWS * COLS);
	rfc6330_A(pState->A, &pState->Params, pState->ISIs, K_prime);

	// Now calculate all intermediate symbols
	rfc6330_gf_gauss(0, &pState->Params, pState->A, pState->Symbols, BytesPerSymbol, ROWS);

	// Generate all the required symbols (systemic + repair).
	// Skip generation of the zero-padding symbols at the end
	for (unsigned int i=0; i < NumSymbols; i++) pState->AESIs[i] = i < K ? i : (K_prime - K) + i;
	rfc6330_encode(Result, &pState->Params, pState->Symbols, BytesPerSymbol, pState->AESIs, NumSymbols);
}

void rfc6330_encode(unsigned  char *Result, rfc6330_params_t *Params, 
					unsigned char *IntermediateSymbols, unsigned int BytesPerSymbol, 
					unsigned int *ESIs, unsigned int NumSymbols)
{
	rfc6330_tuple_t tuple;
	unsigned char *pData;
	for (unsigned int ii = 0; ii < NumSymbols; ii++)
	{
		pData = &Result[ii * BytesPerSymbol];
		rfc6330_tuple(&tuple, Params, ESIs[ii]);
		rfc6330_copy_vec(pData, &IntermediateSymbols[tuple.b * BytesPerSymbol], BytesPerSymbol);
		for(unsigned int jj = 1; jj < tuple.d; jj++)
		{
			tuple.b = (tuple.b + tuple.a) % Params->W;
			rfc6330_xor_vec(pData, pData, &IntermediateSymbols[tuple.b * BytesPerSymbol], BytesPerSymbol);
		}

		// Look for the appropriate index
		while (tuple.b1 >= Params->P)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
		}		
		rfc6330_xor_vec(pData, pData, &IntermediateSymbols[(Params->W + tuple.b1) * BytesPerSymbol], BytesPerSymbol);

		for(unsigned int jj = 1; jj < tuple.d1; jj++)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
			// Look for the appropriate index
			while (tuple.b1 >= Params->P)
			{
				tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
			}
			rfc6330_xor_vec(pData, pData, &IntermediateSymbols[(Params->W + tuple.b1) * BytesPerSymbol], BytesPerSymbol);
		}
	}
}
