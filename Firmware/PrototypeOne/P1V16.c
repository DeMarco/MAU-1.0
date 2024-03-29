//#define	TEST_MODE
//#define	FIXED_TEMPERATURE

#include "definesV16.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <math.h>
#include <avr/pgmspace.h>

#include "AtmosModel_100_v3.h"

volatile struct
{
	uint8_t readout_cycle_state: 3;
}
int_flags;

volatile struct
{
	uint8_t rawdata_readout: 1;
	uint8_t rawdata_record: 1;
	uint8_t infinite_readout_cycles: 1;
}
en_flags;

uint8_t rawdata_readout_cycles_number = 0;
uint16_t calib_data_temp[3], calib_data_press[9];
int32_t t_fine;
uint16_t *array_window_landing = 0, *array_window_ascension = 0, *array_window_ground = 0;


// FUNÇÕES AUXILIARES //

void delay_1s (void)
{
	uint8_t i;

	for (i = 0; i < 10; i++)
		_delay_ms(100);
}

void delay_60s (void)
{
	uint8_t i;

	for (i = 0; i < CYCLE_START_DELAY; i++)
		delay_1s();
}

void LED_sign (void)
{
	output_low(LED);
	_delay_ms(500);
	output_high(LED);
	_delay_ms(100);
	output_low(LED);
	_delay_ms(100);
	output_high(LED);
	_delay_ms(100);
	output_low(LED);
	_delay_ms(100);
	output_high(LED);
	_delay_ms(100);
	output_low(LED);
	delay_1s();
}

void LED_number (uint8_t number)
{
	uint8_t i;

	if(number == 0)
	{
		output_low(LED);
		_delay_ms(500);
		output_high(LED);
		_delay_ms(750);
		output_low(LED);
	}
	else
		for(i=0; i<number; i++)
		{
			output_low(LED);
			_delay_ms(250);
			output_high(LED);
			_delay_ms(250);
		}
	output_low(LED);
}

void LED_data (int32_t data)
{
	uint8_t count = 0, *digit;
	uint32_t n;
	int8_t i;
	
	if(data == 0)
	{
		LED_number(0);	
	}
	else
	{
		if(data < 0)
		{
			LED_sign();
			data = abs(data);
		}
		for(n = data; n > 0; n /= 10)
		{
			count++;
		}
		digit = malloc(count);
		if(digit)
		{
			for(i=count-1; i >= 0; i--)
			{
				digit[i] = data % 10;
				data /= 10;
			}
			for(i=0; i<count; i++)
			{
				LED_number(digit[i]);
				delay_1s();
			}
			free(digit);
		}
	}
}

uint8_t SPI_transfer_byte(uint8_t byte_out)
{
    uint8_t byte_in = 0;

	USIDR = byte_out;
	USISR = _BV(USIOIF);
	
	do
	{
		USICR |= _BV(USITC);
	}
	while((USISR & _BV(USIOIF)) == 0);
	
	byte_in = USIDR;

    return byte_in;
}

void SPI_write (uint8_t address, uint8_t data)
{
	//Baixar pino CSB (chip select)
	PORTB &= ~_BV(CSB);
	//Enviar endereco
	SPI_transfer_byte(address & WRITE);
	//Enviar dado
	SPI_transfer_byte(data);
	//Subir pino CSB
	PORTB |= _BV(CSB);	
}

uint8_t SPI_read_byte (uint8_t address)
{
	uint8_t data = 0;
	
	//Baixar pino CSB (chip select)
	PORTB &= ~_BV(CSB);
	//Enviar endereco
	SPI_transfer_byte(address | READ);
	//Receber dado
	data = SPI_transfer_byte(BLANK);
	//Subir pino CSB
	PORTB |= _BV(CSB);
	
	return data;
}

uint32_t SPI_read_rawdata (uint8_t address) //O tipo de retorno pode ser uint16_t se o modo Ultra Low Power for utilizado
{
	uint8_t data[3];
	
	//Baixar pino CSB (chip select)
	output_low(CSB);
	
	//Enviar endereco
	SPI_transfer_byte(address | READ);
	
	//Receber dados
	data[2] = SPI_transfer_byte(BLANK);
	data[1] = SPI_transfer_byte(BLANK);
	data[0] = SPI_transfer_byte(BLANK);
	
	//Subir pino CSB
	output_high(CSB);
	
	// Retorna raw data justificado à direita de acordo com o modo de amostragem:
	return ((data[2]<<8) + data[1]); // Ultra Low Power
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 7); // Low Power
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 6); // Standard Resolution
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 5); // High Resolution
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 4); // Ultra High Resolution
}

