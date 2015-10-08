/****************************
function val = rfc6330_rand( x, ii, m )
	%
	% Implements the random number generator as described in the RFC XXXX
	% Section 5.3.5.1. Random Generator
	%
	% Input:
	%   x - some non-negative integer
	%	ii - some non-negative integer less than 256
	%   m - some positive integer
	% Output:
	%	val - some integer in [0 m-1]
	%
	% Tables V0 - V3 are used as described in Section 5.5. are 4 arrays of 32 bit integers Random Numbers
	%
	%
**************************/
#include "RFC6330func.h"

const uint32_t V0[] =
{
		251291136, 3952231631, 3370958628, 4070167936,  123631495, 
			3351110283, 3218676425, 2011642291,  774603218, 2402805061, 
			1004366930, 1843948209,  428891132, 3746331984, 1591258008, 
			3067016507, 1433388735,  504005498, 2032657933, 3419319784, 
			2805686246, 3102436986, 3808671154, 2501582075, 3978944421, 
			246043949, 4016898363,  649743608, 1974987508, 2651273766, 
			2357956801,  689605112,  715807172, 2722736134,  191939188, 
			3535520147, 3277019569, 1470435941, 3763101702, 3232409631, 
			122701163, 3920852693,  782246947,  372121310, 2995604341, 
			2045698575, 2332962102, 4005368743,  218596347, 3415381967, 
			4207612806,  861117671, 3676575285, 2581671944, 3312220480, 
			681232419,  307306866, 4112503940, 1158111502,  709227802, 
			2724140433, 4201101115, 4215970289, 4048876515, 3031661061, 
			1909085522,  510985033, 1361682810,  129243379, 3142379587, 
			2569842483, 3033268270, 1658118006,  932109358, 1982290045, 
			2983082771, 3007670818, 3448104768,  683749698,  778296777, 
			1399125101, 1939403708, 1692176003, 3868299200, 1422476658, 
			593093658, 1878973865, 2526292949, 1591602827, 3986158854, 
			3964389521, 2695031039, 1942050155,  424618399, 1347204291, 
			2669179716, 2434425874, 2540801947, 1384069776, 4123580443, 
			1523670218, 2708475297, 1046771089, 2229796016, 1255426612, 
			4213663089, 1521339547, 3041843489,  420130494,   10677091, 
			515623176, 3457502702, 2115821274, 2720124766, 3242576090, 
			854310108,  425973987,  325832382, 1796851292, 2462744411, 
			1976681690, 1408671665, 1228817808, 3917210003,  263976645, 
			2593736473, 2471651269, 4291353919,  650792940, 1191583883, 
			3046561335, 2466530435, 2545983082,  969168436, 2019348792, 
			2268075521, 1169345068, 3250240009, 3963499681, 2560755113, 
			911182396,  760842409, 3569308693, 2687243553,  381854665, 
			2613828404, 2761078866, 1456668111,  883760091, 3294951678, 
			1604598575, 1985308198, 1014570543, 2724959607, 3062518035, 
			3115293053,  138853680, 4160398285, 3322241130, 2068983570, 
			2247491078, 3669524410, 1575146607,  828029864, 3732001371, 
			3422026452, 3370954177, 4006626915,  543812220, 1243116171, 
			3928372514, 2791443445, 4081325272, 2280435605,  885616073, 
			616452097, 3188863436, 2780382310, 2340014831, 1208439576, 
			258356309, 3837963200, 2075009450, 3214181212, 3303882142, 
			880813252, 1355575717,  207231484, 2420803184,  358923368, 
			1617557768, 3272161958, 1771154147, 2842106362, 1751209208, 
			1421030790,  658316681,  194065839, 3241510581,   38625260, 
			301875395, 4176141739,  297312930, 2137802113, 1502984205, 
			3669376622, 3728477036,  234652930, 2213589897, 2734638932, 
			1129721478, 3187422815, 2859178611, 3284308411, 3819792700, 
			3557526733,  451874476, 1740576081, 3592838701, 1709429513, 
			3702918379, 3533351328, 1641660745,  179350258, 2380520112, 
			3936163904, 3685256204, 3156252216, 1854258901, 2861641019, 
			3176611298,  834787554,  331353807,  517858103, 3010168884, 
			4012642001, 2217188075, 3756943137, 3077882590, 2054995199, 
			3081443129, 3895398812, 1141097543, 2376261053, 2626898255, 
			2554703076,  401233789, 1460049922,  678083952, 1064990737, 
			940909784, 1673396780,  528881783, 1712547446, 3629685652, 
			1358307511
};

