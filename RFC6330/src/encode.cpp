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
#include "func.h"

static unsigned int ISIs[10];
static unsigned char A[756];
static unsigned char Symbols[270];

void encode_block(unsigned char *Result, unsigned int *ESIs,  
						  unsigned int NumSymbols,
						  unsigned char *Source, unsigned int BytesPerSymbol, 
						  unsigned int NumSrcBytes)
{
	params_t Params;
	unsigned int K, S, H, K_prime;
	unsigned int ROWS;

	K = (NumSrcBytes + (BytesPerSymbol >> 1))/BytesPerSymbol;
	if (K == 0) return;

	calc_parameters(K, &Params);
	K_prime = Params.K_prime;
	S = Params.S;
	H = Params.H;
	
	ROWS = S + H + K_prime;
	// Zero pad the sourse symbols at the end to create an Extended block
	// Additionally, allocate S + H (or L - K_Prime) zero symbols at the beginning
	memset(Symbols, 0, ROWS  * BytesPerSymbol);
	memcpy(Symbols + (S + H) * BytesPerSymbol, Source, NumSrcBytes);
	// Create ISI array
	for (unsigned int i=0; i < K_prime; i++) ISIs[i] = i;

	// Calculate the A matrix for intermediate symbols calculations
	create_A(A, &Params, ISIs, K_prime);

	// Now calculate all intermediate symbols
	gf_gauss(0, &Params, A, Symbols, BytesPerSymbol, ROWS);

	// Generate all the required symbols (systemic + repair).
	// Skip generation of the zero-padding symbols at the end
	for (unsigned int i=0; i < NumSymbols; i++) ESIs[i] = i < K ? i : (K_prime - K) + i;
	encode(Result, &Params, Symbols, BytesPerSymbol, ESIs, NumSymbols);
}

void encode(unsigned  char *Result, params_t *Params, 
					unsigned char *IntermediateSymbols, unsigned int BytesPerSymbol, 
					unsigned int *ESIs, unsigned int NumSymbols)
{
	tuple_t tuple;
	unsigned char *pData;
	for (unsigned int ii = 0; ii < NumSymbols; ii++)
	{
		pData = &Result[ii * BytesPerSymbol];
		calc_tuple(&tuple, Params, ESIs[ii]);
		vec_copy(pData, &IntermediateSymbols[tuple.b * BytesPerSymbol], BytesPerSymbol);
		for(unsigned int jj = 1; jj < tuple.d; jj++)
		{
			tuple.b = (tuple.b + tuple.a) % Params->W;
			vec_xor(pData, pData, &IntermediateSymbols[tuple.b * BytesPerSymbol], BytesPerSymbol);
		}

		// Look for the appropriate index
		while (tuple.b1 >= Params->P)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
		}		
		vec_xor(pData, pData, &IntermediateSymbols[(Params->W + tuple.b1) * BytesPerSymbol], BytesPerSymbol);

		for(unsigned int jj = 1; jj < tuple.d1; jj++)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
			// Look for the appropriate index
			while (tuple.b1 >= Params->P)
			{
				tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
			}
			vec_xor(pData, pData, &IntermediateSymbols[(Params->W + tuple.b1) * BytesPerSymbol], BytesPerSymbol);
		}
	}
}
