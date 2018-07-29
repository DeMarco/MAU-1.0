// Atmospheric model for altimeter
// 50 elements piece-wise linear aproximation
// Range: 30 kPa (9165.4 m) to 110 kPa (-698.4 m)
// Equation: h = 44330 * (1 - pow(P/P0, 0.1903))
// Considerations:	P0 = 101325 Pa
//					g = 9,80665 m/s^2
//					T0 = 15 degC
//					L0 = -0,0065 K/m
//					R = 8,31432 N*m/Mol*K
//					M = 0,0289644 kg/Mol

#define SENSOR_MAX_PRESSURE 	110000	//Pa
#define SENSOR_MIN_PRESSURE 	30000	//Pa
#define BREAKPOINTS_STEP		1600	//Pa
#define BREAKPOINTS_NUMBER		51

const float height_array[BREAKPOINTS_NUMBER] PROGMEM = {
-698.4,
-573.1,
-446.2,
-317.7,
-187.7,
-56.0,
77.3,
212.4,
349.2,
487.9,
628.5,
771.1,
915.6,
1062.3,
1211.0,
1362.0,
1515.3,
1671.0,
1829.1,
1989.7,
2153.0,
2319.0,
2487.9,
2659.7,
2834.6,
3012.7,
3194.1,
3379.0,
3567.5,
3759.7,
3956.0,
4156.4,
4361.1,
4570.4,
4784.5,
5003.6,
5228.0,
5458.1,
5694.1,
5936.4,
6185.4,
6441.5,
6705.3,
6977.1,
7257.6,
7547.4,
7847.3,
8158.0,
8480.6,
8815.9,
9165.4
};





