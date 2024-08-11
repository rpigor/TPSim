#pragma once

#include "Cell.hpp"

LUT invX1DelayLUT = {
    {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5},
    {0.0,0.00894,0.01073,0.01288,0.01545,0.01854,0.02225,0.0267,0.03204,0.03845,0.04614,0.05537,0.06644,0.07973,0.09568,0.11482,0.13779,0.16535,0.19842,0.23811},
    {
        {0.01116,0.03497,0.03959,0.04513,0.0517,0.05958,0.06909,0.08052,0.09416,0.11045,0.13007,0.15373,0.18208,0.21596,0.25679,0.30575,0.36443,0.43438,0.51873,0.62072},
        {0.01314,0.03732,0.04193,0.04749,0.05407,0.06201,0.07145,0.08291,0.09651,0.11292,0.13262,0.15616,0.18425,0.21812,0.25918,0.30792,0.3667,0.43706,0.52147,0.62313},
        {0.0146,0.04014,0.04478,0.05032,0.05692,0.06482,0.07431,0.08576,0.09941,0.11572,0.13547,0.159,0.18734,0.22129,0.26208,0.31077,0.36964,0.43998,0.52456,0.62516},
        {0.01599,0.04445,0.04909,0.05463,0.06121,0.06915,0.07863,0.09002,0.10362,0.12008,0.13969,0.16312,0.19159,0.22554,0.26603,0.31504,0.37378,0.44401,0.52826,0.63021},
        {0.01725,0.05099,0.05562,0.06116,0.06776,0.07571,0.0852,0.0966,0.11021,0.12663,0.14624,0.16989,0.19816,0.23187,0.27285,0.32154,0.37991,0.45066,0.53517,0.63629},
        {0.01828,0.0595,0.06496,0.07102,0.07772,0.08565,0.09517,0.10657,0.1201,0.13655,0.15615,0.17979,0.20793,0.24203,0.28263,0.33157,0.39033,0.46056,0.54452,0.64661},
        {0.01883,0.06873,0.07544,0.08288,0.09107,0.10013,0.11015,0.12151,0.13516,0.15153,0.17111,0.19472,0.22285,0.25666,0.29751,0.34631,0.40489,0.47545,0.55956,0.66094},
        {0.01845,0.0785,0.0867,0.09579,0.10582,0.11693,0.12919,0.14271,0.15759,0.17411,0.19367,0.21724,0.24548,0.27934,0.31999,0.36858,0.42712,0.49752,0.58201,0.68276},
        {0.01646,0.08844,0.09834,0.10937,0.12158,0.13513,0.15012,0.16665,0.18486,0.20491,0.22696,0.25122,0.27927,0.31309,0.35373,0.40262,0.46108,0.53133,0.61517,0.7165},
        {0.01185,0.09799,0.10985,0.1231,0.13782,0.15422,0.17242,0.19254,0.21475,0.23925,0.26621,0.29586,0.3283,0.36417,0.40475,0.45354,0.51194,0.58182,0.66639,0.76729},
        {0.00306,0.10592,0.12015,0.13604,0.15368,0.17336,0.19527,0.21958,0.24651,0.27627,0.30911,0.34531,0.38512,0.42883,0.47682,0.52972,0.58852,0.65862,0.74272,0.84342},
        {-0.01223,0.11028,0.12729,0.14633,0.16747,0.19107,0.21727,0.24647,0.27887,0.31477,0.35447,0.39847,0.44687,0.50037,0.55907,0.62357,0.69437,0.77217,0.85787,0.95857},
        {-0.0375,0.1077,0.1281,0.1509,0.1761,0.2044,0.2359,0.2708,0.3096,0.3526,0.4004,0.4534,0.5119,0.5766,0.648,0.7267,0.8132,0.9082,1.0124,1.1268}
    },
    {
        {0.01414,0.05208,0.05945,0.06818,0.07869,0.09124,0.1062,0.1243,0.14589,0.1717,0.20295,0.24032,0.28517,0.33891,0.40337,0.48079,0.57359,0.68483,0.81818,0.97923},
        {0.01689,0.05454,0.06197,0.07069,0.08118,0.0938,0.10881,0.12696,0.14854,0.17454,0.20544,0.24277,0.28772,0.34114,0.40581,0.48331,0.57576,0.68755,0.82148,0.98179},
        {0.01975,0.05763,0.06492,0.07378,0.08423,0.09683,0.11183,0.12986,0.15157,0.17756,0.20858,0.24588,0.29084,0.34433,0.40913,0.48636,0.57937,0.69068,0.82437,0.98442},
        {0.02277,0.06239,0.06974,0.07855,0.08904,0.10157,0.11674,0.13473,0.15643,0.1823,0.21352,0.25084,0.29567,0.34945,0.41395,0.49145,0.58395,0.69573,0.82932,0.98947},
        {0.02625,0.06972,0.07702,0.0858,0.09624,0.10888,0.12403,0.14196,0.1637,0.18965,0.22068,0.25808,0.30274,0.35672,0.42113,0.49848,0.5914,0.70292,0.83661,0.99591},
        {0.03035,0.08109,0.08833,0.09705,0.10744,0.11994,0.13508,0.15304,0.17487,0.20069,0.23187,0.26914,0.31387,0.36742,0.43207,0.50934,0.60167,0.71289,0.84728,1.00762},
        {0.03544,0.09723,0.10532,0.11423,0.12454,0.13698,0.1519,0.16988,0.19157,0.21749,0.24863,0.2858,0.33069,0.38422,0.44884,0.52564,0.61893,0.73036,0.8637,1.02323},
        {0.04212,0.11648,0.12645,0.13746,0.14955,0.1629,0.17782,0.19559,0.2171,0.24278,0.27396,0.31129,0.35573,0.40952,0.47392,0.55093,0.64404,0.75465,0.88876,1.04916},
        {0.05108,0.13919,0.15143,0.16498,0.17988,0.19631,0.21438,0.23423,0.25605,0.28166,0.31248,0.34951,0.39394,0.44758,0.51208,0.58934,0.68182,0.79335,0.92605,1.08705},
        {0.06318,0.16607,0.18092,0.19746,0.21572,0.23593,0.25819,0.28264,0.30948,0.33892,0.37121,0.40795,0.4523,0.5056,0.56967,0.64639,0.73929,0.85039,0.98429,1.14309},
        {0.07971,0.19863,0.21625,0.23606,0.25813,0.28271,0.30992,0.33994,0.37296,0.40921,0.44892,0.49252,0.54012,0.59332,0.65712,0.73362,0.82572,0.93672,1.06982,1.22992},
        {0.10253,0.23947,0.25997,0.28317,0.30917,0.33847,0.37127,0.40767,0.44797,0.49237,0.54127,0.59487,0.65357,0.71787,0.78827,0.86577,0.95737,1.06767,1.20017,1.35937},
        {0.1346,0.2923,0.3158,0.3426,0.3727,0.4069,0.4454,0.4887,0.537,0.5908,0.6503,0.7159,0.7881,0.8672,0.9539,1.0486,1.1524,1.266,1.3975,1.5559}
    },
};
LUT invX1SlopeLUT = {
    {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5},
    {0.0,0.00894,0.01073,0.01288,0.01545,0.01854,0.02225,0.0267,0.03204,0.03845,0.04614,0.05537,0.06644,0.07973,0.09568,0.11482,0.13779,0.16535,0.19842,0.23811},
    {
        {0.00521,0.03523,0.04124,0.04852,0.0572,0.06759,0.08006,0.09513,0.11322,0.13477,0.16075,0.19197,0.22927,0.27409,0.32786,0.39253,0.47015,0.56294,0.67423,0.80832},
        {0.00646,0.03522,0.04124,0.04852,0.05721,0.0677,0.08018,0.09517,0.1132,0.13484,0.16075,0.19197,0.22922,0.27396,0.3278,0.39228,0.47004,0.56295,0.67444,0.80868},
        {0.00817,0.0352,0.04124,0.04854,0.05718,0.06757,0.08012,0.09515,0.11321,0.13489,0.16078,0.19192,0.22911,0.27394,0.32799,0.39254,0.47025,0.5633,0.6749,0.80832},
        {0.01025,0.03585,0.0415,0.04851,0.05719,0.06759,0.0802,0.09524,0.11311,0.13483,0.16082,0.19192,0.2292,0.27419,0.32793,0.39232,0.47022,0.56309,0.67441,0.80879},
        {0.01292,0.03859,0.04374,0.05018,0.05813,0.06792,0.08019,0.09502,0.11317,0.13475,0.16071,0.19189,0.22933,0.27412,0.32784,0.39255,0.46994,0.5628,0.67493,0.80865},
        {0.01641,0.04496,0.04933,0.05496,0.06213,0.07109,0.08229,0.09616,0.11352,0.13474,0.16083,0.19209,0.22915,0.27418,0.3281,0.39233,0.47014,0.56326,0.67463,0.80871},
        {0.02118,0.05542,0.06014,0.06522,0.0713,0.07914,0.0891,0.10171,0.11752,0.13723,0.16175,0.19193,0.22931,0.27387,0.32807,0.39283,0.46981,0.5632,0.6749,0.808},
        {0.02775,0.06869,0.07424,0.08048,0.08748,0.09524,0.10375,0.11459,0.12867,0.14638,0.16864,0.19669,0.23124,0.27466,0.32764,0.39282,0.46982,0.56309,0.67472,0.80863},
        {0.03679,0.08536,0.09208,0.09964,0.10813,0.11724,0.12752,0.13911,0.15163,0.16682,0.18663,0.21148,0.24316,0.28282,0.33246,0.39364,0.47018,0.56242,0.67496,0.80914},
        {0.04944,0.10693,0.11493,0.12405,0.13397,0.14517,0.15779,0.17128,0.18651,0.20368,0.22233,0.24375,0.27149,0.30661,0.35141,0.4078,0.47852,0.56641,0.67435,0.80893},
        {0.06721,0.13548,0.14499,0.15558,0.16738,0.18072,0.19566,0.21212,0.23048,0.25086,0.27328,0.29873,0.32606,0.35681,0.39589,0.44564,0.5095,0.58929,0.68949,0.8153},
        {0.09259,0.17375,0.18487,0.19741,0.21139,0.22712,0.24457,0.26441,0.28609,0.31055,0.33807,0.3677,0.401,0.4382,0.4784,0.5232,0.5778,0.6488,0.7389,0.8531},
        {0.1294,0.2253,0.2386,0.2537,0.2702,0.2887,0.3095,0.3326,0.3584,0.3875,0.4197,0.4556,0.4955,0.54,0.5886,0.6428,0.7034,0.7679,0.8456,0.9465}
    },
    {
        {0.0091,0.06316,0.07398,0.08699,0.10261,0.12127,0.1439,0.17081,0.20276,0.24179,0.28841,0.34428,0.41129,0.49152,0.58839,0.7041,0.84263,1.00777,1.20908,1.45043},
        {0.00992,0.06326,0.07413,0.08701,0.10256,0.12113,0.1437,0.17076,0.2027,0.24187,0.28828,0.34388,0.41139,0.4912,0.58846,0.70337,0.84306,1.00985,1.20987,1.45107},
        {0.01228,0.06323,0.07416,0.08719,0.10251,0.1213,0.14365,0.17047,0.20313,0.24195,0.28844,0.34404,0.4111,0.49112,0.58824,0.70404,0.8428,1.00831,1.20998,1.45143},
        {0.01589,0.06326,0.07411,0.08705,0.10265,0.12124,0.14375,0.17051,0.20299,0.24169,0.28838,0.34383,0.41111,0.49129,0.58816,0.70379,0.84265,1.00961,1.20761,1.44739},
        {0.01934,0.06419,0.07447,0.08716,0.10258,0.1213,0.14356,0.17061,0.20336,0.24175,0.2883,0.3437,0.41124,0.49162,0.58787,0.70388,0.84288,1.00931,1.20877,1.44844},
        {0.02358,0.06835,0.07778,0.08937,0.10374,0.12198,0.14358,0.17053,0.20279,0.24154,0.28829,0.34403,0.41061,0.49141,0.58811,0.70424,0.84189,1.00859,1.21232,1.44799},
        {0.0287,0.07787,0.08612,0.09654,0.10971,0.12598,0.14626,0.17183,0.20291,0.24247,0.28818,0.34518,0.41111,0.49124,0.58808,0.7033,0.84271,1.00933,1.20988,1.44844},
        {0.03531,0.095,0.10242,0.11153,0.12304,0.1376,0.15611,0.1793,0.2083,0.24442,0.28874,0.3451,0.41095,0.49209,0.58885,0.70406,0.84371,1.0089,1.20886,1.45025},
        {0.04421,0.11699,0.12634,0.13685,0.14807,0.16111,0.17734,0.19782,0.22382,0.25681,0.29786,0.34903,0.41383,0.49153,0.5873,0.70339,0.84214,1.00958,1.20801,1.45223},
        {0.05666,0.14367,0.15574,0.16833,0.1824,0.19819,0.21515,0.23406,0.25706,0.28595,0.32357,0.36983,0.42796,0.50163,0.59143,0.70453,0.84207,1.00826,1.21026,1.44879},
        {0.07443,0.1759,0.1907,0.20691,0.22451,0.24389,0.26542,0.28807,0.31398,0.34126,0.37415,0.4154,0.46783,0.53375,0.61733,0.72135,0.8527,1.0123,1.2085,1.4482},
        {0.09993,0.21561,0.23358,0.25311,0.27517,0.2995,0.3258,0.3546,0.3867,0.4202,0.4586,0.4995,0.5462,0.6049,0.6785,0.7731,0.8918,1.0398,1.2238,1.4566},
        {0.1372,0.2666,0.2873,0.3107,0.3369,0.3667,0.3989,0.4354,0.4748,0.5174,0.5648,0.6154,0.6699,0.7316,0.7984,0.8831,0.9884,1.1214,1.2893,1.5002}
    }
};
std::unordered_map<Arc, LUT> invX1Delay = {
    { {"A", "Y"}, invX1DelayLUT }
};
std::unordered_map<Arc, LUT> invX1Slope = {
    { {"A", "Y"}, invX1SlopeLUT }
};
std::unordered_map<std::string, double> invX1Capacitance = {
    {"A", 0.00295}
};
Cell invX1 = {"INVX1", {"A"}, {"Y"}, {"!A"}, "pF", invX1Capacitance, "ns", invX1Delay, invX1Slope};

