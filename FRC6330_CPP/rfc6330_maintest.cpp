#include "rfc6330_func.h"
#include "stdlib.h"


void test_div()
{
	unsigned char op1[] = {2, 5, 251, 166, 89, 48, 111};
	unsigned char op2[] = {140, 48, 255, 105, 4, 23};

	for(int i = 0; i < sizeof(op1); i++)
	{
		int u =  op1[i];
		for (int j = 0; j < sizeof(op2); j++)
		{
			int v = op2[j];
//
//			int d = rfc6330_gf_div(u, v);
////			int d1 =  rfc6330_gf_div2(u, v);
//
//			int r = rfc6330_gf_mult(d, v);
//			int rr = rfc6330_gf_mult(v, d);
////			int r1 = rfc6330_gf_mult(d1, v);
////			int rr1 = rfc6330_gf_mult(v, d1);
//			int ss = rfc6330_gf_mult(v, 1);
		}
	}
}

void test_mat()
{
	unsigned char InvTestA[] = {1, 2, 3, 1,   0, 4, 3, 1,  0, 6, 1, 1,  3, 5, 0, 0 };
	unsigned char InvTestB[] = {0, 1, 2, 3};
	unsigned char TestA[4*4];
	unsigned char TestB[4];

	rfc6330_copy_mat(TestA, 4, InvTestA, 4, 4, 4);
//	rfc6330_gf_gauss(0, InvTestA, InvTestB, 1, 4);
	rfc6330_gf_mult_mat(TestB, TestA, 4, 4, InvTestB, 4, 1);
}


int main()
{
	rfc6330_params_t Param;


	unsigned char Source[] = {1,2,3,4,5,6,7,8,9,10};
	unsigned char D[] = {0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,
		1,2,3,4,5,6,7,8,9,10};

	unsigned char Dest[27];

	unsigned char *A = (unsigned char *) malloc(27 * 27);

	unsigned int ISIs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	unsigned int ESIs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

	rfc6330_encode_block(Dest, 10, Source, 2, 8);
	rfc6330_decode_block(D, 8, Dest, 2, ESIs, 10);


	rfc6330_parameters(10, &Param);
	rfc6330_A(A, &Param, ISIs, 10);
	rfc6330_gf_gauss(0, &Param, A, D, 1, 27);

	rfc6330_A(A, &Param, ISIs, 10);
	rfc6330_gf_mult_mat(Dest, A, 27, 27, D, 27, 1); 
	free(A);

}