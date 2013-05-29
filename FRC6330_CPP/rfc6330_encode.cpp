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

void rfc6330_encode(unsigned  char *Result, rfc6330_params_t *Params, 
					unsigned char *IntermediateSymbols, 
					unsigned int *ISIs, unsigned int Size)
{
	rfc6330_tuple_t tuple;
	unsigned char Data;
	for (unsigned int ii = 0; ii < Size; ii++)
	{
		rfc6330_tuple(&tuple, Params, ISIs[ii]);
		Data = IntermediateSymbols[tuple.b];
		for(unsigned int jj = 1; jj < tuple.d; jj++)
		{
			tuple.b = (tuple.b + tuple.a) % Params->W;
			Data ^= IntermediateSymbols[tuple.b];
		}

		// Look for the appropriate index
		while (tuple.b1 >= Params->P)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
		}		
		Data ^= IntermediateSymbols[Params->W + tuple.b1];

		for(unsigned int jj = 1; jj < tuple.d1; jj++)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
			// Look for the appropriate index
			while (tuple.b1 >= Params->P)
			{
				tuple.b1 = (tuple.b1 + tuple.a1) % Params->P1;
			}
			Data ^= IntermediateSymbols[Params->W + tuple.b1];
		}
		
		Result[ii] = Data;		 
	}
}