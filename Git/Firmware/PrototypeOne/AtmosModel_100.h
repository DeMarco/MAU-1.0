// Atmospheric model for altimeter
// 100 elements piece-wise linear aproximation
// Range: 30 kPa (9165.4 m) to 110 kPa (-698.4 m)
// Equation: h = 44330 * (1 - pow(P/P0, 0.1903))
// Considerations:	P0 = 101325 Pa
//					g = 9,80665 m/s^2
//					T0 = 15ºC
//					L0 = -0,0065 K/m
//					R = 8,31432 N*m/Mol*K
//					M = 0,0289644 kg/Mol

#define SENSOR_MAX_PRESSURE 	110000	//Pa
#define SENSOR_MIN_PRESSURE 	30000	//Pa
#define BREAKPOINTS_STEP		800		//Pa
#define BREAKPOINTS_NUMBER		101

const float height_array[BREAKPOINTS_NUMBER] PROGMEM = {
-698.4,
-635.9,
-573.1,
-509.8,
-446.2,
-382.2,
-317.7,
-252.9,
-187.7,
-122.1,
-56.0,
10.4,
77.3,
144.6,
212.4,
280.6,
349.2,
418.4,
487.9,
558.0,
628.5,
699.5,
771.1,
843.1,
915.6,
988.7,
1062.3,
1136.4,
1211.0,
1286.2,
1362.0,
1438.4,
1515.3,
1592.8,
1671.0,
1749.7,
1829.1,
1909.1,
1989.7,
2071.0,
2153.0,
2235.7,
2319.0,
2403.1,
2487.9,
2573.4,
2659.7,
2746.8,
2834.6,
2923.2,
3012.7,
3103.0,
3194.1,
3286.1,
3379.0,
3472.8,
3567.5,
3663.1,
3759.7,
3857.4,
3956.0,
4055.6,
4156.4,
4258.2,
4361.1,
4465.1,
4570.4,
4676.8,
4784.5,
4893.4,
5003.6,
5115.1,
5228.0,
5342.3,
5458.1,
5575.3,
5694.1,
5814.5,
5936.4,
6060.1,
6185.4,
6312.6,
6441.5,
6572.4,
6705.3,
6840.1,
6977.1,
7116.2,
7257.6,
7401.3,
7547.4,
7696.0,
7847.3,
8001.2,
8158.0,
8317.7,
8480.6,
8646.6,
8815.9,
8988.8,
9165.4
};





