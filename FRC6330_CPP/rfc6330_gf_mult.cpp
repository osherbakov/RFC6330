/***************
%
	% Returns the value of the multiplication in GF(256) according to the
	% rfc6330 specification.
	%
	%The multiplication of two octets is defined with the help of two
	%tables: GF256 OCT_EXP, and GF256 OCT_LOG.  
	%Section 5.7.2 and Section 5.7.3, respectively. The table OCT_LOG maps octets to
	%integers, and OCT_EXP maps integers to octets. For two octets u and v, we define
	%u * v =
	%           0, if either u or v are 0,
	%           OCT_EXP[OCT_LOG[u] + OCT_LOG[v]] otherwise.
	%
	% OCT_EXP(510 Entries):The table OCT_EXP contains 512510 octets. The indexing starts at 0, and
	% ranges up to 509, and the entries are the octets with the following positive integer representation:look-up table
	% OCT_LOG(255 Entries),The octet corresponding
	% Indexing starts at 0, to the zero element, which is represented by the integer 0, is
	% excluded as an index, and thus indexing starts at 1 and ranges up to 255:
% look-up table
***************/
unsigned char OCT_EXP[] = { 1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
	76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192, 157,
	39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35,
	70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222,
	161, 95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60,
	120, 240, 253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163,
	91, 182, 113, 226, 217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52,
	104, 208, 189, 103, 206, 129, 31, 62, 124, 248, 237, 199, 147, 59,
	118, 236, 197, 151, 51, 102, 204, 133, 23, 46, 92, 184, 109, 218,
	169, 79, 158, 33, 66, 132, 21, 42, 84, 168, 77, 154, 41, 82, 164, 85,
	170, 73, 146, 57, 114, 228, 213, 183, 115, 230, 209, 191, 99, 198,
	145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255, 227, 219, 171,
	75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65, 130, 25,
	50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166, 81,
	162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
	18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11,
	22, 44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71,
	142, 1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
	76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192,
	157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159,
	35, 70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111,
	222, 161, 95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30,
	60, 120, 240, 253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223,
	163, 91, 182, 113, 226, 217, 175, 67, 134, 17, 34, 68, 136, 13, 26,
	52, 104, 208, 189, 103, 206, 129, 31, 62, 124, 248, 237, 199, 147,
	59, 118, 236, 197, 151, 51, 102, 204, 133, 23, 46, 92, 184, 109, 218,
	169, 79, 158, 33, 66, 132, 21, 42, 84, 168, 77, 154, 41, 82, 164, 85,
	170, 73, 146, 57, 114, 228, 213, 183, 115, 230, 209, 191, 99, 198,
	145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255, 227, 219, 171,
	75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65, 130, 25,
	50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166, 81,
	162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
	18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11,
	22, 44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71,
	142 };

unsigned char OCT_LOG[] = {
	0,		// Extra entry to account for 0-based index
	0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75, 4,
	100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113, 5,
	138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69,
	29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 
	166, 6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145,
	34, 136, 54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92,
	131, 56, 70, 64, 30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40,
	84, 250, 133, 186, 61, 202, 94, 155, 159, 10, 21, 121, 43, 78, 212,
	229, 172, 115, 243, 167, 87, 7, 112, 192, 247, 140, 128, 99, 13, 103,
	74, 222, 237, 49, 197, 254, 24, 227, 165, 153, 119, 38, 184, 180,
	124, 17, 68, 146, 217, 35, 32, 137, 46, 55, 63, 209, 91, 149, 188,
	207, 205, 144, 135, 151, 178, 220, 252, 190, 97, 242, 86, 211, 171,
	20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162, 31, 45, 67, 216,
	183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246, 108, 161,
	59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90, 203,
	89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215,
	79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80,
	88, 175 };

unsigned char rfc6330_gf_mult(unsigned char u, unsigned char v)
{
	unsigned char ret = 0;
	if((u==0)||(v==0))
		ret = 0;
	else
		ret = OCT_EXP[ OCT_LOG[u] + OCT_LOG[v] ];
	return ret;
}

//unsigned char rfc6330_gf_div2(unsigned char u, unsigned char v)
//{
//	unsigned char ret = 0;
//	if((u==0)||(v==0))
//		ret = 0;
//	else
//	{
//		int idx = OCT_LOG[u] - OCT_LOG[v];
//		if(idx < 0) idx += 510; 
//		ret = OCT_EXP[ idx ];
//	}
//	return ret;
//}