LUT and2ADelayLUT = {
    {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5},
    {0.0,0.00841,0.01009,0.01211,0.01453,0.01743,0.02091,0.02509,0.03011,0.03613,0.04335,0.05202,0.06242,0.0749,0.08988,0.10786,0.12943,0.15532,0.18638,0.22366},
    {
        {0.05541,0.0872,0.09216,0.09792,0.10463,0.1125,0.1218,0.13281,0.14591,0.16157,0.18029,0.20264,0.22961,0.2618,0.30051,0.3469,0.40271,0.4693,0.54966,0.64593},
        {0.05756,0.08938,0.09432,0.10007,0.10678,0.11463,0.12393,0.13492,0.14806,0.16373,0.18238,0.20486,0.23164,0.26388,0.30265,0.34906,0.40467,0.47172,0.55183,0.64814},
        {0.06038,0.09215,0.09709,0.10286,0.10958,0.11745,0.12673,0.13777,0.1509,0.1665,0.18524,0.20768,0.23455,0.26682,0.30544,0.35199,0.40742,0.47447,0.55444,0.65083},
        {0.06496,0.0967,0.10164,0.10741,0.11414,0.12201,0.13131,0.14233,0.15544,0.17114,0.18982,0.21227,0.23913,0.27132,0.3101,0.35644,0.41229,0.47883,0.55931,0.65516},
        {0.0722,0.10391,0.10887,0.11465,0.12137,0.12926,0.13858,0.1496,0.16274,0.17837,0.19715,0.21948,0.24648,0.2787,0.3174,0.36387,0.41955,0.48644,0.56657,0.66248},
        {0.08298,0.11482,0.11982,0.1256,0.13233,0.14026,0.14959,0.1606,0.17372,0.18944,0.20818,0.2306,0.25746,0.28972,0.32837,0.37485,0.43045,0.49742,0.57738,0.67404},
        {0.09649,0.12982,0.13487,0.1407,0.14749,0.15544,0.16479,0.17583,0.18904,0.20471,0.22341,0.24588,0.27272,0.30508,0.34377,0.39022,0.44574,0.51281,0.59293,0.68886},
        {0.11254,0.14815,0.15341,0.15948,0.16649,0.17462,0.1841,0.19527,0.20848,0.2242,0.24296,0.26539,0.29228,0.32445,0.36325,0.4097,0.46535,0.53212,0.61241,0.70831},
        {0.13181,0.17031,0.17579,0.18207,0.18929,0.19762,0.20732,0.2187,0.23217,0.24805,0.26694,0.28945,0.31636,0.34852,0.38713,0.43373,0.48931,0.55608,0.63641,0.73251},
        {0.15568,0.19764,0.20344,0.21003,0.21753,0.22612,0.23605,0.24763,0.26121,0.2772,0.29621,0.31893,0.34602,0.37838,0.41707,0.46352,0.51913,0.58597,0.66629,0.76199},
        {0.18641,0.23216,0.23842,0.24543,0.25333,0.2623,0.27256,0.28442,0.29823,0.31439,0.3335,0.35625,0.38333,0.4157,0.45452,0.50112,0.55702,0.62372,0.70402,0.79982},
        {0.22707,0.27697,0.28377,0.29137,0.29987,0.30937,0.32007,0.33237,0.34657,0.36297,0.38227,0.40507,0.43217,0.46467,0.50347,0.55007,0.60597,0.67307,0.75327,0.84937},
        {0.2819,0.3365,0.344,0.3523,0.3616,0.3719,0.3833,0.3962,0.4109,0.4278,0.4475,0.4706,0.4978,0.5302,0.5691,0.6156,0.6714,0.7384,0.8185,0.9152}
    },
    {
        {0.04683,0.08888,0.09647,0.10553,0.11629,0.12929,0.14485,0.16351,0.18593,0.21286,0.24518,0.28358,0.32997,0.38581,0.45259,0.53282,0.62925,0.74482,0.88347,1.04997},
        {0.04877,0.09083,0.09843,0.10751,0.11833,0.1312,0.14675,0.16547,0.18796,0.21471,0.2469,0.28555,0.33182,0.38799,0.45462,0.53483,0.63068,0.74679,0.88468,1.05204},
        {0.05121,0.09331,0.10092,0.11,0.12082,0.1337,0.14928,0.16793,0.19049,0.21726,0.24934,0.28816,0.33426,0.39054,0.45718,0.53731,0.63377,0.74914,0.88778,1.05412},
        {0.05497,0.09707,0.10465,0.11371,0.12455,0.13756,0.15316,0.17184,0.1942,0.22121,0.2534,0.292,0.33843,0.39394,0.46076,0.54132,0.63762,0.75292,0.89192,1.05747},
        {0.06035,0.10248,0.1101,0.11919,0.13005,0.14306,0.15863,0.17722,0.19959,0.2267,0.2588,0.29752,0.3438,0.39965,0.46611,0.54657,0.64308,0.75823,0.89695,1.06391},
        {0.06633,0.10912,0.11673,0.12579,0.13672,0.14967,0.16519,0.18389,0.20647,0.23343,0.26568,0.30392,0.35069,0.40628,0.47312,0.55348,0.64921,0.76533,0.90411,1.07062},
        {0.07252,0.1162,0.12388,0.13305,0.1439,0.15693,0.1725,0.1911,0.21354,0.24052,0.27285,0.3116,0.35769,0.41379,0.48056,0.56029,0.65712,0.77241,0.91113,1.07783},
        {0.07863,0.12316,0.13087,0.14013,0.15113,0.16421,0.17989,0.19865,0.22106,0.24794,0.2802,0.31903,0.36504,0.42124,0.48815,0.56831,0.66455,0.77959,0.91886,1.08466},
        {0.08401,0.13015,0.13792,0.14711,0.15812,0.17122,0.18685,0.20572,0.22824,0.25529,0.28753,0.32614,0.37259,0.42815,0.49544,0.57566,0.67173,0.78745,0.92635,1.09165},
        {0.08756,0.13651,0.14434,0.15363,0.16466,0.17777,0.19346,0.21231,0.23481,0.26201,0.29455,0.33318,0.37955,0.43531,0.50211,0.58227,0.67869,0.79399,0.93349,1.09889},
        {0.08746,0.14064,0.1487,0.15817,0.16936,0.18263,0.19842,0.21737,0.23995,0.26705,0.29951,0.33833,0.38517,0.44114,0.50792,0.58782,0.68402,0.79952,0.93762,1.10382},
        {0.08099,0.13998,0.14842,0.15826,0.16977,0.18337,0.19937,0.21857,0.24157,0.26877,0.30167,0.34067,0.38747,0.44357,0.51017,0.59127,0.68817,0.80337,0.94087,1.10757},
        {0.064,0.1304,0.1396,0.15,0.1621,0.1761,0.1927,0.2123,0.2357,0.2635,0.2969,0.3365,0.3839,0.4402,0.508,0.5885,0.686,0.8019,0.9414,1.1066}
    },
};
LUT and2ASlopeLUT = {
    {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5},
    {0.0,0.00841,0.01009,0.01211,0.01453,0.01743,0.02091,0.02509,0.03011,0.03613,0.04335,0.05202,0.06242,0.0749,0.08988,0.10786,0.12943,0.15532,0.18638,0.22366},
    {
        {0.01182,0.04089,0.04635,0.05286,0.06079,0.07029,0.08174,0.09569,0.11262,0.13306,0.1576,0.18736,0.22322,0.26622,0.31807,0.38029,0.45493,0.54449,0.65164,0.78127},
        {0.01182,0.0409,0.04632,0.05289,0.06078,0.07024,0.08171,0.09575,0.11263,0.13301,0.15756,0.18743,0.22293,0.26617,0.3181,0.3803,0.45489,0.54463,0.65191,0.78127},
        {0.01183,0.0409,0.04635,0.0529,0.06074,0.07029,0.0818,0.09573,0.1125,0.13301,0.1577,0.18728,0.22317,0.26631,0.31783,0.38037,0.4545,0.54465,0.65151,0.78017},
        {0.01182,0.0409,0.04637,0.0529,0.06077,0.07027,0.08172,0.09573,0.11264,0.13302,0.15748,0.18743,0.22305,0.26596,0.31811,0.3801,0.45498,0.54412,0.65196,0.78117},
        {0.01189,0.04095,0.0464,0.05292,0.0608,0.07032,0.08172,0.09575,0.11264,0.13306,0.15766,0.1873,0.22322,0.26606,0.3181,0.38032,0.45503,0.54446,0.65221,0.7806},
        {0.01246,0.04119,0.04655,0.05308,0.06089,0.07041,0.08187,0.09581,0.11269,0.133,0.1576,0.1874,0.22299,0.2664,0.3179,0.38026,0.45483,0.54428,0.65208,0.78053},
        {0.01417,0.04229,0.04753,0.05392,0.06164,0.07095,0.08231,0.09615,0.1128,0.13305,0.15761,0.18745,0.22319,0.2663,0.31812,0.38029,0.45484,0.54435,0.65221,0.78048},
        {0.01638,0.04475,0.04986,0.05613,0.06367,0.07279,0.08385,0.09743,0.11389,0.13393,0.15826,0.18784,0.22346,0.26603,0.31803,0.38036,0.45497,0.54412,0.65222,0.78061},
        {0.01902,0.04771,0.05274,0.05888,0.06628,0.0753,0.08626,0.09961,0.11589,0.13573,0.1597,0.18895,0.22434,0.26703,0.31837,0.38019,0.45482,0.54407,0.65205,0.78117},
        {0.02202,0.05174,0.05664,0.06258,0.06973,0.07843,0.0891,0.10209,0.11818,0.13786,0.16161,0.19033,0.22531,0.26792,0.31919,0.38112,0.45543,0.54398,0.65211,0.77992},
        {0.02548,0.05677,0.06164,0.06742,0.07437,0.08276,0.09301,0.10562,0.12122,0.14031,0.16369,0.19231,0.22706,0.26936,0.32021,0.38137,0.45571,0.54467,0.6522,0.78151},
        {0.0298,0.0629,0.0679,0.0737,0.0805,0.0886,0.0984,0.1105,0.1255,0.1439,0.1667,0.1947,0.2291,0.271,0.3216,0.3835,0.4571,0.5459,0.653,0.7807},
        {0.0353,0.0708,0.0759,0.0818,0.0886,0.0966,0.1062,0.1178,0.1321,0.1497,0.1716,0.1987,0.2323,0.2735,0.3236,0.3841,0.4583,0.5474,0.6543,0.7824}
    },
    {
        {0.01518,0.06831,0.07946,0.09257,0.1087,0.12778,0.15059,0.17865,0.21197,0.25156,0.30004,0.35697,0.42654,0.51067,0.60971,0.72883,0.87216,1.04493,1.25157,1.50099},
        {0.01517,0.06839,0.07942,0.09251,0.10837,0.1275,0.15098,0.1786,0.21228,0.25155,0.29959,0.35763,0.42735,0.50981,0.60936,0.73009,0.87246,1.04644,1.25071,1.5008},
        {0.0152,0.06833,0.07943,0.09251,0.10847,0.12749,0.15092,0.17874,0.21198,0.25159,0.29974,0.35786,0.42626,0.51007,0.60954,0.72865,0.87316,1.04552,1.25045,1.498},
        {0.01519,0.06848,0.07933,0.09252,0.10838,0.12783,0.15052,0.17849,0.21154,0.25192,0.29968,0.3569,0.42675,0.5102,0.60971,0.72987,0.87297,1.04486,1.25082,1.49802},
        {0.01557,0.0684,0.07948,0.09265,0.10862,0.12745,0.15058,0.1788,0.21172,0.2519,0.29951,0.35844,0.42662,0.50992,0.60934,0.72858,0.87447,1.04496,1.25022,1.49885},
        {0.01651,0.06881,0.07969,0.09285,0.10858,0.12767,0.15066,0.17837,0.21151,0.25189,0.29991,0.35734,0.42676,0.51065,0.60907,0.72912,0.87194,1.04662,1.25237,1.49935},
        {0.01766,0.06972,0.08038,0.09334,0.10916,0.12816,0.15083,0.17914,0.21222,0.25238,0.30009,0.35711,0.42649,0.50948,0.60985,0.72843,0.87337,1.04445,1.25224,1.50078},
        {0.01949,0.07052,0.08132,0.0943,0.1099,0.12892,0.1516,0.17909,0.21254,0.25214,0.29978,0.35827,0.42661,0.5092,0.60941,0.73017,0.8729,1.04529,1.2522,1.49866},
        {0.02197,0.07163,0.08218,0.09507,0.11078,0.12949,0.15245,0.17999,0.21301,0.25278,0.30026,0.35844,0.42673,0.51081,0.60981,0.72868,0.87216,1.0478,1.25235,1.49769},
        {0.02513,0.07389,0.08399,0.0967,0.11201,0.13069,0.15358,0.18059,0.21435,0.2537,0.3011,0.35843,0.42813,0.51024,0.60995,0.72968,0.87189,1.04734,1.25374,1.49897},
        {0.02888,0.07781,0.08775,0.09986,0.115,0.13325,0.1555,0.18299,0.21573,0.2551,0.30274,0.36032,0.42902,0.51191,0.61065,0.73018,0.875,1.04508,1.2519,1.5028},
        {0.03353,0.08434,0.0939,0.10553,0.12011,0.13807,0.15985,0.18657,0.2195,0.25895,0.3058,0.3635,0.4314,0.5152,0.6127,0.7321,0.8747,1.0471,1.2514,1.5001},
        {0.0395,0.0944,0.1035,0.1149,0.1287,0.146,0.1675,0.1937,0.2257,0.2647,0.3118,0.3696,0.4372,0.5192,0.6176,0.7365,0.8809,1.049,1.2566,1.5015}
    }
};
LUT and2BDelayLUT = {
    {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5},
    {0.0,0.00841,0.01009,0.01211,0.01453,0.01743,0.02091,0.02509,0.03011,0.03613,0.04335,0.05202,0.06242,0.0749,0.08988,0.10786,0.12943,0.15532,0.18638,0.22366},
    {
        {0.06139,0.0929,0.0978,0.10351,0.11018,0.11801,0.12728,0.13826,0.15137,0.16699,0.18572,0.20813,0.23501,0.26718,0.30591,0.35237,0.40817,0.47492,0.55518,0.65158},
        {0.06364,0.09517,0.10007,0.10579,0.11248,0.12031,0.12958,0.14058,0.15367,0.16932,0.18802,0.21041,0.23733,0.26959,0.30824,0.3547,0.4104,0.47711,0.55748,0.65383},
        {0.06657,0.09811,0.10302,0.10873,0.11542,0.12327,0.13254,0.14351,0.15665,0.17224,0.19097,0.21339,0.24024,0.27245,0.31129,0.35749,0.41342,0.47999,0.56007,0.65689},
        {0.07128,0.1028,0.10772,0.11345,0.12014,0.12798,0.13725,0.14825,0.16135,0.177,0.19572,0.21812,0.24505,0.27729,0.31594,0.3625,0.41815,0.48473,0.56498,0.66138},
        {0.07869,0.11024,0.11515,0.12088,0.12758,0.13544,0.14471,0.15573,0.16881,0.18449,0.20319,0.22558,0.25252,0.28476,0.32342,0.36999,0.42551,0.49246,0.57273,0.66894},
        {0.09009,0.12165,0.12657,0.13231,0.13904,0.14692,0.15617,0.16718,0.18031,0.19592,0.2146,0.23706,0.26401,0.29618,0.33482,0.38128,0.43709,0.50397,0.58411,0.68003},
        {0.10573,0.13819,0.14315,0.14893,0.15565,0.16353,0.17283,0.18389,0.197,0.21266,0.23134,0.25383,0.28064,0.31296,0.35158,0.39819,0.45354,0.52048,0.60099,0.6972},
        {0.12458,0.15944,0.16458,0.17051,0.17737,0.18536,0.19473,0.20579,0.21898,0.23464,0.25336,0.27583,0.30262,0.33484,0.37363,0.41984,0.47582,0.54227,0.62284,0.71864},
        {0.14736,0.18522,0.19057,0.19672,0.20379,0.21199,0.22157,0.23284,0.24616,0.26195,0.28074,0.30323,0.33013,0.36233,0.40091,0.44747,0.50316,0.56981,0.65022,0.74649},
        {0.1755,0.21709,0.22274,0.22918,0.23652,0.24495,0.25473,0.26615,0.27961,0.2955,0.31444,0.337,0.36406,0.39631,0.43502,0.48139,0.53688,0.60393,0.68409,0.78029},
        {0.21158,0.25732,0.2634,0.27024,0.27796,0.28673,0.29681,0.30849,0.32213,0.33815,0.35715,0.3797,0.40679,0.43916,0.47782,0.52442,0.58012,0.64692,0.72702,0.82332},
        {0.25947,0.30957,0.31627,0.32367,0.33187,0.34107,0.35157,0.36367,0.37757,0.39387,0.41297,0.43567,0.46267,0.49497,0.53367,0.58037,0.63597,0.70297,0.78297,0.87907},
        {0.3248,0.3795,0.3868,0.3948,0.4037,0.4136,0.4247,0.4373,0.4517,0.4683,0.4877,0.5105,0.5376,0.5699,0.6085,0.6549,0.7108,0.7775,0.8577,0.954}
    },
    {
        {0.05129,0.09308,0.10056,0.10954,0.12019,0.1331,0.14856,0.16712,0.18941,0.21602,0.24844,0.28692,0.33331,0.38888,0.45549,0.53596,0.63217,0.74703,0.88644,1.05297},
        {0.05315,0.09495,0.10243,0.11136,0.12214,0.13504,0.15044,0.16901,0.19133,0.21792,0.25032,0.28864,0.33516,0.39066,0.45723,0.53749,0.63366,0.74914,0.88814,1.05494},
        {0.05522,0.09702,0.10452,0.11347,0.12427,0.13715,0.15255,0.17114,0.19348,0.22009,0.25248,0.29079,0.33739,0.39283,0.45938,0.54012,0.636,0.7514,0.89042,1.05712},
        {0.05821,0.1,0.10754,0.11657,0.12734,0.14017,0.15557,0.17416,0.1967,0.22329,0.25545,0.29424,0.34076,0.39631,0.46319,0.54277,0.63972,0.75497,0.89382,1.06008},
        {0.0624,0.10433,0.1119,0.12088,0.13171,0.14454,0.16003,0.17859,0.20092,0.22791,0.26001,0.29863,0.34517,0.40037,0.46767,0.54785,0.64401,0.75961,0.89787,1.06421},
        {0.0675,0.11008,0.11765,0.12669,0.1375,0.15045,0.16594,0.18451,0.20705,0.23383,0.26615,0.3046,0.35089,0.40667,0.47352,0.55377,0.65004,0.76476,0.90357,1.07002},
        {0.0729,0.11651,0.12414,0.13326,0.14409,0.15704,0.17253,0.19124,0.21368,0.2404,0.27277,0.3111,0.35783,0.41339,0.47997,0.56056,0.65637,0.77199,0.91083,1.07733},
        {0.07804,0.12283,0.13052,0.13971,0.15066,0.16367,0.17927,0.19799,0.22039,0.24723,0.2794,0.31804,0.36474,0.41995,0.48712,0.56742,0.66294,0.77917,0.91706,1.08386},
        {0.08198,0.12857,0.13631,0.14552,0.15646,0.16947,0.18515,0.20396,0.22652,0.25324,0.28554,0.3242,0.37057,0.42627,0.49316,0.5732,0.67002,0.78575,0.92365,1.09055},
        {0.08328,0.1326,0.14047,0.14977,0.16078,0.17387,0.1895,0.20823,0.23066,0.25755,0.29016,0.32895,0.37498,0.43058,0.49741,0.57761,0.67369,0.78979,0.92889,1.09459},
        {0.07976,0.13312,0.1412,0.15069,0.16184,0.17503,0.19072,0.20949,0.23203,0.25902,0.29123,0.33011,0.37658,0.43225,0.49972,0.57902,0.67542,0.79072,0.92902,1.09542},
        {0.0682,0.12694,0.1354,0.14522,0.15669,0.17017,0.18607,0.20507,0.22777,0.25487,0.28737,0.32627,0.37277,0.42897,0.49537,0.57597,0.67247,0.78787,0.92537,1.09187},
        {0.0438,0.1092,0.1183,0.1287,0.1406,0.1546,0.171,0.1903,0.2134,0.2408,0.2737,0.313,0.3597,0.4161,0.4828,0.5637,0.6596,0.7763,0.915,1.0823}
    },
};
LUT and2BSlopeLUT = {
    {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5},
    {0.0,0.00841,0.01009,0.01211,0.01453,0.01743,0.02091,0.02509,0.03011,0.03613,0.04335,0.05202,0.06242,0.0749,0.08988,0.10786,0.12943,0.15532,0.18638,0.22366},
    {
        {0.01262,0.04131,0.04668,0.05319,0.06106,0.07053,0.08193,0.0959,0.11277,0.13311,0.15782,0.18757,0.22336,0.26638,0.3181,0.38019,0.45507,0.54485,0.65204,0.78109},
        {0.0126,0.04129,0.04669,0.05321,0.06098,0.07051,0.08198,0.09584,0.11279,0.13303,0.15785,0.18751,0.22337,0.2665,0.31797,0.38027,0.45492,0.54409,0.65201,0.78125},
        {0.01263,0.04131,0.04668,0.05319,0.06103,0.07045,0.08194,0.09591,0.11273,0.13319,0.15762,0.18739,0.22328,0.26634,0.31826,0.37999,0.45486,0.54416,0.65203,0.78124},
        {0.01261,0.04127,0.04669,0.05319,0.06097,0.07051,0.08197,0.09584,0.1128,0.13302,0.15785,0.18751,0.22339,0.26637,0.31793,0.38043,0.45491,0.54412,0.65138,0.78145},
        {0.01263,0.04131,0.04666,0.0532,0.06099,0.07047,0.08198,0.09579,0.1128,0.13306,0.15784,0.18747,0.22336,0.2663,0.31801,0.38045,0.45452,0.54463,0.65235,0.78147},
        {0.01281,0.04136,0.04674,0.05325,0.06106,0.07044,0.08199,0.09589,0.11277,0.13317,0.15779,0.18749,0.22339,0.26609,0.31792,0.37995,0.45498,0.54484,0.65192,0.78062},
        {0.01432,0.04198,0.04724,0.05365,0.06142,0.07081,0.08222,0.09598,0.11288,0.13322,0.15789,0.18748,0.22323,0.26649,0.31811,0.38048,0.45445,0.54408,0.65202,0.78063},
        {0.01656,0.04406,0.04923,0.0554,0.06295,0.07219,0.08333,0.097,0.11356,0.13366,0.15814,0.18778,0.22355,0.26615,0.31834,0.38005,0.4551,0.54413,0.65212,0.78055},
        {0.01941,0.04698,0.05202,0.0581,0.06553,0.07454,0.08543,0.09885,0.11525,0.13509,0.15927,0.18854,0.22408,0.267,0.31841,0.38047,0.45518,0.54428,0.65234,0.78134},
        {0.02265,0.051,0.05582,0.06169,0.06882,0.07751,0.08825,0.10138,0.1174,0.1371,0.16095,0.18995,0.22521,0.26759,0.31918,0.38114,0.45543,0.54451,0.65231,0.78132},
        {0.02632,0.05613,0.0608,0.0665,0.07329,0.0817,0.09191,0.10467,0.12033,0.1395,0.16293,0.19195,0.22677,0.26903,0.32005,0.38165,0.45539,0.54482,0.65254,0.7807},
        {0.0306,0.0623,0.0669,0.0725,0.0792,0.0872,0.0971,0.1092,0.1242,0.1429,0.1659,0.194,0.2283,0.2706,0.3216,0.3831,0.4567,0.5451,0.6524,0.781},
        {0.0359,0.0696,0.0745,0.08,0.0866,0.0944,0.1039,0.1155,0.1299,0.1478,0.1699,0.1973,0.2313,0.2726,0.323,0.3841,0.458,0.5475,0.6544,0.7818}
    },
    {
        {0.0152,0.0685,0.07942,0.09261,0.10874,0.12785,0.15062,0.17821,0.21146,0.25174,0.29995,0.35692,0.42731,0.50955,0.609,0.72846,0.87281,1.04442,1.25184,1.50023},
        {0.01521,0.06848,0.0794,0.09258,0.10844,0.12755,0.15104,0.1788,0.21151,0.25174,0.29974,0.35719,0.42659,0.50946,0.6087,0.72832,0.87167,1.04374,1.2519,1.49822},
        {0.01521,0.06849,0.0794,0.09262,0.10845,0.12788,0.15102,0.17881,0.2115,0.25174,0.29978,0.35717,0.42705,0.5104,0.60921,0.72952,0.87293,1.04353,1.25224,1.49856},
        {0.0152,0.06836,0.07944,0.09277,0.10837,0.12746,0.15102,0.17878,0.21201,0.25168,0.2997,0.35837,0.42672,0.50997,0.61087,0.72827,0.87357,1.04396,1.25066,1.49849},
        {0.01549,0.06841,0.07948,0.09263,0.10844,0.12752,0.15101,0.17894,0.21162,0.25187,0.29997,0.35737,0.42742,0.509,0.61139,0.72919,0.87334,1.04447,1.25205,1.50015},
        {0.01632,0.06875,0.07967,0.09295,0.10868,0.12751,0.15062,0.17854,0.21239,0.25143,0.30009,0.35728,0.42651,0.50991,0.60954,0.72939,0.87169,1.0444,1.25033,1.49871},
        {0.01743,0.06947,0.08015,0.0933,0.10908,0.12782,0.15096,0.17848,0.21246,0.25177,0.29957,0.35797,0.42769,0.51062,0.6087,0.72991,0.8725,1.04497,1.25209,1.499},
        {0.01911,0.0703,0.08101,0.09394,0.10957,0.12864,0.15148,0.17909,0.21202,0.25199,0.29986,0.35757,0.42625,0.50958,0.60867,0.72974,0.87172,1.04519,1.25006,1.4994},
        {0.0214,0.07151,0.08204,0.09486,0.11041,0.12923,0.15189,0.17953,0.21233,0.25265,0.30082,0.35799,0.42701,0.50929,0.61078,0.7289,0.87164,1.04493,1.25371,1.50267},
        {0.02434,0.07366,0.0838,0.09633,0.11166,0.13031,0.1529,0.18017,0.21357,0.25286,0.30091,0.35817,0.42714,0.50984,0.60901,0.72837,0.8732,1.0437,1.2498,1.49841},
        {0.02785,0.07729,0.0871,0.09942,0.11434,0.13243,0.15453,0.18159,0.21481,0.2545,0.30227,0.35905,0.42766,0.51106,0.60937,0.72954,0.87469,1.04459,1.25157,1.5019},
        {0.03208,0.08331,0.09283,0.10434,0.11888,0.13677,0.15838,0.18494,0.21732,0.25693,0.30376,0.3607,0.4293,0.5119,0.6109,0.7304,0.8732,1.0448,1.2505,1.4986},
        {0.0372,0.0919,0.1011,0.1125,0.1264,0.1435,0.1649,0.1911,0.2228,0.2616,0.3091,0.3654,0.4334,0.5159,0.6157,0.7347,0.8778,1.0471,1.2545,1.4997}
    }
};
std::unordered_map<Arc, LUT> and2Delay = {
    { {"A", "Y"}, and2ADelayLUT },
    { {"B", "Y"}, and2BDelayLUT }
};
std::unordered_map<Arc, LUT> and2Slope = {
    { {"A", "Y"}, and2ASlopeLUT },
    { {"B", "Y"}, and2BSlopeLUT }
};
std::unordered_map<std::string, double> and2Capacitance = {
    {"A", 0.0024},
    {"B", 0.0023}
};
Cell and2 = {"AND2X1", {"A", "B"}, {"Y"}, {"A&B"}, "pF", and2Capacitance, "ns", and2Delay, and2Slope};

std::unordered_map<std::string, Cell> cellLib =
{
    {invX1.name, invX1},
    {and2.name, and2}
};