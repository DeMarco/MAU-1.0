#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

//#define TEST_MODE

/* Mapa EEPROM */

#ifdef TEST_MODE
	#define EEPROM_ADDRESS_DATA_MAX			448
	#define EEPROM_ADDRESS_GROUND_TEMP		448
	#define EEPROM_ADDRESS_PRESS_APOGEE		450
	#define EEPROM_ADDRESS_PRESS_GROUND		452
	#define EEPROM_ADDRESS_DIG_T1			454
	#define EEPROM_ADDRESS_DIG_T2			EEPROM_ADDRESS_DIG_T1 + 2
	#define EEPROM_ADDRESS_DIG_T3			EEPROM_ADDRESS_DIG_T2 + 2
	#define EEPROM_ADDRESS_DIG_P1			EEPROM_ADDRESS_DIG_T3 + 2
	#define EEPROM_ADDRESS_DIG_P2			EEPROM_ADDRESS_DIG_P1 + 2
	#define EEPROM_ADDRESS_DIG_P3			EEPROM_ADDRESS_DIG_P2 + 2
	#define EEPROM_ADDRESS_DIG_P4			EEPROM_ADDRESS_DIG_P3 + 2
	#define EEPROM_ADDRESS_DIG_P5			EEPROM_ADDRESS_DIG_P4 + 2
	#define EEPROM_ADDRESS_DIG_P6			EEPROM_ADDRESS_DIG_P5 + 2
	#define EEPROM_ADDRESS_DIG_P7			EEPROM_ADDRESS_DIG_P6 + 2
	#define EEPROM_ADDRESS_DIG_P8			EEPROM_ADDRESS_DIG_P7 + 2
	#define EEPROM_ADDRESS_DIG_P9			EEPROM_ADDRESS_DIG_P8 + 2 
	#define EEPROM_ADDRESS_DEBUG_1			478 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_DEBUG_2			482 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_H_MAX			486 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_H_GND			490 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_APOGEE			494 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_TRUE_TEMP_GND	498 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_TRUE_PRESS_LST	502 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_TRUE_PRESS_HST	506 //Temporário, usa 4 bytes
	#define EEPROM_ADDRESS_LAST				510 //Temporário, usa 2 bytes
	
#else
	#define EEPROM_ADDRESS_DATA_MAX			480
	#define EEPROM_ADDRESS_GROUND_TEMP		482
	#define EEPROM_ADDRESS_PRESS_APOGEE		484
	#define EEPROM_ADDRESS_PRESS_GROUND		486
	#define EEPROM_ADDRESS_DIG_T1			488
	#define EEPROM_ADDRESS_DIG_T2			EEPROM_ADDRESS_DIG_T1 + 2
	#define EEPROM_ADDRESS_DIG_T3			EEPROM_ADDRESS_DIG_T2 + 2
	#define EEPROM_ADDRESS_DIG_P1			EEPROM_ADDRESS_DIG_T3 + 2
	#define EEPROM_ADDRESS_DIG_P2			EEPROM_ADDRESS_DIG_P1 + 2
	#define EEPROM_ADDRESS_DIG_P3			EEPROM_ADDRESS_DIG_P2 + 2
	#define EEPROM_ADDRESS_DIG_P4			EEPROM_ADDRESS_DIG_P3 + 2
	#define EEPROM_ADDRESS_DIG_P5			EEPROM_ADDRESS_DIG_P4 + 2
	#define EEPROM_ADDRESS_DIG_P6			EEPROM_ADDRESS_DIG_P5 + 2
	#define EEPROM_ADDRESS_DIG_P7			EEPROM_ADDRESS_DIG_P6 + 2
	#define EEPROM_ADDRESS_DIG_P8			EEPROM_ADDRESS_DIG_P7 + 2
	#define EEPROM_ADDRESS_DIG_P9			EEPROM_ADDRESS_DIG_P8 + 2
#endif

