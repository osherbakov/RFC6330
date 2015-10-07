#include "func.h"

void mat_eye(unsigned char *Target, unsigned int nStride, unsigned int Size)
{
	unsigned int i;
	unsigned int Step = nStride + 1;
	mat_zero(Target, nStride, Size, Size);
	for(i = 0; i < Size; i++, Target += Step)
	{
		*Target = 1;
	}
}

void mat_zero(unsigned char *Target, unsigned int nStride, unsigned int numRows, unsigned int numCols)
{
	unsigned char *Zero;
	unsigned int i, j;

	for(i = 0; i < numRows; i++)
	{
		Zero = Target + i * nStride;
		for(j = 0; j < numCols; j++)
		{
			*Zero++ = 0;
		}
	}
}

void mat_mult(unsigned char *Result,
						 unsigned char *H, unsigned int H_row, unsigned int H_col, 
						 unsigned char *G, unsigned int G_row, unsigned int G_col )
{
	unsigned char *p_H, *p_G;
	unsigned char Data;
	unsigned int row, col, k;

	
	if(H_col != G_row) return;

	for (row = 0; row < H_row; row++)
	{
		for (col = 0; col < G_col; col++)
		{
			p_H = H + H_col * row;
			p_G = G + col;
			Data = 0;
			for (k = 0; k < H_col; k++)
			{
				Data += (*p_H * *p_G);
				p_H++;
				p_G += G_col;
			}
			*Result++ = Data;
		}
	}
}

void mat_copy(unsigned char *Result, unsigned int nStride,
						 unsigned char *H, unsigned int H_stride,
						 unsigned int Rows, unsigned int Cols)
{
	unsigned char *p_Result;
	unsigned char *p_H;
	unsigned int i, j;
	for (i = 0; i < Rows; i++)
	{
		p_Result = Result + i * nStride;
		p_H = H + i * H_stride;
		for (j = 0; j < Cols; j++)
		{
			*p_Result++ = *p_H++;
		}
	}
}

void vec_copy(unsigned char *Result, unsigned char *Src, unsigned int Size)
{
	unsigned int i;
	for(i = 0; i < Size; i++)
	{
		*Result++ = *Src++;
	}
}

void vec_xor(unsigned char *Result, unsigned char *v1, unsigned char *v2, unsigned int Size)
{
	unsigned int i;
	for(i = 0; i < Size; i++)
	{
		*Result = *v1 ^ *v2;
		Result++; v1++; v2++;
	}
}

void vec_swap(unsigned char *v1, unsigned char *v2, unsigned int Size)
{
	unsigned char tmp;
	unsigned int i;

	for(i = 0; i < Size; i++)
	{
		tmp = *v1;
		*v1 = *v2;
		*v2 = tmp;
		v1++; v2++;
	}
}