#include "rfc6330_func.h"


void rfc6330_A(unsigned char *Target, rfc6330_params_t *Params, unsigned int *ISIs, unsigned int NumSymbols)
{
	unsigned int Rows, Cols;
	unsigned char *Gamma;
	unsigned int S, H, B, P, W, L, U, K_prime, P1;
	K_prime = Params->K_prime;
	S = Params->S;
	H = Params->H;
	B = Params->B;
	P = Params->P;
	W = Params->W;
	L = Params->L;
	U = Params->U;
	P1 = Params->P1;

	Rows = S + H + NumSymbols;
	Cols = L;
	Gamma = (unsigned char *) malloc(Rows * Cols);
	rfc6330_zero(Target, Cols, Rows, Cols);
	/******************
% LDPC Symbols
% See section 5.3.3.3 Pre-coding relationships
for ii = 0:(B-1)
    a           = 1 + floor(ii/S);
    b           = mod(ii,S);
    A(b+1,ii+1) = bitxor(A(b+1,ii+1),1);
    b           = mod( (a+b), S );
    A(b+1,ii+1) = bitxor(A(b+1,ii+1),1);
    b           = mod((a+b),S);
    A(b+1,ii+1) = bitxor(A(b+1,ii+1),1);
end
	*******************/
	unsigned int a, b;
	for (unsigned int ii = 0; ii < B; ii++)
	{
		a = 1 + ii/S;
		b = ii % S;
		Target[b * Cols + ii] ^= 1;
		b = (a + b) % S;
		Target[b * Cols + ii] ^= 1;
		b = (a + b) % S;
		Target[b * Cols + ii] ^= 1;
	}
	/*********************
for ii = 0:(S-1)
    a = mod(ii,P);
    b = mod(ii+1,P);
    A(ii+1,W+a+1) = bitxor(A(ii+1,W+a+1),1);
    A(ii+1,W+b+1) = bitxor(A(ii+1,W+b+1),1);
	%     A(mod(ii,S)+1,W+a+1) = bitxor(A(mod(ii,S)+1,W+a+1),1);
	%     A(mod(ii,S)+1,W+b+1) = bitxor(A(mod(ii,S)+1,W+b+1),1);
end
	**********************/	
	for(unsigned int ii = 0; ii < S; ii++)
	{
		a = ii % P;
		b = (ii+1) % P;
		Target[ii * Cols + W + a] ^= 1;
		Target[ii * Cols + W + b] ^= 1;
	}

	/*************************
	% insert SxS identity matrix
	A(1:S,B+1:B+S) = eye(S);%     A(ii+1,W+a+1) = bitxor(A(ii+1,W+a+1),1);
	%     A(ii+1,W+b+1) = bitxor(A(ii+1,W+b+1),1);
	**************************/
	rfc6330_eye(&Target[0 * Cols + B], Cols, S);

	/*************************
	% HDPC Symbols
% See section 5.3.3.3 Pre-coding relationships
% MT denote an H x (K' + S) matrix of octets,
% Matrix MT generation
for jj = 1:(K_prime + S - 1)
    ii = rfc6330_rand( jj, 6, H );
    A(S+ii+1,jj) = bitxor(A(S+ii+1,jj),1);
    %A(S+ii+1,jj) = 1;
    ii = mod(rfc6330_rand( jj, 6, H )+rfc6330_rand( jj, 7, H-1 )+1, H);
    A(S+ii+1,jj) = bitxor(A(S+ii+1,jj),1);
    %A(S+ii+1,jj) = 1;
end
	**************************/
	for(unsigned int jj = 1; jj < (K_prime + S);jj++)
	{
		int ii = rfc6330_rand(jj, 6, H);
		Target[(S + ii)*Cols + jj - 1] ^= 1;
		ii = ( rfc6330_rand(jj, 6, H) + rfc6330_rand(jj, 7, H - 1) + 1) % H;
		Target[(S + ii)*Cols + jj - 1] ^= 1;
	}

/*****************************
A((S + 1):(S + H),K_prime + S) = rfc6330_gfpower(0:(H-1)); 
%Formation of the last coloumn of matrix MT
******************************/
	unsigned int Column = (K_prime + S) - 1;
	for(unsigned int i = 0; i < H; i++)
	{
		Target[(S + i) * Cols + Column] = rfc6330_gf_power(i);
	}

/*****************************
% GAMMA denote the a (K'+S ) x (K'+S ) matrix of octets
% G_HDPC=MT*GAMMA; the multiplication is not normal matrix multiplication
% but GF matrix multiplication
A((S + 1):(S + H),1:(K_prime + S)) = ...
    rfc6330_gfMatrixMult( A((S + 1):(S + H),1:(K_prime + S)), ...
    rfc6330_gamma( K_prime, S ) );
******************************/	
	unsigned int GammaSize = K_prime + S;
	unsigned char *Tmp1 =  (unsigned char *) malloc(H * GammaSize);
	unsigned char *Tmp2 =  (unsigned char *) malloc(H * GammaSize);

	rfc6330_gf_gamma(Gamma, GammaSize);
	rfc6330_copy_mat(Tmp1, GammaSize, &Target[S * Cols + 0], Cols, H, GammaSize);

	rfc6330_gf_mult_mat(Tmp2, Tmp1, H, GammaSize, Gamma, GammaSize, GammaSize);
	rfc6330_copy_mat(&Target[S * Cols + 0], Cols, Tmp2, GammaSize, H, GammaSize);

	/******************************
	% insert HxH identity matrix
	A(S+1:S+H,B+S+U+1:B+S+U+H) = eye(H);
	*******************************/
	rfc6330_eye(&Target[S * Cols + B + S + U], Cols, H);

	/*******************************
	% rows which are ISI-specific
	% See section 5.3.5.3. LT Encoding Symbol Generator
for ii = 1:length(ISIs)
    % obtain (d,a,b) triple for given ISI
    [ d, a, b, d1, a1, b1 ] = rfc6330_tuple( K_prime, ISIs(ii) );
    A(ii+S+H,1+b) = 1;
    for jj = 1:(d-1)
        b = mod(b+a,W);
        A(ii+S+H,1+b) = bitxor(A(ii+S+H,1+b),1);
    end
    while (b1>=P)
        b1 = mod(b1+a1,P1);
    end
    A(ii+S+H,1+b1+W) = bitxor(A(ii+S+H,1+b1+W),1);
    for jj = 1:(d1-1)
        b1 = mod(b1+a1,P1);
        while (b1>=P)
            b1 = mod(b1+a1,P1);
        end
        A(ii+S+H,1+b1+W) = bitxor(A(ii+S+H,1+b1+W),1);
    end
end
	********************************/

	rfc6330_tuple_t tuple;
	for(unsigned int ii = 0; ii < NumSymbols; ii++)
	{
		rfc6330_tuple(&tuple, Params, ISIs[ii]);

		Target[ (ii + S + H) * Cols + tuple.b] = 1;
		for(unsigned int jj = 1; jj < tuple.d; jj++)
		{
			tuple.b = (tuple.b + tuple.a) % W;
			Target[ (ii + S + H) * Cols + tuple.b] ^= 1;
		}
		while (tuple.b1 >= P)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % P1;
		}
		
		Target[ (ii + S + H) * Cols + tuple.b1 + W] ^= 1;

		for(unsigned int jj = 1; jj < tuple.d1; jj++)
		{
			tuple.b1 = (tuple.b1 + tuple.a1) % P1;
			while (tuple.b1 >= P)
			{
				tuple.b1 = (tuple.b1 + tuple.a1) % P1;
			}
			Target[ (ii + S + H) * Cols + tuple.b1 + W] ^= 1;
		}
	}
        free(Tmp2);
	free(Tmp1);
	free(Gamma);
}
