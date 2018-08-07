// Atmospheric model for altimeter
// 100 elements piece-wise linear aproximation
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
#define BREAKPOINTS_STEP		800		//Pa
#define BREAKPOINTS_NUMBER		101

const float height_array[BREAKPOINTS_NUMBER] PROGMEM = {
-710.4,
-646.9,
-582.9,
-518.6,
-453.8,
-388.7,
-323.2,
-257.3,
-190.9,
-124.2,
-57.0,
10.6,
78.6,
147.1,
216.0,
285.4,
355.2,
425.5,
496.3,
567.6,
639.3,
711.6,
784.3,
857.6,
931.3,
1005.7,
1080.5,
1155.9,
1231.8,
1308.3,
1385.4,
1463.1,
1541.3,
1620.2,
1699.7,
1779.8,
1860.5,
1941.9,
2023.9,
2106.6,
2190.0,
2274.1,
2358.9,
2444.4,
2530.7,
2617.7,
2705.4,
2794.0,
2883.3,
2973.5,
3064.5,
3156.3,
3249.0,
3342.5,
3437.0,
3532.4,
3628.8,
3726.1,
3824.3,
3923.6,
4024.0,
4125.3,
4227.8,
4331.3,
4436.0,
4541.9,
4648.9,
4757.2,
4866.7,
4977.5,
5089.6,
5203.0,
5317.9,
5434.2,
5551.9,
5671.2,
5792.0,
5914.4,
6038.5,
6164.2,
6291.7,
6421.1,
6552.3,
6685.4,
6820.5,
6957.7,
7097.0,
7238.5,
7382.3,
7528.5,
7677.1,
7828.3,
7982.2,
8138.8,
8298.3,
8460.8,
8626.4,
8795.2,
8967.5,
9143.4,
9323.0
};