uint16_t calib_data_temp[3], calib_data_press[9];
int32_t t_fine;

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t bmp280_compensate_temp(int32_t t_raw)
{
	uint16_t dig_T1;
	int16_t  dig_T2, dig_T3;
	int32_t var1, var2, T;
	
	dig_T1 = calib_data_temp[0];
	dig_T2 = calib_data_temp[1];
	dig_T3 = calib_data_temp[2];
	
	var1 = ((((t_raw>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((t_raw>>4) - ((int32_t)dig_T1)) * ((t_raw>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	//t_fine = 102375;
	T = (t_fine * 5 + 128) >> 8;
	
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t bmp280_compensate_press(int32_t p_raw)
{
	uint16_t dig_P1;
	int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
	int64_t var1, var2, p;
	
	dig_P1 = calib_data_press[0];
	dig_P2 = calib_data_press[1];
	dig_P3 = calib_data_press[2];
	dig_P4 = calib_data_press[3];
	dig_P5 = calib_data_press[4];
	dig_P6 = calib_data_press[5];
	dig_P7 = calib_data_press[6];
	dig_P8 = calib_data_press[7];
	dig_P9 = calib_data_press[8];
	
	var1 = ((int64_t)t_fine)-128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // Avoid exception caused by division by zero
	}
	p = 1048576-p_raw;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	
	return (uint32_t)p;
}


int main()
{
	FILE *in_data, *out_data, *flight_data_true, *flight_data_raw;
	int sample_count = 0;
	float temperature_true = 0.0, pressure_true = 0.0;
	unsigned int pressure_raw = 0, temperature_raw = 0;
	char sample_value_true[11], sample_value_raw[11];
	//int test_num;
	int prot_num;
	char newname[30];
	int ret = 0;
	
	//printf("\n\nType in the test number: ");
	//scanf("%d", &test_num);
	printf("\n\nDigite o numero do prototipo: ");
	scanf("%d", &prot_num);
	printf("\n\n");
	
	in_data = fopen("eeprom_contents.bin", "rb");
	flight_data_true = fopen("flight_data_true.txt", "w");
	flight_data_raw = fopen("flight_data_raw.txt", "w");
	
	if(!in_data)
	{
		printf ("\n\n\tErro na abertura do arquivo de entrada!\n\n\n");
		getch();
		return(1);
	}
	
	if(!flight_data_true)
	{
		printf ("\n\n\tErro na abertura do arquivo de saida de dados de voo!\n\n\n");
		getch();
		return(1);
	}
	
	if(!flight_data_raw)
	{
		printf ("\n\n\tErro na abertura do arquivo de saida de dados de voo brutos!\n\n\n");
		getch();
		return(1);
	}
	
	// Ler dados de calibracao do BMP280
	fseek(in_data, EEPROM_ADDRESS_DIG_T1, SEEK_SET);
	fread(calib_data_temp, 2, 3, in_data);
	fread(calib_data_press, 2, 9, in_data);
	
	// Calcular temperatura no solo
	fseek(in_data, EEPROM_ADDRESS_GROUND_TEMP, SEEK_SET);
	fread(&temperature_raw, 2, 1, in_data);
	//printf("Temp_raw = %x\n", temperature_raw);	
	temperature_true = bmp280_compensate_temp((int32_t)temperature_raw<<4) / 100.0;
	//printf("Temp_true = %.2f\n\n", temperature_true);
	sprintf(sample_value_true, "%.2f\n", temperature_true);
	sample_value_true[strlen(sample_value_true)-4] = ',';
	fprintf(flight_data_true, sample_value_true);
	
	// Calcular Pressao no solo
	fseek(in_data, EEPROM_ADDRESS_PRESS_GROUND, SEEK_SET);
	fread(&pressure_raw, 2, 1, in_data);
	//printf("Press_raw_GROUND = %x\n", pressure_raw);
	pressure_true = bmp280_compensate_press((int32_t)pressure_raw <<4) / 256.0;
	//printf("Press_true_GROUND = %f\n\n", pressure_true);
	sprintf(sample_value_true, "%.2f\n", pressure_true);
	sample_value_true[strlen(sample_value_true)-4] = ',';
	fprintf(flight_data_true, sample_value_true);
	
	// Calcular pressao no apogeu
	fseek(in_data, EEPROM_ADDRESS_PRESS_APOGEE, SEEK_SET);
	fread(&pressure_raw, 2, 1, in_data);
	//printf("Press_raw_APOGEE = %x\n", pressure_raw);
	pressure_true = bmp280_compensate_press((int32_t)pressure_raw <<4) / 256.0;
	//printf("Press_true_APOGEE = %f\n\n", pressure_true);
	sprintf(sample_value_true, "%.2f\n", pressure_true);
	sample_value_true[strlen(sample_value_true)-4] = ',';
	fprintf(flight_data_true, sample_value_true);
	
	if(prot_num == 1)
	{
		fprintf(flight_data_true, "=SE($J$1<>0;D3-($J$1*101325/760);\"N/A\")\n\nP1 Pressao\n");
	}
	else if(prot_num == 2)
	{
		fprintf(flight_data_true, "=SE($J$1<>0;I3-($J$1*101325/760);\"N/A\")\n\nP2 Pressao\n");
	}

	rewind(in_data);
	while(1)
	{
		fread(&pressure_raw, 2, 1, in_data);
		if((pressure_raw == 0xffff) || (sample_count == (EEPROM_ADDRESS_DATA_MAX / 2)))
		{
			printf("\n\nDownload dos dados de voo concluido!\n\n");
			break;
		}
		//printf("Press_raw = %x\n", pressure_raw);
		sprintf(sample_value_raw, "%d\n", pressure_raw);
		pressure_true = bmp280_compensate_press((int32_t)pressure_raw <<4) / 256.0;
		//printf("Press_true = %f\n\n", pressure_true);
		sprintf(sample_value_true, "%.2f\n", pressure_true);
		sample_value_true[strlen(sample_value_true)-4] = ',';
		fprintf(flight_data_true, sample_value_true);
		fprintf(flight_data_raw, sample_value_raw);
		sample_count++;
	}
	
	fclose(in_data);
	fclose(flight_data_true);
	fclose(flight_data_raw);
	

	sprintf(newname, "P%d_eeprom_contents.txt", prot_num);
	if(rename("eeprom_contents.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivo 1!\n\n\n");
		getch();
		//return 1;
	}
	
	sprintf(newname, "P%d_eeprom_contents.bin", prot_num);
	if(rename("eeprom_contents.bin", newname))
	{
		printf("\n\n\tErro ao renomear arquivo 2!\n\n\n");
		getch();
		//return 1;
	}
	
	sprintf(newname, "P%d_flight_data_raw.txt", prot_num);
	if(rename("flight_data_raw.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivo 3!\n\n\n");
		getch();
		//return 1;
	}
	
	sprintf(newname, "P%d_flight_data_true.txt", prot_num);
	if(rename("flight_data_true.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivo 4!\n\n\n");
		getch();
		//return 1;
	}
	
	return 0;
}
