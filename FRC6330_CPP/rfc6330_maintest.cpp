#include "rfc6330_func.h"
#include "stdlib.h"

int main()
{
	rfc6330_params_t Param;

	unsigned char op1[] = {2, 5, 251, 166, 89, 48, 111};
	unsigned char op2[] = {140, 48, 255, 105, 4, 23};

	unsigned char Source[] = {1,2,3,4,5,6,7,8,9,10};
	unsigned char D[] = {0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,
		1,2,3,4,5,6,7,8,9,10};

	unsigned char Dest[27];

	unsigned char *Gamma = (unsigned char *) calloc(13*13, 1);


	unsigned char *Target = (unsigned char *) calloc(27 * 27, 1);

	unsigned int ISIs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	unsigned char InvTestA[] = {1, 2, 3, 1,   0, 4, 3, 1,  0, 6, 1, 1,  3, 5, 0, 0 };
	unsigned char InvTestB[] = {0, 1, 2, 3};

	rfc6330_gf_gauss(0, InvTestA, InvTestB, 4);

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


	rfc6330_parameters(10, &Param);
	rfc6330_A(Target, &Param, ISIs, 10);
	rfc6330_gf_gauss(0, Target, D, 27);

	rfc6330_A(Target, &Param, ISIs, 10);
	rfc6330_gf_mult_mat(Dest, Target, 27, 27, D, 27, 1); 

}