uint16_t SPI_read_calib_word (uint8_t calib_address)
{
	uint16_t calib_data = 0;
	
	//Baixar pino CSB (chip select)
	output_low(CSB);
	
	//Enviar endereco
	SPI_transfer_byte(calib_address | READ);
	
	//Receber dados
	calib_data = SPI_transfer_byte(BLANK);
	calib_data += (SPI_transfer_byte(BLANK) << 8);
	
	//Subir pino CSB
	output_high(CSB);
	
	return calib_data;
}

void SPI_read_calib_data(void)
{
	uint8_t i;
	uint8_t address = BMP280_REG_DIG_T1;
	
	for(i=0; i<3; i++)
	{
		calib_data_temp[i] = SPI_read_calib_word(address);
		address += 2;
	}
	for(i=0; i<9; i++)
	{
		calib_data_press[i] = SPI_read_calib_word(address);
		address += 2;
	}	
}

void EEPROM_write_byte(uint16_t address, uint8_t data)
{
	// Aguardar término da escrita anterior
	while(EECR & _BV(EEPE));

	// Ativar modo de programação da EEPROM
	EECR &= (~_BV(EEPM1) & ~_BV(EEPM0));

	// Preparar registradores de endereço e dado
	EEARL = address - 256;
	EEARH = address / 256;
	EEDR = data;

	// Escrever "1" em EEMPE
	EECR |= _BV(EEMPE);

	// Iniciar escrita em EEPROM ativando EEPE
	EECR |= _BV(EEPE);
}

void EEPROM_write_word(uint16_t address, uint16_t data)
{
	EEPROM_write_byte(address, data);
	EEPROM_write_byte(address + 1, data >> 8);
}

void EEPROM_write_dword(uint16_t address, uint32_t data)
{
	EEPROM_write_word(address, data);
	EEPROM_write_word(address + 2, data >> 16);
}

uint8_t EEPROM_read_byte(uint16_t address)
{
	// Aguardar término da escrita anterior
	while(EECR & _BV(EEPE));

	// Preparar registrador de endereço
	EEARL = address - 256;
	EEARH = address / 256;

	// Iniciar leitura da EEPROM ativando EERE
	EECR |= _BV(EERE);

	// Retornar dado armazenado no registrador de dado
	return EEDR;
}

uint16_t EEPROM_read_word(uint16_t address)
{
	uint16_t data = 0;
	
	data = EEPROM_read_byte(address);
	data += EEPROM_read_byte(address + 1) * 256;
	
	return data;
}

void EEPROM_store_calib_data(void)
{
	uint8_t i;
	uint16_t address = EEPROM_ADDRESS_DIG_T1;
	
	for(i=0; i<3; i++)
	{
		EEPROM_write_word(address, calib_data_temp[i]);
		address += 2;
	}
	for(i=0; i<9; i++)
	{
		EEPROM_write_word(address, calib_data_press[i]);
		address += 2;
	}
}