const uint32_t V1[] =
{
	807385413, 2043073223, 3336749796, 1302105833, 2278607931, 
		541015020, 1684564270,  372709334, 3508252125, 1768346005, 
		1270451292, 2603029534, 2049387273, 3891424859, 2152948345, 
		4114760273,  915180310, 3754787998,  700503826, 2131559305, 
		1308908630,  224437350, 4065424007, 3638665944, 1679385496, 
		3431345226, 1779595665, 3068494238, 1424062773, 1033448464, 
		4050396853, 3302235057,  420600373, 2868446243,  311689386, 
		259047959, 4057180909, 1575367248, 4151214153,  110249784, 
		3006865921, 4293710613, 3501256572,  998007483,  499288295, 
		1205710710, 2997199489,  640417429, 3044194711,  486690751, 
		2686640734, 2394526209, 2521660077,   49993987, 3843885867, 
		4201106668,  415906198,   19296841, 2402488407, 2137119134, 
		1744097284,  579965637, 2037662632,  852173610, 2681403713, 
		1047144830, 2982173936,  910285038, 4187576520, 2589870048, 
		989448887, 3292758024,  506322719,  176010738, 1865471968, 
		2619324712,  564829442, 1996870325,  339697593, 4071072948, 
		3618966336, 2111320126, 1093955153,  957978696,  892010560, 
		1854601078, 1873407527, 2498544695, 2694156259, 1927339682, 
		1650555729,  183933047, 3061444337, 2067387204,  228962564, 
		3904109414, 1595995433, 1780701372, 2463145963,  307281463, 
		3237929991, 3852995239, 2398693510, 3754138664,  522074127, 
		146352474, 4104915256, 3029415884, 3545667983,  332038910, 
		976628269, 3123492423, 3041418372, 2258059298, 2139377204, 
		3243642973, 3226247917, 3674004636, 2698992189, 3453843574, 
		1963216666, 3509855005, 2358481858,  747331248, 1957348676, 
		1097574450, 2435697214, 3870972145, 1888833893, 2914085525, 
		4161315584, 1273113343, 3269644828, 3681293816,  412536684, 
		1156034077, 3823026442, 1066971017, 3598330293, 1979273937, 
		2079029895, 1195045909, 1071986421, 2712821515, 3377754595, 
		2184151095,  750918864, 2585729879, 4249895712, 1832579367, 
		1192240192,  946734366,   31230688, 3174399083, 3549375728, 
		1642430184, 1904857554,  861877404, 3277825584, 4267074718, 
		3122860549,  666423581,  644189126,  226475395,  307789415, 
		1196105631, 3191691839,  782852669, 1608507813, 1847685900, 
		4069766876, 3931548641, 2526471011,  766865139, 2115084288, 
		4259411376, 3323683436,  568512177, 3736601419, 1800276898, 
		4012458395,    1823982,   27980198, 2023839966,  869505096, 
		431161506, 1024804023, 1853869307, 3393537983, 1500703614, 
		3019471560, 1351086955, 3096933631, 3034634988, 2544598006, 
		1230942551, 3362230798,  159984793,  491590373, 3993872886, 
		3681855622,  903593547, 3535062472, 1799803217,  772984149, 
		895863112, 1899036275, 4187322100,  101856048,  234650315, 
		3183125617, 3190039692,  525584357, 1286834489,  455810374, 
		1869181575,  922673938, 3877430102, 3422391938, 1414347295, 
		1971054608, 3061798054,  830555096, 2822905141,  167033190, 
		1079139428, 4210126723, 3593797804,  429192890,  372093950, 
		1779187770, 3312189287,  204349348,  452421568, 2800540462, 
		3733109044, 1235082423, 1765319556, 3174729780, 3762994475, 
		3171962488,  442160826,  198349622,   45942637, 1324086311, 
		2901868599,  678860040, 3812229107,   19936821, 1119590141, 
		3640121682, 3545931032, 2102949142, 2828208598, 3603378023, 
		4135048896
};

