/*********************
function K_prime = rfc6330_K_prime( K )
	%
	% Calculates K_prime for zero padding given the number of source symbols
	%
	% Input:
	%        K - number of source symbols
	% Output:
%  K_prime - number of symbols of the extended source block
%
*******************/

#include "rfc6330_func.h"

unsigned int K_prime_table[] = 
{
	10,12,18, 20, 26, 30, 32, 36, 42, 46, 48, 49, 55,
	60,62, 69,75,84, 88,  91,  95, 97,101, 114,119, 125, 127,138,140, 149,153,160,
	166,168, 179,181, 185,187,200, 213, 217, 225,236, 242,248,257, 263,269, 
	280,295,301,305,324, 337,341,347, 355,362, 368,372,380,385,393, 405,418,428,434,447,
	453, 466, 478, 486,491,497, 511, 526, 532, 542,549,557,563,573, 580,588,594,600,606, 619,633, 
	640,648, 666,675,685,693,703,718,728,736, 747,759,778,792,802, 811,821,835, 845,860,870, 891,903, 
	913,926,938,950,963,977,989,1002,1020,1032,1050,1074,1085,1099,1111,1136,1152,1169,1183,1205,1220, 
	1236,1255,1269, 1285,1306,1347, 1361, 1389,1404,1420, 1436,1461,1477, 1502,1522, 1539,1561,1579, 1600, 
	1616, 1649, 1698,1716,1734,1759, 1777,1800,1824,1844, 1863,1887,1906, 1926, 1954,1979,2005,2040,2070, 
	2103,2125,2152, 2195,2217,2247,2278,2315,2339, 2367,2392,2416,2447, 2473,2502,2528,2565,2601,2640,2668, 
	2701,2737,2772, 2802,2831,2875,2906,2938,2979,3015, 3056, 3101,3151,3186,3224,3265, 3299,3344,3387,3423,
	3466, 3502,3539, 3579,3616,3658,3697,3751,3792,3840, 3883,3924, 3970,4015,4069, 4112,4165,4207,4252,4318, 
	4365,4418,4468, 4513,4567,4626,4681,4731,4780,4838,4901,4954,5008,5063,5116,5172,5225,5279,5334,5391,5449,
	5506,5566,5637,5763,5823,5896,5975, 6039,6102,6169, 6233,6296,6363,6427,6518,6589,6655,6730, 6799,6878,6956,7033,7108, 
	7185, 7281,7360,7445,7520,7596,7675,7770,7855,7935,8030,8111,8194,8290,8377,8474,8559,8654,8744,8837,8928,9019,9111,9206, 
	9303,9400,9497,9601,9813,9916,10017,10120,10241,10351,10458,10567,10676,10787,10899,11015,11130,11245,11358,11475,11590,11711, 
	11829,11956,12087,12208,12333,12460,12593,12726,12857,13002,13143,13284,13417,13558,13695,13833,13974,14115,14272, 14415,14560, 
	14713,14862,15011,15170,15325,15496,15651,15808,15977, 16161,16336,16505,16674,16851,17024,17195,17376,17559,17742,17929,18116, 
	18309,18503,18694,18909,19126,19325,19539,19740,19939,20152,20355,20564,20778,20988,21199,21412,21629,21852,22073,22301,22536,
	22779,23010,23252,23491,23730,23971,24215,24476,24721,24976,25230, 25493,25756,26022,26291,26566,26838,27111,27392,27682,27959,
	28248,28548, 28845,29138,29434,29731,30037,30346,30654,30974,31285,31605,31948,32272,32601,32932,33282,33623,33961,34302,34654,
	35031,35395,35750,36112, 36479, 36849,37227,37606,37992,38385,38787,39176,39576,39980,40398,40816,41226,41641,42067,42490,42916, 
	43388, 43840,44279, 44729,45183,45638,46104,46574,47047,47523,48007, 48489, 48976, 49470,49978,50511,51017,51530,52062,52586,53114, 
	53650,54188,54735, 55289, 55843,56403
};

