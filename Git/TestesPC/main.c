#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#define	EQ1_Tb	(273,15 + 15)
#define	EQ1_Lb	-0,0065
#define	EQ1_R	8,31432
#define	EQ1_g	9,80665
#define	EQ1_M	0,0289644
#define	EQ1_Pb	101325.0


//int vet[3] = {1, 2, 3};

#define SENSOR_MAX_PRESSURE 	110000	//Pa
#define SENSOR_MIN_PRESSURE 	30000	//Pa
#define BREAKPOINTS_STEP		8000	//Pa
#define BREAKPOINTS_NUMBER		11

int press_array[BREAKPOINTS_NUMBER] = {
110000,
102000,
94000,
86000,
78000,
70000,
62000,
54000,
46000,
38000,
30000	
};

float height_array[BREAKPOINTS_NUMBER] = {
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


/*
double calculate_height(unsigned int P)
{
	int h = 0;
	
	//h = 44307 * (1 - pow((P >> 8)/EQ1_Pb, 0.2119));
	//h = 44307.0 * (1 - powf(P/EQ1_Pb, 0.2119));
	//h = ((P>>8)/100)-100; //TESTE
	
	h = 44330.0 * (1 - powf((double)P/EQ1_Pb, 0.1903));
	
	return h;
}
*/

double calculate_height(int press_highest)
{
	double height_ground = 0.0, height_apogee = 0.0, coef = 0.0;
	int i = 3;
	
	height_ground = ((height_array[i] - height_array[i-1]) / (press_array[i] - press_array[i-1])) * (press_highest - press_array[i-1]) + height_array[i-1];
	
	return height_ground;
}

int main() 
{
	/*
	double alt = 0;
	unsigned int Press = 0x016587;
	int count = 17;
	int samples[16];
	*/
	
	//alt = calculate_height(Press);
	//printf("%f", alt);
	//getch();
	
	
	/*while(count)
	{
		if(--count)
		{
			samples[count-1] = count-1;
			printf("%d\n", samples[count-1]);
		}
	}
	printf("\n");
	for(count = 0; count < 16; count++)
	{
        printf("%d\n", samples[count]);
    }
	getch();*/
	
	
	/*
	int press[450];
	double alt = 0;
	int i;
	
	press[0] = 110000;
	for(i=0; i<450; i++)
		press[i] -= 178;
	
	printf("%d %d %d", vet[0], vet[1], vet[2]);
	*/
	
	uint16_t array[5] = {0x6194, 0x61BC, 0x61D6, 0x61E6, 0x61EE};
	uint32_t numerator = 0;
	int32_t variance = 0, average = 0;
	int16_t term = 0;
	uint8_t count = 5;
	
	
	average = 25035;
	//for(count = 5; count; --count)
	for(count = 0; count < 5; count++)
	{
		term = array[count] - average;
		variance += term*term;
	}
	//term = array[count] - average;
	//variance += term*term;
	variance /= 5;
	
	printf("Variance = %d",  variance);

	return 0;
}
