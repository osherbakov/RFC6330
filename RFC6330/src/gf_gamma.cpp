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
#include "RFC6330func.h"

void gf_gamma(unsigned char *Target, unsigned Size)
{
	unsigned char *Gamma;
	unsigned int ii, jj;
	mat_zero(Target, Size, Size, Size);
	for (jj = 0; jj < Size; jj++)
	{
		Gamma = Target +  jj * Size + jj;
		for (ii = jj; ii < Size; ii++)
		{
			*Gamma = gf_power(ii - jj);
			Gamma += Size;
		}
	}
}