unsigned char rfc6330_gf_div(unsigned char u, unsigned char v)
{
	unsigned char ret = 0;
	if((u==0)||(v==0))
		ret = 0;
	else
	{
		int idx = (int)OCT_LOG[u] - (int)OCT_LOG[v] + 255;
		ret = OCT_EXP[ idx ];
	}
	return ret;
}
// Old and simple (but ineffective) version of division
//unsigned char rfc6330_gf_div(unsigned char u, unsigned char v)
//{
//	unsigned char ret = 0;
//	if((u==0)||(v==0))
//		ret = 0;
//	else
//	{	
//		for(ret = 0; ret <= 255; ret++)
//		{
//			if(u ==  rfc6330_gf_mult(v, ret))
//				break;
//		}
//	}
//	return ret;
//}

/***************
function p = rfc6330_gfpower( ii )
	%
	% Calculates the power, ii, of the primitive element alpha of the GF(256)
	%The octet alpha is the octet with the integer representation we
	%alpha^^i = OCT_EXP[i].

	%
	% Input: ii: Integer in {0,255} (forced to be, if not)
	% Output: 
	%     p = OCT_EXP( mod( ii, 255) + 1 );

******************/

unsigned char rfc6330_gf_power(unsigned int ii)
{
	return OCT_EXP[ii & 0x000000FF];
}

/************************
function val = rfc6330_gfmult_elementwise( v1, v2 )
%
% Returns the elementwise multiplication of the two vectors v1, v2 in the
% GF(256).
%

if (length(v1) ~= length(v2))
	error('Input vector sizes not equal!')
end
val = zeros(1,length(v1));
for ii = 1:length(v1)
	if ((v1(ii) == 0)||(v2(ii) == 0))
		val(ii) = 0;
	elseif (v1(ii) == 1)
		val(ii) = v2(ii);
	elseif (v2(ii) == 1)
		val(ii) = v1(ii);
	else
		val(ii) = rfc6330_gfmult(v1(ii),v2(ii));
	end
end
*************************/
void rfc6330_gf_mult_vec(unsigned char *result, unsigned char *v1, unsigned char *v2,  unsigned int len)
{
	while(len--)
	{
		*result++ = rfc6330_gf_mult(*v1++, *v2++);
	}
}

/****************************
unction A = rfc6330_gfMatrixMult( H, G )
%
% Matrix multiplication where both H, G matrices have elements in GF(256)
%
% Input: 
%  H,G - Input matrices
% Output: 
%    A - Output matrix - A = H*G
%
[HROWS HCOLS] = size(H);
[GROWS GCOLS] = size(G);
if ( HCOLS ~= GROWS )
	error('Matrix sizes do not fit!')
end

A = zeros(HROWS,GCOLS);
for ii = 1:HROWS
	for jj = 1:GCOLS
		for kk = 1:HCOLS
			if ((H(ii,kk) == 0)||(G(kk,jj) == 0))
				coeff = 0;
			elseif (H(ii,kk) == 1)
				coeff = G(kk,jj);
			elseif (G(kk,jj) == 1)
				coeff = H(ii,kk);
			else
				coeff = rfc6330_gfmult(H(ii,kk),G(kk,jj));
			end
			A(ii,jj) = bitxor(A(ii,jj),coeff);
		end
	end
end
*****************************/
void rfc6330_gf_mult_mat(unsigned char *Result,
						 unsigned char *H, unsigned int H_row, unsigned int H_col, 
						 unsigned char *G, unsigned int G_row, unsigned int G_col )
{
	unsigned char *p_H, *p_G;
	unsigned char Data;

	if(H_col != G_row) return;
	
	for (unsigned int row = 0; row < H_row; row++)
	{
		for (unsigned int col = 0; col < G_col; col++)
		{
			p_H = H + H_col * row;
			p_G = G + col;
			Data = 0;
			for (unsigned int k = 0; k < H_col; k++)
			{
				Data ^= rfc6330_gf_mult(*p_H, *p_G);
				p_H++;
				p_G += G_col;
			}
			*Result++ = Data;
		}
	}
}