/*************************************
function val = rfc6330_deg( v, W )
	%
	% Implements the degree generator as described in the RFC6330
	% Section 5.3.5.2. Degree Generator
	%
	% Input:
	%   v - some non-negative integer such that 0<=v<=1048576
	%   W - W is derived from K' as described in Section 5.3.3.3.
	% Output:
	% val - Given v find index d in table1 such that f[d-1] <= v < f[d]
	%       % and set Deg[v] = d obsolete since November 2009
	%       and set Deg[v] = min(d, W-2)
	%

**************************/
#include "rfc6330_func.h"

uint32_t const dist_table[] = {
	0,
	5243,
	529531,
	704294,
	791675,
	844104,
	879057,
	904023,
	922747,
	937311,
	948962,
	958494,
	966438,
	973160,
	978921,
	983914,
	988283,
	992138,
	995565,
	998631,
	1001391,
	1003887,
	1006157,
	1008229,
	1010129,
	1011876,
	1013490,
	1014983,
	1016370,
	1017662,
	1048576
};

unsigned int rfc6330_deg(
	uint32_t v,
	unsigned int W)
{
	unsigned int degree = 0;
	unsigned int i = 1;
	for (; dist_table[i] <= v; i++) {}
	degree = i;
	return (degree < (W - 2)) ? degree : (W - 2); // min(degree, W-2);
}