const uint32_t V2[] =
{
	1629829892,  282540176, 2794583710,  496504798, 2990494426, 
		3070701851, 2575963183, 4094823972, 2775723650, 4079480416, 
		176028725, 2246241423, 3732217647, 2196843075, 1306949278, 
		4170992780, 4039345809, 3209664269, 3387499533,  293063229, 
		3660290503, 2648440860, 2531406539, 3537879412,  773374739, 
		4184691853, 1804207821, 3347126643, 3479377103, 3970515774, 
		1891731298, 2368003842, 3537588307, 2969158410, 4230745262, 
		831906319, 2935838131,  264029468,  120852739, 3200326460, 
		355445271, 2296305141, 1566296040, 1760127056,   20073893, 
		3427103620, 2866979760, 2359075957, 2025314291, 1725696734, 
		3346087406, 2690756527,   99815156, 4248519977, 2253762642, 
		3274144518,  598024568, 3299672435,  556579346, 4121041856, 
		2896948975, 3620123492,  918453629, 3249461198, 2231414958, 
		3803272287, 3657597946, 2588911389,  242262274, 1725007475, 
		2026427718,   46776484, 2873281403, 2919275846, 3177933051, 
		1918859160, 2517854537, 1857818511, 3234262050,  479353687, 
		200201308, 2801945841, 1621715769,  483977159,  423502325, 
		3689396064, 1850168397, 3359959416, 3459831930,  841488699, 
		3570506095,  930267420, 1564520841, 2505122797,  593824107, 
		1116572080,  819179184, 3139123629, 1414339336, 1076360795, 
		512403845,  177759256, 1701060666, 2239736419,  515179302, 
		2935012727, 3821357612, 1376520851, 2700745271,  966853647, 
		1041862223,  715860553,  171592961, 1607044257, 1227236688, 
		3647136358, 1417559141, 4087067551, 2241705880, 4194136288, 
		1439041934,   20464430,  119668151, 2021257232, 2551262694, 
		1381539058, 4082839035,  498179069,  311508499, 3580908637, 
		2889149671,  142719814, 1232184754, 3356662582, 2973775623, 
		1469897084, 1728205304, 1415793613,   50111003, 3133413359, 
		4074115275, 2710540611, 2700083070, 2457757663, 2612845330, 
		3775943755, 2469309260, 2560142753, 3020996369, 1691667711, 
		4219602776, 1687672168, 1017921622, 2307642321,  368711460, 
		3282925988,  213208029, 4150757489, 3443211944, 2846101972, 
		4106826684, 4272438675, 2199416468, 3710621281,  497564971, 
		285138276,  765042313,  916220877, 3402623607, 2768784621, 
		1722849097, 3386397442,  487920061, 3569027007, 3424544196, 
		217781973, 2356938519, 3252429414,  145109750, 2692588106, 
		2454747135, 1299493354, 4120241887, 2088917094,  932304329, 
		1442609203,  952586974, 3509186750,  753369054,  854421006, 
		1954046388, 2708927882, 4047539230, 3048925996, 1667505809, 
		805166441, 1182069088, 4265546268, 4215029527, 3374748959, 
		373532666, 2454243090, 2371530493, 3651087521, 2619878153, 
		1651809518, 1553646893, 1227452842,  703887512, 3696674163, 
		2552507603, 2635912901,  895130484, 3287782244, 3098973502, 
		990078774, 3780326506, 2290845203,   41729428, 1949580860, 
		2283959805, 1036946170, 1694887523,    4880696,  466000198, 
		2765355283, 3318686998, 1266458025, 3919578154, 3545413527, 
		2627009988, 3744680394, 1696890173, 3250684705, 4142417708, 
		915739411, 3308488877, 1289361460, 2942552331, 1169105979, 
		3342228712,  698560958, 1356041230, 2401944293,  107705232, 
		3701895363,  903928723, 3646581385,  844950914, 1944371367, 
		3863894844, 2946773319, 1972431613, 1706989237,   29917467, 
		3497665928
};

