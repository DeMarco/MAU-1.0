#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define TEST_MODE

/* NVM map */
#ifdef TEST_MODE

	// Extra debug addresses
	#define EEPROM_ADDRESS_DATA_MAX					438
	#define EEPROM_ADDRESS_DEBUG_1					EEPROM_ADDRESS_DATA_MAX
	#define EEPROM_ADDRESS_DEBUG_2					EEPROM_ADDRESS_DEBUG_1 + 4
	#define EEPROM_ADDRESS_DEBUG_3					EEPROM_ADDRESS_DEBUG_2 + 4
	#define EEPROM_ADDRESS_DEBUG_4					EEPROM_ADDRESS_DEBUG_3 + 4
	#define EEPROM_ADDRESS_H_MAX						EEPROM_ADDRESS_DEBUG_4 + 4
	#define EEPROM_ADDRESS_H_GND						EEPROM_ADDRESS_H_MAX + 4
	#define EEPROM_ADDRESS_APOGEE						EEPROM_ADDRESS_H_GND + 4
	#define EEPROM_ADDRESS_TRUE_TEMP_GND		EEPROM_ADDRESS_APOGEE + 4
	#define EEPROM_ADDRESS_TRUE_PRESS_LST		EEPROM_ADDRESS_TRUE_TEMP_GND + 4
	#define EEPROM_ADDRESS_TRUE_PRESS_HST		EEPROM_ADDRESS_TRUE_PRESS_LST + 4
	#define EEPROM_ADDRESS_LAST							EEPROM_ADDRESS_TRUE_PRESS_HST + 4

	// Same as default memory map
	#define EEPROM_ADDRESS_SAMPL_RATE_CHNG	EEPROM_ADDRESS_LAST + 2
	#define EEPROM_ADDRESS_GROUND_TEMP			EEPROM_ADDRESS_SAMPL_RATE_CHNG + 2
	#define EEPROM_ADDRESS_PRESS_APOGEE			EEPROM_ADDRESS_GROUND_TEMP + 2
	#define EEPROM_ADDRESS_PRESS_GROUND			EEPROM_ADDRESS_PRESS_APOGEE + 2
	#define EEPROM_ADDRESS_DIG_T1						EEPROM_ADDRESS_PRESS_GROUND + 2
	#define EEPROM_ADDRESS_DIG_T2						EEPROM_ADDRESS_DIG_T1 + 2
	#define EEPROM_ADDRESS_DIG_T3						EEPROM_ADDRESS_DIG_T2 + 2
	#define EEPROM_ADDRESS_DIG_P1						EEPROM_ADDRESS_DIG_T3 + 2
	#define EEPROM_ADDRESS_DIG_P2						EEPROM_ADDRESS_DIG_P1 + 2
	#define EEPROM_ADDRESS_DIG_P3						EEPROM_ADDRESS_DIG_P2 + 2
	#define EEPROM_ADDRESS_DIG_P4						EEPROM_ADDRESS_DIG_P3 + 2
	#define EEPROM_ADDRESS_DIG_P5						EEPROM_ADDRESS_DIG_P4 + 2
	#define EEPROM_ADDRESS_DIG_P6						EEPROM_ADDRESS_DIG_P5 + 2
	#define EEPROM_ADDRESS_DIG_P7						EEPROM_ADDRESS_DIG_P6 + 2
	#define EEPROM_ADDRESS_DIG_P8						EEPROM_ADDRESS_DIG_P7 + 2
	#define EEPROM_ADDRESS_DIG_P9						EEPROM_ADDRESS_DIG_P8 + 2

#else

	#define EEPROM_ADDRESS_DATA_MAX					480
	#define EEPROM_ADDRESS_SAMPL_RATE_CHNG	480
	#define EEPROM_ADDRESS_GROUND_TEMP			482
	#define EEPROM_ADDRESS_PRESS_APOGEE			484
	#define EEPROM_ADDRESS_PRESS_GROUND			486
	#define EEPROM_ADDRESS_DIG_T1						488
	#define EEPROM_ADDRESS_DIG_T2						EEPROM_ADDRESS_DIG_T1 + 2
	#define EEPROM_ADDRESS_DIG_T3						EEPROM_ADDRESS_DIG_T2 + 2
	#define EEPROM_ADDRESS_DIG_P1						EEPROM_ADDRESS_DIG_T3 + 2
	#define EEPROM_ADDRESS_DIG_P2						EEPROM_ADDRESS_DIG_P1 + 2
	#define EEPROM_ADDRESS_DIG_P3						EEPROM_ADDRESS_DIG_P2 + 2
	#define EEPROM_ADDRESS_DIG_P4						EEPROM_ADDRESS_DIG_P3 + 2
	#define EEPROM_ADDRESS_DIG_P5						EEPROM_ADDRESS_DIG_P4 + 2
	#define EEPROM_ADDRESS_DIG_P6						EEPROM_ADDRESS_DIG_P5 + 2
	#define EEPROM_ADDRESS_DIG_P7						EEPROM_ADDRESS_DIG_P6 + 2
	#define EEPROM_ADDRESS_DIG_P8						EEPROM_ADDRESS_DIG_P7 + 2
	#define EEPROM_ADDRESS_DIG_P9						EEPROM_ADDRESS_DIG_P8 + 2

