#include "rfc6330_func.h"

void rfc6330_eye(unsigned char *Target, unsigned int nStride, unsigned int Size)
{
	unsigned int Step = nStride + 1;
	rfc6330_zero(Target, nStride, Size, Size);
	for(unsigned int i = 0; i < Size; i++, Target += Step)
	{
		*Target = 1;
	}
}

void rfc6330_zero(unsigned char *Target, unsigned int nStride, unsigned int numRows, unsigned int numCols)
{
	unsigned char *Zero;
	for(unsigned int i = 0; i < numRows; i++)
	{
		Zero = Target + i * nStride;
		for(unsigned int j = 0; j < numCols; j++)
		{
			*Zero++ = 0;
		}
	}
}

void rfc6330_mult_mat(unsigned char *Result,
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
				Data += (*p_H * *p_G);
				p_H++;
				p_G += G_col;
			}
			*Result++ = Data;
		}
	}
}

void rfc6330_copy_mat(unsigned char *Result, unsigned int nStride,
						 unsigned char *H, unsigned int H_stride,
						 unsigned int Rows, unsigned int Cols)
{
	unsigned char *p_Result;
	unsigned char *p_H;
	for (unsigned int i = 0; i < Rows; i++)
	{
		p_Result = Result + i * nStride;
		p_H = H + i * H_stride;
		for (unsigned int j = 0; j < Cols; j++)
		{
			*p_Result++ = *p_H++;
		}
	}
}

void rfc6330_copy_vec(unsigned char *Result, unsigned char *Src, unsigned int Size)
{
	for(unsigned int i = 0; i < Size; i++)
	{
		*Result++ = *Src++;
	}
}

void rfc6330_xor_vec(unsigned char *Result, unsigned char *v1, unsigned char *v2, unsigned int Size)
{
	for(unsigned int i = 0; i < Size; i++)
	{
		*Result = *v1 ^ *v2;
		Result++; v1++; v2++;
	}
}

void rfc6330_swap_vec(unsigned char *v1, unsigned char *v2, unsigned int Size)
{
	unsigned char tmp;
	for(unsigned int i = 0; i < Size; i++)
	{
		tmp = *v1;
		*v1 = *v2;
		*v2 = tmp;
		v1++; v2++;
	}
}