const uint32_t V3[] =
{
	1191369816,  744902811, 2539772235, 3213192037, 3286061266, 
		1200571165, 2463281260,  754888894,  714651270, 1968220972, 
		3628497775, 1277626456, 1493398934,  364289757, 2055487592, 
		3913468088, 2930259465,  902504567, 3967050355, 2056499403, 
		692132390,  186386657,  832834706,  859795816, 1283120926, 
		2253183716, 3003475205, 1755803552, 2239315142, 4271056352, 
		2184848469,  769228092, 1249230754, 1193269205, 2660094102, 
		642979613, 1687087994, 2726106182,  446402913, 4122186606, 
		3771347282,   37667136,  192775425, 3578702187, 1952659096, 
		3989584400, 3069013882, 2900516158, 4045316336, 3057163251, 
		1702104819, 4116613420, 3575472384, 2674023117, 1409126723, 
		3215095429, 1430726429, 2544497368, 1029565676, 1855801827, 
		4262184627, 1854326881, 2906728593, 3277836557, 2787697002, 
		2787333385, 3105430738, 2477073192,  748038573, 1088396515, 
		1611204853,  201964005, 3745818380, 3654683549, 3816120877, 
		3915783622, 2563198722, 1181149055,   33158084, 3723047845, 
		3790270906, 3832415204, 2959617497,  372900708, 1286738499, 
		1932439099, 3677748309, 2454711182, 2757856469, 2134027055, 
		2780052465, 3190347618, 3758510138, 3626329451, 1120743107, 
		1623585693, 1389834102, 2719230375, 3038609003,  462617590, 
		260254189, 3706349764, 2556762744, 2874272296, 2502399286, 
		4216263978, 2683431180, 2168560535, 3561507175,  668095726, 
		680412330, 3726693946, 4180630637, 3335170953,  942140968, 
		2711851085, 2059233412, 4265696278, 3204373534,  232855056, 
		881788313, 2258252172, 2043595984, 3758795150, 3615341325, 
		2138837681, 1351208537, 2923692473, 3402482785, 2105383425, 
		2346772751,  499245323, 3417846006, 2366116814, 2543090583, 
		1828551634, 3148696244, 3853884867, 1364737681, 2200687771, 
		2689775688,  232720625, 4071657318, 2671968983, 3531415031, 
		1212852141,  867923311, 3740109711, 1923146533, 3237071777, 
		3100729255, 3247856816,  906742566, 4047640575, 4007211572, 
		3495700105, 1171285262, 2835682655, 1634301229, 3115169925, 
		2289874706, 2252450179,  944880097,  371933491, 1649074501, 
		2208617414, 2524305981, 2496569844, 2667037160, 1257550794, 
		3399219045, 3194894295, 1643249887,  342911473,  891025733, 
		3146861835, 3789181526,  938847812, 1854580183, 2112653794, 
		2960702988, 1238603378, 2205280635, 1666784014, 2520274614, 
		3355493726, 2310872278, 3153920489, 2745882591, 1200203158, 
		3033612415, 2311650167, 1048129133, 4206710184, 4209176741, 
		2640950279, 2096382177, 4116899089, 3631017851, 4104488173, 
		1857650503, 3801102932,  445806934, 3055654640,  897898279, 
		3234007399, 1325494930, 2982247189, 1619020475, 2720040856, 
		885096170, 3485255499, 2983202469, 3891011124,  546522756, 
		1524439205, 2644317889, 2170076800, 2969618716,  961183518, 
		1081831074, 1037015347, 3289016286, 2331748669,  620887395, 
		303042654, 3990027945, 1562756376, 3413341792, 2059647769, 
		2823844432,  674595301, 2457639984, 4076754716, 2447737904, 
		1583323324,  625627134, 3076006391,  345777990, 1684954145, 
		879227329, 3436182180, 1522273219, 3802543817, 1456017040, 
		1897819847, 2970081129, 1382576028, 3820044861, 1044428167, 
		612252599, 3340478395, 2150613904, 3397625662, 3573635640, 
		3432275192 
};

uint32_t rfc6330_rand(uint32_t x, unsigned int ii, uint32_t m)
{
	unsigned int x0, x1, x2, x3;
	unsigned int ret;
	x0 = (x + ii) & 0x000000FF;
	x1 = ((x >> 8) + ii) & 0x000000FF;
	x2 = ((x >> 16) + ii) & 0x000000FF;
	x3 = ((x >> 24) + ii) & 0x000000FF;
	ret = ( V0[x0] ^ 
                V1[x1] ^ 
                V2[x2] ^ 
                V3[x3] ) % m;
	return ret;
}