unsigned int Coding_table[] = 
{
// K' J(K') S(K') H(K') W(K') P1(K')
10,254,7,10,17,11,
12,630,7,10,19,11,
18,682,11,10,29,11,
20,293,11,10,31,11,
26,80,11,10,37,11,
30,566,11,10,41,11,
32,860,11,10,43,11,
36,267,11,10,47,11,
42,822,11,10,53,11,
46,506,13,10,59,11,
48,589,13,10,61,11,
49,87,13,10,61,11,
55,520,13,10,67,11,
60,159,13,10,71,13,
62,235,13,10,73,13,
69,157,13,10,79,13,
75,502,17,10,89,13,
84,334,17,10,97,17,
88,583,17,10,101,17,
91,66,17,10,103,17,
95,352,17,10,107,17,
97,365,17,10,109,17,
101,562,17,10,113,17,
114,5,19,10,127,17,
119,603,19,10,131,17,
125,721,19,10,137,17,
127,28,19,10,139,17,
138,660,19,10,149,19,
140,829,19,10,151,19,
149,900,23,10,163,19,
153,930,23,10,167,19,
160,814,23,10,173,23,
166,661,23,10,179,23,
168,693,23,10,181,23,
179,780,23,10,191,23,
181,605,23,10,193,23,
185,551,23,10,197,23,
187,777,23,10,199,23,
200,491,23,10,211,23,
213,396,23,10,223,23,
217,764,29,10,233,23,
225,843,29,10,241,23,
236,646,29,10,251,29,
242,557,29,10,257,29,
248,608,29,10,263,29,
257,265,29,10,271,29,
263,505,29,10,277,29,
269,722,29,10,283,29,
280,263,29,10,293,29,
295,999,29,10,307,29,
301,874,29,10,313,29,
305,160,29,10,317,29,
324,575,31,10,337,29,
337,210,31,10,349,29,
341,513,31,10,353,29,
347,503,31,10,359,29,
355,558,31,10,367,29,
362,932,31,10,373,31,
368,404,31,10,379,31,
372,520,37,10,389,31,
380,846,37,10,397,31,
385,485,37,10,401,31,
393,728,37,10,409,31,
405,554,37,10,421,31,
418,471,37,10,433,37,
428,641,37,10,443,37,
434,732,37,10,449,37,
447,193,37,10,461,37,
453,934,37,10,467,37,
466,864,37,10,479,37,
478,790,37,10,491,37,
486,912,37,10,499,37,
491,617,37,10,503,37,
497,587,37,10,509,37,
511,800,37,10,523,37,
526,923,41,10,541,37,
532,998,41,10,547,37,
542,92,41,10,557,37,
549,497,41,10,563,37,
557,559,41,10,571,37,
563,667,41,10,577,37,
573,912,41,10,587,37,
580,262,41,10,593,41,
588,152,41,10,601,41,
594,526,41,10,607,41,
600,268,41,10,613,41,
606,212,41,10,619,41,
619,45,41,10,631,41,
633,898,43,10,647,41,
640,527,43,10,653,41,
648,558,43,10,661,41,
666,460,47,10,683,41,
675,5,47,10,691,41,
685,895,47,10,701,41,
693,996,47,10,709,41,
703,282,47,10,719,41,
718,513,47,10,733,43,
728,865,47,10,743,43,
736,870,47,10,751,43,
747,239,47,10,761,43,
759,452,47,10,773,43,
778,862,53,10,797,47,
792,852,53,10,811,47,
802,643,53,10,821,47,
811,543,53,10,829,47,
821,447,53,10,839,47,
835,321,53,10,853,47,
845,287,53,10,863,47,
860,12,53,10,877,47,
870,251,53,10,887,47,
891,30,53,10,907,47,
903,621,53,10,919,47,
913,555,53,10,929,47,
926,127,53,10,941,53,
938,400,53,10,953,53,
950,91,59,10,971,53,
963,916,59,10,983,53,
977,935,59,10,997,53,
989,691,59,10,1009,53,
1002,299,59,10,1021,53,
1020,282,59,10,1039,53,
1032,824,59,10,1051,53,
1050,536,59,11,1069,53,
1074,596,59,11,1093,53,
1085,28,59,11,1103,53,
1099,947,59,11,1117,53,
1111,162,59,11,1129,53,
1136,536,59,11,1153,53,
1152,1000,61,11,1171,53,
1169,251,61,11,1187,59,
1183,673,61,11,1201,59,
1205,559,61,11,1223,59,
1220,923,61,11,1237,59,
1236,81,67,11,1259,59,
1255,478,67,11,1277,59,
1269,198,67,11,1291,59,
1285,137,67,11,1307,59,
1306,75,67,11,1327,59,
1347,29,67,11,1367,59,
1361,231,67,11,1381,59,
1389,532,67,11,1409,59,
1404,58,67,11,1423,59,
1420,60,67,11,1439,59,
1436,964,71,11,1459,59,
1461,624,71,11,1483,61,
1477,502,71,11,1499,61,
1502,636,71,11,1523,61,
1522,986,71,11,1543,61,
1539,950,71,11,1559,67,
1561,735,73,11,1583,67,
1579,866,73,11,1601,67,
1600,203,73,11,1621,67,
1616,83,73,11,1637,67,
1649,14,73,11,1669,67,
1673,522,79,11,1699,67,
1698,226,79,11,1723,67,
1716,282,79,11,1741,67,
1734,88,79,11,1759,67,
1759,636,79,11,1783,67,
1777,860,79,11,1801,67,
1800,324,79,11,1823,67,
1824,424,79,11,1847,67,
1844,999,79,11,1867,67,
1863,682,83,11,1889,71,
1887,814,83,11,1913,71,
1906,979,83,11,1931,71,
1926,538,83,11,1951,71,
1954,278,83,11,1979,71,
1979,580,83,11,2003,71,
2005,773,83,11,2029,71,
2040,911,89,11,2069,71,
2070,506,89,11,2099,71,
2103,628,89,11,2131,73,
2125,282,89,11,2153,73,
2152,309,89,11,2179,73,
2195,858,89,11,2221,79,
2217,442,89,11,2243,79,
2247,654,89,11,2273,79,
2278,82,97,11,2311,79,
2315,428,97,11,2347,79,
2339,442,97,11,2371,79,
2367,283,97,11,2399,79,
2392,538,97,11,2423,79,
2416,189,97,11,2447,79,
2447,438,97,11,2477,79,
2473,912,97,11,2503,79,
2502,1,97,11,2531,79,
2528,167,97,11,2557,79,
2565,272,97,11,2593,83,
2601,209,101,11,2633,83,
2640,927,101,11,2671,83,
2668,386,101,11,2699,83,
2701,653,101,11,2731,83,
2737,669,101,11,2767,83,
2772,431,101,11,2801,83,
2802,793,103,11,2833,83,
2831,588,103,11,2861,89,
2875,777,107,11,2909,89,
2906,939,107,11,2939,89,
2938,864,107,11,2971,89,
2979,627,107,11,3011,89,
3015,265,109,11,3049,89,
3056,976,109,11,3089,89,
3101,988,113,11,3137,89,
3151,507,113,11,3187,89,
3186,640,113,11,3221,89,
3224,15,113,11,3259,89,
3265,667,113,11,3299,97,
3299,24,127,11,3347,97,
3344,877,127,11,3391,97,
3387,240,127,11,3433,97,
3423,720,127,11,3469,97,
3466,93,127,11,3511,97,
3502,919,127,11,3547,97,
3539,635,127,11,3583,97,
3579,174,127,11,3623,97,
3616,647,127,11,3659,97,
3658,820,127,11,3701,97,
3697,56,127,11,3739,97,
3751,485,127,11,3793,97,
3792,210,127,11,3833,97,
3840,124,127,11,3881,97,
3883,546,127,11,3923,101,
3924,954,131,11,3967,101,
3970,262,131,11,4013,101,
4015,927,131,11,4057,101,
4069,957,131,11,4111,101,
4112,726,137,11,4159,101,
4165,583,137,11,4211,103,
4207,782,137,11,4253,103,
4252,37,137,11,4297,103,
4318,758,137,11,4363,103,
4365,777,137,11,4409,107,
4418,104,139,11,4463,107,
4468,476,139,11,4513,107,
4513,113,149,11,4567,107,
4567,313,149,11,4621,107,
4626,102,149,11,4679,107,
4681,501,149,11,4733,109,
4731,332,149,11,4783,109,
4780,786,149,11,4831,109,
4838,99,149,11,4889,109,
4901,658,149,11,4951,113,
4954,794,149,11,5003,113,
5008,37,151,11,5059,113,
5063,471,151,11,5113,113,
5116,94,157,11,5171,113,
5172,873,157,11,5227,113,
5225,918,157,11,5279,127,
5279,945,157,11,5333,127,
5334,211,157,11,5387,127,
5391,341,157,11,5443,127,
5449,11,163,11,5507,127,
5506,578,163,11,5563,127,
5566,494,163,11,5623,127,
5637,694,163,11,5693,127,
5694,252,163,11,5749,127,
5763,451,167,11,5821,127,
5823,83,167,11,5881,127,
5896,689,167,11,5953,127,
5975,488,173,11,6037,127,
6039,214,173,11,6101,127,
6102,17,173,11,6163,127,
6169,469,173,11,6229,127,
6233,263,179,11,6299,127,
6296,309,179,11,6361,127,
6363,984,179,11,6427,127,
6427,123,179,11,6491,127,
6518,360,179,11,6581,127,
6589,863,181,11,6653,131,
6655,122,181,11,6719,131,
6730,522,191,11,6803,131,
6799,539,191,11,6871,131,
6878,181,191,11,6949,131,
6956,64,191,11,7027,131,
7033,387,191,11,7103,137,
7108,967,191,11,7177,137,
7185,843,191,11,7253,137,
7281,999,193,11,7351,137,
7360,76,197,11,7433,137,
7445,142,197,11,7517,137,
7520,599,197,11,7591,137,
7596,576,199,11,7669,137,
7675,176,211,11,7759,139,
7770,392,211,11,7853,139,
7855,332,211,11,7937,149,
7935,291,211,11,8017,149,
8030,913,211,11,8111,149,
8111,608,211,11,8191,149,
8194,212,211,11,8273,149,
8290,696,211,11,8369,149,
8377,931,223,11,8467,149,
8474,326,223,11,8563,149,
8559,228,223,11,8647,149,
8654,706,223,11,8741,149,
8744,144,223,11,8831,149,
8837,83,223,11,8923,149,
8928,743,223,11,9013,149,
9019,187,223,11,9103,151,
9111,654,227,11,9199,151,
9206,359,227,11,9293,151,
9303,493,229,11,9391,157,
9400,369,233,11,9491,157,
9497,981,233,11,9587,157,
9601,276,239,11,9697,157,
9708,647,239,11,9803,157,
9813,389,239,11,9907,157,
9916,80,239,11,10009,157,
10017,396,241,11,10111,163,
10120,580,251,11,10223,163,
10241,873,251,11,10343,163,
10351,15,251,11,10453,163,
10458,976,251,11,10559,163,
10567,584,251,11,10667,163,
10676,267,257,11,10781,163,
10787,876,257,11,10891,167,
10899,642,257,12,11003,167,
11015,794,257,12,11119,167,
11130,78,263,12,11239,167,
11245,736,263,12,11353,167,
11358,882,269,12,11471,173,
11475,251,269,12,11587,173,
11590,434,269,12,11701,173,
11711,204,269,12,11821,173,
11829,256,271,12,11941,173,
11956,106,277,12,12073,173,
12087,375,277,12,12203,173,
12208,148,277,12,12323,179,
12333,496,281,12,12451,179,
12460,88,281,12,12577,179,
12593,826,293,12,12721,179,
12726,71,293,12,12853,179,
12857,925,293,12,12983,179,
13002,760,293,12,13127,181,
13143,130,293,12,13267,181,
13284,641,307,12,13421,191,
13417,400,307,12,13553,191,
13558,480,307,12,13693,191,
13695,76,307,12,13829,191,
13833,665,307,12,13967,191,
13974,910,307,12,14107,191,
14115,467,311,12,14251,191,
14272,964,311,12,14407,191,
14415,625,313,12,14551,191,
14560,362,317,12,14699,191,
14713,759,317,12,14851,191,
14862,728,331,12,15013,193,
15011,343,331,12,15161,193,
15170,113,331,12,15319,197,
15325,137,331,12,15473,197,
15496,308,331,12,15643,197,
15651,800,337,12,15803,197,
15808,177,337,12,15959,199,
15977,961,337,12,16127,199,
16161,958,347,12,16319,211,
16336,72,347,12,16493,211,
16505,732,347,12,16661,211,
16674,145,349,12,16831,211,
16851,577,353,12,17011,211,
17024,305,353,12,17183,211,
17195,50,359,12,17359,211,
17376,351,359,12,17539,211,
17559,175,367,12,17729,211,
17742,727,367,12,17911,211,
17929,902,367,12,18097,211,
18116,409,373,12,18289,223,
18309,776,373,12,18481,223,
18503,586,379,12,18679,223,
18694,451,379,12,18869,223,
18909,287,383,12,19087,223,
19126,246,389,12,19309,223,
19325,222,389,12,19507,223,
19539,563,397,12,19727,223,
19740,839,397,12,19927,223,
19939,897,401,12,20129,223,
20152,409,401,12,20341,227,
20355,618,409,12,20551,227,
20564,439,409,12,20759,227,
20778,95,419,13,20983,227,
20988,448,419,13,21191,229,
21199,133,419,13,21401,233,
21412,938,419,13,21613,233,
21629,423,431,13,21841,233,
21852,90,431,13,22063,233,
22073,640,431,13,22283,239,
22301,922,433,13,22511,239,
22536,250,439,13,22751,239,
22779,367,439,13,22993,239,
23010,447,443,13,23227,239,
23252,559,449,13,23473,241,
23491,121,457,13,23719,251,
23730,623,457,13,23957,251,
23971,450,457,13,24197,251,
24215,253,461,13,24443,251,
24476,106,467,13,24709,251,
24721,863,467,13,24953,251,
24976,148,479,13,25219,251,
25230,427,479,13,25471,251,
25493,138,479,13,25733,257,
25756,794,487,13,26003,257,
26022,247,487,13,26267,257,
26291,562,491,13,26539,257,
26566,53,499,13,26821,257,
26838,135,499,13,27091,263,
27111,21,503,13,27367,263,
27392,201,509,13,27653,263,
27682,169,521,13,27953,263,
27959,70,521,13,28229,269,
28248,386,521,13,28517,269,
28548,226,523,13,28817,269,
28845,3,541,13,29131,269,
29138,769,541,13,29423,269,
29434,590,541,13,29717,271,
29731,672,541,13,30013,277,
30037,713,547,13,30323,277,
30346,967,547,13,30631,277,
30654,368,557,14,30949,277,
30974,348,557,14,31267,281,
31285,119,563,14,31583,281,
31605,503,569,14,31907,281,
31948,181,571,14,32251,283,
32272,394,577,14,32579,293,
32601,189,587,14,32917,293,
32932,210,587,14,33247,293,
33282,62,593,14,33601,293,
33623,273,593,14,33941,293,
33961,554,599,14,34283,293,
34302,936,607,14,34631,293,
34654,483,607,14,34981,307,
35031,397,613,14,35363,307,
35395,241,619,14,35731,307,
35750,500,631,14,36097,307,
36112,12,631,14,36457,307,
36479,958,641,14,36833,307,
36849,524,641,14,37201,307,
37227,8,643,14,37579,307,
37606,100,653,14,37967,307,
37992,339,653,14,38351,311,
38385,804,659,14,38749,311,
38787,510,673,14,39163,311,
39176,18,673,14,39551,313,
39576,412,677,14,39953,317,
39980,394,683,14,40361,317,
40398,830,691,15,40787,317,
40816,535,701,15,41213,331,
41226,199,701,15,41621,331,
41641,27,709,15,42043,331,
42067,298,709,15,42467,331,
42490,368,719,15,42899,331,
42916,755,727,15,43331,331,
43388,379,727,15,43801,331,
43840,73,733,15,44257,331,
44279,387,739,15,44701,337,
44729,457,751,15,45161,337,
45183,761,751,15,45613,337,
45638,855,757,15,46073,337,
46104,370,769,15,46549,347,
46574,261,769,15,47017,347,
47047,299,787,15,47507,347,
47523,920,787,15,47981,347,
48007,269,787,15,48463,347,
48489,862,797,15,48953,349,
48976,349,809,15,49451,349,
49470,103,809,15,49943,353,
49978,115,821,15,50461,353,
50511,93,821,16,50993,359,
51017,982,827,16,51503,359,
51530,432,839,16,52027,359,
52062,340,853,16,52571,367,
52586,173,853,16,53093,367,
53114,421,857,16,53623,367,
53650,330,863,16,54163,367,
54188,624,877,16,54713,373,
54735,233,877,16,55259,373,
55289,362,883,16,55817,373,
55843,963,907,16,56393,373,
56403,471,907,16,56951,379
};

