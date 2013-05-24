#include "rfc6330_func.h"
#include "stdlib.h"

int main()
{
	rfc6330_params_t Param;

	unsigned char *Gamma = (unsigned char *) calloc(13*13, 1);


	unsigned char *Target = (unsigned char *) calloc(27 * 27, 1);
	unsigned int ISIs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	rfc6330_parameters(10, &Param);
	rfc6330_A(Target, &Param, ISIs, 10);

}