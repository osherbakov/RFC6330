#include "rfc6330_func.h"

void rfc6330_eye(unsigned char *Target, unsigned int nStride, unsigned int Size)
{
	for(unsigned int i = 0; i < Size; i++)
	{
		*Target ^= 1;
		Target += nStride + 1;
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
						 unsigned char *H, unsigned int H_row, unsigned int H_stride, 
						 unsigned char *G, unsigned int G_col, unsigned int G_stride,
						 unsigned int H_Col_G_Row)
{
	unsigned char *p_H, *p_G;
	unsigned char Data;

	for (unsigned int i = 0; i < H_row; i++)
	{
		for (unsigned int j = 0; j < G_col; j++)
		{
			p_H = H + H_stride * i;
			p_G = G + j;
			Data = 0;
			for (unsigned int k = 0; k < H_Col_G_Row; k++)
			{
				Data ^= (*p_H ^ *p_G);
				p_H++;
				p_G += G_stride;
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
