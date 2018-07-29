// Atmospheric model for altimeter
// 50 elements piece-wise linear aproximation
// Range: 30 kPa (9165.4 m) to 110 kPa (-698.4 m)
// Equation: h = 44330 * (1 - pow(P/P0, 0.1903))
// Considerations:	P0 = 101325 Pa
//					g = 9,80665 m/s^2
//					T0 = 20 degC
//					L0 = -0,0065 K/m
//					R = 8,31432 N*m/Mol*K
//					M = 0,0289644 kg/Mol

#define SENSOR_MAX_PRESSURE 	110000	//Pa
#define SENSOR_MIN_PRESSURE 	30000	//Pa
#define BREAKPOINTS_STEP		1600	//Pa
#define BREAKPOINTS_NUMBER		51

const float height_array[BREAKPOINTS_NUMBER] PROGMEM = {
-698.3,
-573.0,
-446.1,
-317.7,
-187.7,
-56.0,
77.3,
212.3,
349.2,
487.9,
628.4,
770.9,
915.5,
1062.1,
1210.8,
1361.8,
1515.0,
1670.7,
1828.8,
1989.4,
2152.7,
2318.7,
2487.5,
2659.3,
2834.1,
3012.2,
3193.6,
3378.4,
3566.9,
3759.1,
3955.3,
4155.7,
4360.4,
4569.6,
4783.7,
5002.8,
5227.2,
5457.2,
5693.2,
5935.5,
6184.4,
6440.5,
6704.2,
6975.9,
7256.4,
7546.2,
7846.0,
8156.7,
8479.2,
8814.6,
9164.0
};





