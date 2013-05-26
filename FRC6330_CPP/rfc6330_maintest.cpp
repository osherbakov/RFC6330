#include "rfc6330_func.h"
#include "stdlib.h"

int main()
{
	rfc6330_params_t Param;

	unsigned char op1[] = {2, 5, 251, 166, 89, 48, 111};
	unsigned char op2[] = {140, 48, 255, 105, 4, 23};

	unsigned char *Gamma = (unsigned char *) calloc(13*13, 1);


	unsigned char *Target = (unsigned char *) calloc(27 * 27, 1);
	unsigned int ISIs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	//for(int i = 0; i < sizeof(op1); i++)
	//{
	//	int u =  op1[i];

	//	for (int j = 0; j < sizeof(op2); j++)
	//	{
	//		int v = op2[j];

	//		int d = rfc6330_gf_div(u, v);
	//		int d1 =  rfc6330_gf_div1(u, v);

	//		int r = rfc6330_gf_mult(d, v);
	//		int rr = rfc6330_gf_mult(v, d);
	//		int r1 = rfc6330_gf_mult(d1, v);
	//		int rr1 = rfc6330_gf_mult(v, d1);

	//	}
	//}

	rfc6330_parameters(10, &Param);
	rfc6330_A(Target, &Param, ISIs, 10);
	rfc6330_gf_invert(0, Target, 

}