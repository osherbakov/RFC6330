/****************************
function Gamma = rfc6330_gamma( K_prime, S )
	%
	% Calculates matrix Gamma needed for the GF(256) arithmetic.
	% The construction is described in section 5.3.3.3.
	% Field : GF(256)
	% Irreducible polynomial: 
	%        f(x) = x^8 + x^4 + x^3 + x^2 + 1 = 0x100011101b
	% Primitive Element: alpha = x = 0x10b
	%
	% Input:
	%  K_prime - number of symbols in the extended source block
	%        S - number of LDPC symbols
	% Output:
	%    Gamma - (K_prime + S)x(K_prime + S) matrix with elements from GF(256)
	%
	%
	%Gamma = zeros( K_prime + S );
	for jj = 1:(K_prime + S)
		for ii = jj:(K_prime + S)
			Gamma(ii,jj) = rfc6330_gfpower(ii-jj);
		end
	end
*****************/
#include "rfc6330_func.h"

void rfc6330_gf_gamma(unsigned char *Target, unsigned Size)
{
	unsigned char *Gamma;
	rfc6330_zero(Target, Size, Size, Size);
	for (unsigned int jj = 0; jj < Size; jj++)
	{
		Gamma = Target +  jj * Size + jj;
		for (unsigned int ii = jj; ii < Size; ii++)
		{
			*Gamma = rfc6330_gf_power(ii - jj);
			Gamma += Size;
		}
	}
}