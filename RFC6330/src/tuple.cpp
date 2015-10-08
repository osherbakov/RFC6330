/**************************
[S H B U L W P P1] = rfc6330_parameters( K_prime );

J = Table2((find(Table2(:,1) == K_prime)),2);
A =53591 + (997*J);
if ( mod(A,2) == 0 )
	A = A + 1;  
end
B = 10267*(J+1);
y = mod((B + x*A),2^32);
v = rfc6330_rand(y, 0, 2^20);
d = rfc6330_deg(v,W);
a = 1 + rfc6330_rand(y, 1, W-1);
b = rfc6330_rand(y, 2, W);
if ( d < 4 )
d1 = 2 + rfc6330_rand(x, 3, 2);
else
d1 = 2;
end
a1 = 1 + rfc6330_rand(x, 4, P1-1);
b1 = rfc6330_rand(x, 5, P1);
***************************/

#include "RFC6330func.h"

void calc_tuple(tuple_t *tuple, params_t *param, unsigned int ISI)
{
	uint32_t J, y, v, W;
	J = param->J;
	W = param->W;
	y =  param->tupleB + ISI * param->tupleA;
	v = rfc6330_rand(y, 0, ((uint32_t)0x1L) << 20);
	
	tuple->d = degree(v, W);
	tuple->a = 1 + rfc6330_rand(y, 1, W-1);
	tuple->b = rfc6330_rand(y, 2, W);

	tuple->d1 = tuple->d < 4 ? 2 + rfc6330_rand(ISI, 3, 2) : 2;
	tuple->a1 = 1 + rfc6330_rand(ISI, 4, param->P1-1);
	tuple->b1 = rfc6330_rand(ISI, 5, param->P1);
}