// Retorna a temperatura em ºC. Exemplo: valor de saída "5123" é igual a 51.23ºC
// t_fine carrega o valor no formato utilizado para compensação de pressão.
int32_t bmp280_compensate_temp(int32_t t_raw)
{
	int32_t T;
	#ifndef FIXED_TEMPERATURE
	uint16_t dig_T1;
	int16_t  dig_T2, dig_T3;
	int32_t var1, var2;
		
	dig_T1 = calib_data_temp[0];
	dig_T2 = calib_data_temp[1];
	dig_T3 = calib_data_temp[2];
	
	var1 = ((((t_raw>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((t_raw>>4) - ((int32_t)dig_T1)) * ((t_raw>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	#else
	t_fine = GROUND_FIXED_TEMP_FINE;
	T = GROUND_FIXED_TEMP*100;
	#endif
	
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

void report_apogee (void)
{
	#ifdef TEST_MODE
	int32_t temperature_true = 0;
	#endif
	uint32_t pressure_raw = 0, temperature_raw = 0;
	int32_t pressure_true_apogee = 0, pressure_true_ground = 0;
	float height_array_tmp_prev = 0;
	double height_ground = 0.0, height_apogee = 0.0, coef = 0.0;
	int32_t press_breakpoint = 0, press_breakpoint_previous = 0, press_breakpoint_step = 0;
	uint16_t i;
	
	delay_1s();
	
	// Checar se EEPROM não está vazia
	temperature_raw = EEPROM_read_word(EEPROM_ADDRESS_GROUND_TEMP);
	if(temperature_raw != 0xFFFF)
	{
		
		#ifdef TEST_MODE
		temperature_true = bmp280_compensate_temp((int32_t)temperature_raw<<4);
		EEPROM_write_dword(EEPROM_ADDRESS_TRUE_TEMP_GND, temperature_true);
		#else
		bmp280_compensate_temp((int32_t)temperature_raw<<4);
		#endif
		
		// Calcular pressão no apogeu do último vôo
		pressure_raw = EEPROM_read_word(EEPROM_ADDRESS_PRESS_APOGEE);
		pressure_true_apogee = bmp280_compensate_press((int32_t)pressure_raw <<4);
		#ifdef TEST_MODE
		EEPROM_write_dword(EEPROM_ADDRESS_TRUE_PRESS_LST, pressure_true_apogee);
		#endif

		// Calcular pressão no sólo do último vôo
		pressure_raw = EEPROM_read_word(EEPROM_ADDRESS_PRESS_GROUND);
		pressure_true_ground = bmp280_compensate_press((int32_t)pressure_raw <<4);
		#ifdef TEST_MODE
		EEPROM_write_dword(EEPROM_ADDRESS_TRUE_PRESS_HST, pressure_true_ground);
		#endif
		
		// Adaptar breakpoints do modelo atmosférico para fixed point
		press_breakpoint = SENSOR_MAX_PRESSURE;
		press_breakpoint <<= 8;
		press_breakpoint_step = BREAKPOINTS_STEP;
		press_breakpoint_step <<= 8;
		
		for(i=1; i < BREAKPOINTS_NUMBER; i++)
		{
			press_breakpoint_previous = press_breakpoint;
			press_breakpoint -= press_breakpoint_step;
			
			height_array_tmp_prev = pgm_read_float(&height_array[i-1]);
			coef = (pgm_read_float(&height_array[i]) - height_array_tmp_prev) / ((press_breakpoint - press_breakpoint_previous)>>8);
			
			// Calcular altitude no  solo
			if((pressure_true_ground <= press_breakpoint_previous) && (pressure_true_ground >= press_breakpoint))
			{
				height_ground = coef * ((pressure_true_ground - press_breakpoint_previous)>>8) + height_array_tmp_prev;
				
				#ifdef TEST_MODE
				EEPROM_write_dword(EEPROM_ADDRESS_H_GND, (uint32_t)height_ground);
				#endif
			}
			
			// Calcular altitude no apogeu
			if((pressure_true_apogee <= press_breakpoint_previous) && (pressure_true_apogee >= press_breakpoint))
			{	
				height_apogee = coef * ((pressure_true_apogee - press_breakpoint_previous)>>8) + height_array_tmp_prev;
				
				#ifdef TEST_MODE
				EEPROM_write_dword(EEPROM_ADDRESS_H_MAX, (uint32_t)height_apogee);
				#endif
			}
		}
		
		#ifdef TEST_MODE	
		EEPROM_write_dword(EEPROM_ADDRESS_APOGEE, height_apogee - height_ground);
		#endif
		
		// Reportar apogeu em decímetros
		LED_data((int32_t)((height_apogee - height_ground)*10));
		
	}
}

void bmp280_init (void)
{
	// Configurar BMP280
	SPI_write(CONFIG, BMP280_SETUP_CONFIG);
	SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL);
	
	// Ler valores de calibração da memória do BMP280
	SPI_read_calib_data();
}

void ioinit (void)
{
    //Ativar TIMER 1 com clock principal do sistema
	TCCR1 = _BV(CS10);

	//Configurar pinos DO, USCK, LED e CSB como Saída, e DI como entrada
	DDRB = (_BV(DO) | _BV(USCK) | _BV(LED) | _BV(CSB)) & ~_BV(DI);
	
	//Configurar Universal Serial Interface para SPI (Three Wire mode) e modo SPI 00;
	output_low(CSB); //Iniciar com CSB = 0 para BMP280 se fixar em modo SPI;
	_delay_ms(10);
	output_high(CSB); //Mantém CSB = 1 (condição para início de novas transmissões);
	output_low(USCK); //Mantém UCSK = 0 (assim o BMP280 entende como SPI modo 00)
	USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
	
	//Habilitar interrupção por overflow do TIMER 1
    TIMSK = _BV(TOIE1);
}

ISR(TIMER1_OVF_vect)
{
	static uint16_t scaler = POST_SCALER_MEASUREMENT + 1;
	static uint8_t cycle_count = 0;

	if(en_flags.rawdata_readout)
	{
		switch(--scaler)
		{
			case POST_SCALER_MEASUREMENT:
				int_flags.readout_cycle_state = MEASURE;
				break;
			case POST_SCALER_READOUT:
				int_flags.readout_cycle_state = READ_OUT;
				break;
			case POST_SCALER_WRITE_EEPROM:
				int_flags.readout_cycle_state = STORE_NVM;
				break;
			case POST_SCALER_PROCESS:
				int_flags.readout_cycle_state = PROCESS;
				break;
			case POST_SCALER_CONTROL_LED:
				int_flags.readout_cycle_state = CONTROL_LED;
				break;
			case 0:
				output_low(LED);
				if(++cycle_count == rawdata_readout_cycles_number)
				{
					cycle_count = 0;
					if(!en_flags.infinite_readout_cycles)
					{
						int_flags.readout_cycle_state = CYCLES_FULL;
						en_flags.rawdata_readout = FALSE;	
					}
				}
				scaler = POST_SCALER_MEASUREMENT + 1;
				break;
		}
	}	
}

void rawdata_readout_cycle (uint16_t *eeprom_address, uint8_t process_type)
{
	uint32_t pressure_raw = 0;
	uint8_t count = 0;
	uint8_t i = 0;
	uint8_t cycle_count_LED = 0;
	uint32_t numerator = 0;
	uint32_t numerator_ground = 0;
	int32_t average = 0;
	int32_t average_ground = 0;
	int32_t variance = 0;
	int32_t term = 0;
	
	switch(process_type)
	{
		case DETECT_ASCENSION:
			array_window_ascension = malloc(2*BMP280_CYCLES_NUM_ASCENSION);
			array_window_ground = malloc(2*(BMP280_CYCLES_NUM_GROUND + BMP280_CYCLES_NUM_ASCENSION));
			break;
			
		case DETECT_LANDING:
			array_window_landing = malloc(2*BMP280_CYCLES_NUM_LANDED);
			break;
	}
	
	while(en_flags.rawdata_readout)
	{
		switch(int_flags.readout_cycle_state)
		{
			case MEASURE:
				SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL | BMP280_MODE_FORCED);
				int_flags.readout_cycle_state = DO_NOTHING;
				break;
				
			case READ_OUT:
				pressure_raw = SPI_read_rawdata(PRESSURE);
				int_flags.readout_cycle_state = DO_NOTHING;
				break;
				
			case STORE_NVM:
				if(en_flags.rawdata_record)
				{
					if(*eeprom_address >= EEPROM_ADDRESS_DATA_MAX)
						//*eeprom_address = 0;
						en_flags.rawdata_readout = FALSE;
					else
					{
						cli();
						EEPROM_write_word(*eeprom_address, pressure_raw);
						sei();
						*eeprom_address += 2;
					}
				}
				int_flags.readout_cycle_state = DO_NOTHING;
				break;
			
			case PROCESS:
				switch(process_type)
				{
					case NO_PROCESSING:
						break;
						
					case DETECT_ASCENSION:
						if(count < (BMP280_CYCLES_NUM_GROUND + BMP280_CYCLES_NUM_ASCENSION))
						{
							array_window_ground[count] = pressure_raw;
							if(count >= BMP280_CYCLES_NUM_GROUND)
							{
								array_window_ascension[count - BMP280_CYCLES_NUM_GROUND] = pressure_raw;
							}
							count++;
						}
						else
						{
							//CÁLCULO DA PRESSÃO MÉDIA DE SOLO
							numerator_ground = 0;
							for(i = 1; i < (BMP280_CYCLES_NUM_GROUND + BMP280_CYCLES_NUM_ASCENSION); i++)
							{
								array_window_ground[i-1] = array_window_ground[i];
								if(i <= BMP280_CYCLES_NUM_GROUND)
									numerator_ground += array_window_ground[i-1];
							}
							array_window_ground[i-1] = pressure_raw;
							average_ground = numerator_ground / BMP280_CYCLES_NUM_GROUND;
							
							//CÁLCULO DA VARIÂNCIA PARA DETECÇÃO DE ASCENÇÃO
							for(i = 1; i < BMP280_CYCLES_NUM_ASCENSION; i++)
							{
								array_window_ascension[i-1] = array_window_ascension[i];
								numerator += array_window_ascension[i];
							}
							array_window_ascension[i-1] = pressure_raw;
							numerator += pressure_raw;
							average = numerator / BMP280_CYCLES_NUM_ASCENSION;
							for(i = 0; i < BMP280_CYCLES_NUM_ASCENSION; i++)
							{
								term = array_window_ascension[i] - average;
								variance += term*term;
							}
							variance /= BMP280_CYCLES_NUM_ASCENSION;
							
							//SE ASCENÇÃO DETECTADA:
							if(variance > MIN_VARIANCE_ASCENSION_DETECT)
							{
								en_flags.rawdata_readout = FALSE;
								
								// Registrar pressão de solo em memória não-volátil
								cli();
								EEPROM_write_word(EEPROM_ADDRESS_PRESS_GROUND, average_ground);
								sei();
								
								array_window_ascension[0] = average_ground; //????????
								
								#ifdef TEST_MODE
								cli();
								EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_1, variance);
								sei();
								#endif
							}
							numerator = 0;
							average = 0;
							variance = 0;
						}
						break;
						
					case DETECT_LANDING:
						if(count < BMP280_CYCLES_NUM_LANDED)
						{
							numerator += pressure_raw;
							array_window_landing[count] = pressure_raw;
							count++;
						}
						else
						{							
							for(i=1; i < BMP280_CYCLES_NUM_LANDED; i++)
							{
								array_window_landing[i-1] = array_window_landing[i];
								numerator += array_window_landing[i];
							}
							array_window_landing[i-1] = pressure_raw;
							numerator += pressure_raw;
							average = numerator / BMP280_CYCLES_NUM_LANDED;
							for(i = 0; i < BMP280_CYCLES_NUM_LANDED; i++)
							{
								term = array_window_landing[i] - average;
								variance += term*term;
							}
							variance /= BMP280_CYCLES_NUM_LANDED;
							if((variance > 0) && (variance < MAX_VARIANCE_LANDING_DETECT))
							{
								en_flags.rawdata_readout = FALSE;
								
								#ifdef TEST_MODE
								cli();
								EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_2, variance);
								sei();
								#endif
							}
							numerator = 0;
							average = 0;
							variance = 0;
							
						}
						break;
				}
				int_flags.readout_cycle_state = DO_NOTHING;
				break;
			
			case CONTROL_LED:
				switch(process_type)
				{
					case DETECT_LANDING:
						output_high(LED);
						break;

					case DETECT_ASCENSION:
						if(count > BMP280_CYCLES_NUM_GROUND)
						{
							if(++cycle_count_LED == 5)
							{
								output_high(LED);
								cycle_count_LED = 0;
							}
						}
						break;

					case MEASURE_GROUND_PRESSURE:
						if(++cycle_count_LED == 2)
						{
							output_high(LED);
							cycle_count_LED = 0;
						}
						break;

					case NO_PROCESSING:
						output_high(LED);
						break;
				}
				int_flags.readout_cycle_state = DO_NOTHING;
				break;
		}
	}
}

//////////////////////////////////////////////////////////////////
//	FUNÇÃO MAIN
//////////////////////////////////////////////////////////////////
int main (void)
{
///////////////////// EXECUTAR INICIALIZAÇõES /////////////////////////////////////////////////////////////////

	// Dados de pressão e temperature
	uint32_t temperature_raw = 0;
	uint16_t raw_press_highest = 0;
	
	// EEPROM
	uint16_t eeprom_initial_adr = 0;
	uint16_t eeprom_final_adr = 0;
	uint16_t eeprom_adr = 0;
	uint16_t eeprom_data = 0;	
	
	// Inicialização das flags
	int_flags.readout_cycle_state = DO_NOTHING;
	en_flags.rawdata_readout = FALSE;
	en_flags.rawdata_record = FALSE;
	
	// Misc
	uint8_t i = 0;
	
	ioinit();
	bmp280_init();
	
	//LED_sign(); ///causa muita confusão nos usuários

///////////////////// REPORTAR APOGEU DO VÔO ANTERIOR /////////////////////////////////////////////////////////////////

	report_apogee();

///////////////////// AGUARDAR 60 SEGUNDOS ////////////////////////////////////////////////////////////////////////////
	
	delay_60s();

///////////////////// FLUXO PRINCIPAL /////////////////////////////////////////////////////////////////////////////////

	// Registrar temperatura no solo
	SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL | BMP280_MODE_FORCED);
	_delay_ms(10);
	temperature_raw = SPI_read_rawdata(TEMPERATURE);
	EEPROM_write_word(EEPROM_ADDRESS_GROUND_TEMP, temperature_raw);
	
	// Posicionar escrita em memória no endereco zero
	eeprom_adr = eeprom_initial_adr;
	
	// Habilitar interrupções globais
	sei();
	
	// Detectar ascensão, determinar e registrar pressão de solo em memória não volátil
	rawdata_readout_cycles_number = BMP280_CYCLES_NUM_GROUND + BMP280_CYCLES_NUM_ASCENSION;
	en_flags.infinite_readout_cycles = TRUE;
	en_flags.rawdata_readout = TRUE;
	//en_flags.rawdata_record = TRUE; //DELETAR DEPOIS
	rawdata_readout_cycle(&eeprom_adr, DETECT_ASCENSION);
	
	// Armazenar dados de vôo e detectar pouso
	SPI_write(CONFIG, BMP280_FILTER_DISABLED);
	rawdata_readout_cycles_number = BMP280_MAX_CYCLE_COUNT;
	en_flags.rawdata_record = TRUE;
	en_flags.infinite_readout_cycles = FALSE;
	en_flags.rawdata_readout = TRUE;
	eeprom_adr += (BMP280_CYCLES_NUM_ASCENSION * 2);
	rawdata_readout_cycle(&eeprom_adr, DETECT_LANDING);
	free(array_window_landing);
	
	cli();
	TCCR1 &= ~_BV(CS10); // Desativar Timer1
		
	eeprom_final_adr = eeprom_adr;
	#ifdef TEST_MODE
	EEPROM_write_word(EEPROM_ADDRESS_LAST, eeprom_final_adr);
	#endif
	
	// Salvar em memória amostras da janela de detecção de ascensão
	eeprom_adr = eeprom_initial_adr;
	for(i = 0; i < BMP280_CYCLES_NUM_ASCENSION; i++)
	{
		EEPROM_write_word(eeprom_adr, array_window_ascension[i]);
		eeprom_adr += 2;
	}
	
	free(array_window_ascension);
	free(array_window_ground);
	
	// Determinar pressão no apogeu e gravar em memória
	for(eeprom_adr = eeprom_initial_adr; eeprom_adr < eeprom_final_adr; eeprom_adr += 2)
	{
		eeprom_data = EEPROM_read_word(eeprom_adr);
		if(eeprom_data > raw_press_highest)
			raw_press_highest = eeprom_data;
	}
	EEPROM_write_word(EEPROM_ADDRESS_PRESS_APOGEE, raw_press_highest);
	
	// Apagar memória não utilizada durante o vôo
	for(eeprom_adr = eeprom_final_adr; eeprom_adr < EEPROM_ADDRESS_DATA_MAX; eeprom_adr += 2)
		EEPROM_write_word(eeprom_adr, 0xffff);
		
///////////////////// REPORTAR APOGEU DO VÔO ANTERIOR /////////////////////////////////////////////////////////////////
	
	//report_apogee(); ///depois que pousa não precisa, só gasta mais bateria

	// Armazenar dados de clibração em memória
	EEPROM_store_calib_data();
	
///////////////////// AUTO POWER DOWN /////////////////////////////////////////////////////////////////
	
	LED_sign();
	
	cli();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();

    return(0);
}