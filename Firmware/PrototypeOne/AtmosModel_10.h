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
#define BREAKPOINTS_STEP		8000	//Pa
#define BREAKPOINTS_NUMBER		11

const float height_array[BREAKPOINTS_NUMBER] PROGMEM = {
-698.4,
-56.0,
628.5,
1362.0,
2153.0,
3012.7,
3956.0,
5003.6,
6185.4,
7547.4,
9165.4
};