#endif

uint16_t calib_data_temp[3], calib_data_press[9];
int32_t t_fine;

// Retorna a temperatura em ºC. Exemplo: valor de saída "5123" é igual a 51.23ºC
// t_fine carrega o valor no formato utilizado para compensação de pressão.
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
// Retorna pressão em Pa, formato unsigned int 4 bytes, 3 MSBytes = parte inteira, 1 LSByte = parte decimal.
// Exemplo: o valor de saída “24674867” representa 24674867/256 = 96386.2 Pa.
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
	FILE *in_data, *out_data, *flight_data_true, *flight_data_raw, *timestamps;
	int sample_count = 0;
	float temperature_true = 0.0, pressure_true = 0.0;
	unsigned int pressure_raw = 0, temperature_raw = 0;
	char sample_value_true[11], sample_value_raw[11];
	int rate_change_address = 0, rate_change_sample_index = 0;
	float timestamp = 0.0;

	in_data = fopen ("eeprom_contents.bin", "rb");
	if(!in_data)
	{
		printf("\n\n\n\t\t!!! ERRO NA LEITURA DO ALTIMETRO !!!\n\n");

		printf ("\tO arquivo binario com os dados do voo nao foi gerado!\n\n");

		printf("\tSugestoes:\n\n");
		printf("\t- Verifique a conexao entre altimetro e leitor\n");
		printf("\t- Verifique a conexao entre leitor e porta USB\n");
		printf("\t- Garanta que o driver do leitor foi instalado corretamente\n");
		printf("\t- Leia o Manual do Usuario\n\n\n");

		getch();
		return(1);
	}

	flight_data_true = fopen("flight_data_true.txt", "w");
	if(!flight_data_true)
	{
		printf ("\n\n\tErro na abertura do arquivo de saida de dados de voo!\n\n\n");
		getch();
		return(1);
	}

	flight_data_raw = fopen("flight_data_raw.txt", "w");
	if(!flight_data_raw)
	{
		printf ("\n\n\tErro na abertura do arquivo de saida de dados de voo brutos!\n\n\n");
		getch();
		return(1);
	}

	timestamps = fopen("timestamps.txt", "w");
	if(!timestamps)
	{
		printf ("\n\n\tErro na abertura do arquivo de marcações de tempo!\n\n\n");
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
	temperature_true = bmp280_compensate_temp((int32_t)temperature_raw<<4) / 100.0;

	// Calcular Pressao no solo
	fseek(in_data, EEPROM_ADDRESS_PRESS_GROUND, SEEK_SET);
	fread(&pressure_raw, 2, 1, in_data);
	pressure_true = bmp280_compensate_press((int32_t)pressure_raw <<4) / 256.0;
	sprintf(sample_value_true, "\n%.2f\n", pressure_true);
	sample_value_true[strlen(sample_value_true)-4] = ','; //Line commented = true values with dot instead of coma
	fprintf(flight_data_true, sample_value_true);

	// Calcular pressao no apogeu
	fseek(in_data, EEPROM_ADDRESS_PRESS_APOGEE, SEEK_SET);
	fread(&pressure_raw, 2, 1, in_data);
	pressure_true = bmp280_compensate_press((int32_t)pressure_raw <<4) / 256.0;
	sprintf(sample_value_true, "%.2f\n", pressure_true);
	sample_value_true[strlen(sample_value_true)-4] = ','; //Line commented = true values with dot instead of coma
	fprintf(flight_data_true, sample_value_true);

	fprintf(flight_data_true, "\n\nPressao\n");

	// Ler endereço que marca a primeira amostra a partir da mudança de taxa de amostragem
	fseek(in_data, EEPROM_ADDRESS_SAMPL_RATE_CHNG, SEEK_SET);
	fread(&rate_change_address, 2, 1, in_data);
	rate_change_sample_index = rate_change_address / 2;

	rewind(in_data);
	while(1)
	{
		fread(&pressure_raw, 2, 1, in_data);
		if((pressure_raw == 0xffff) || (sample_count == (EEPROM_ADDRESS_DATA_MAX / 2)))
		{
			printf("\n\nDownload dos dados de voo concluido!\n\n");
			break;
		}
		sprintf(sample_value_raw, "%d\n", pressure_raw);
		pressure_true = bmp280_compensate_press((int32_t)pressure_raw <<4) / 256.0;
		sprintf(sample_value_true, "%.2f\n", pressure_true);
		sample_value_true[strlen(sample_value_true)-4] = ','; //Line commented = true values with dot instead of coma
		fprintf(flight_data_true, sample_value_true);
		fprintf(flight_data_raw, sample_value_raw);

		if(sample_count >= rate_change_sample_index)
		{
			// If the altimeter detects a descent with parachutes and no NVM space enough to record the whole descent,
			// it will reduce the sampling rate to 1 Hz (one sample per second).
			timestamp += 1.000;
		}
		else
		{
			// The normal sampling rate of the altimeter is 5.2 Hz (one sample per 0.192 second).
			timestamp += 0.192;
		}
		fprintf(timestamps, "%.3f\n", timestamp);

		sample_count++;
	}



	fclose(in_data);
	fclose(flight_data_true);
	fclose(flight_data_raw);
	fclose(timestamps);

	return 0;
}