/***********************
function [S H B U L W P P1] = rfc6330_parameters( K_prime )
	%
	% Calculates the parameters for the construction of the matrix as
	% as described in section 5.3.3.3.
	%
	% Input:
	%  K_prime - number of symbols in the extended source block
	% Output:
	%   S - number of LDPC symbols
	%   H - number of HDPC symbols
	%   P - number of permanently inactive symbols (HDPC + set of K_prime)
	%   W - number of LT symbols (LDPC + set of K_prime)
	% 
	%
	ind = find(Table2(:,1) == K_prime);
	S = Table2(ind,3);
	H = Table2(ind,4);
	W = Table2(ind,5);

	L = K_prime + S + H;
	P = L - W;
	PRIMS   = primes(100000);
	indexes = find( PRIMS >= P );
	P1      = PRIMS(indexes(1));
	U = P - H;
	B = W - S;

*************************/

void rfc6330_parameters(unsigned int K,
						rfc6330_params_t *params)
{
	for (unsigned int i = 0; i < sizeof(K_prime_table); i++)
	{
		if (K_prime_table[i] > K)
		{
			i--;
			params->K_prime =  Coding_table[i * 6 + 0];
			params->J = Coding_table[i * 6 + 1];
			params->S = Coding_table[i * 6 + 2];
			params->H = Coding_table[i * 6 + 3];
			params->W = Coding_table[i * 6 + 4];
			params->P1 = Coding_table[i * 6 + 5];

			params->L =  params->K_prime + params->S + params->H;
			params->P = params->L - params->W;
			params->U = params->P - params->H;
			params->B = params->W - params->S;

			break;
		}
	}

}

unsigned int rfc6330_K_prime(
	unsigned int K)
{
	unsigned int K_prime = 0;
	for (unsigned int i = 0; i < sizeof(K_prime_table); i++)
	{
		if (K_prime_table[i] > K)
		{
			i--;
			K_prime = K_prime_table[i];
			break;
		}
	}
	return K_prime;
}
