/*
 * PrototypeOne.c
 * Version4
 *
 * Created: 24/07/2016 23:00:00
 * Author : DRAYCON
 */ 

 /*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 */

#include "definesV4.h"
 
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
//#include <avr/eeprom.h>

/*typedef uint32_t uint32_t;
typedef int32_t	int32_t;
typedef int64_t	int64_t;*/

volatile struct
{
	uint8_t measurement_go: 1;
	uint8_t readout_go: 1;
	uint8_t write_eeprom_go: 1;
	uint8_t readout_cycles_full: 1;
}
int_flags;

volatile struct
{
	uint8_t rawdata_readout: 1;
}
en_flags;

int32_t t_fine;

/* BMP280 related */
uint16_t dig_T1, dig_P1;
int16_t  dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

// FUNÇÕES //

void delay_1s (void)
{
	uint8_t i;

	for (i = 0; i < 100; i++)
		_delay_ms(10);
}

void delay_60s (void)
{
	uint8_t i;

	for (i = 0; i < 60; i++)
		delay_1s();
}

void LED_sign (void)
{
	output_low(LED);
	_delay_ms(500);
	output_high(LED);
	_delay_ms(250);
	output_low(LED);
	_delay_ms(250);
	output_high(LED);
	_delay_ms(250);
	output_low(LED);
	_delay_ms(250);
	output_high(LED);
	_delay_ms(250);
	output_low(LED);
	delay_1s();
}

void LED_sign2 (void)
{
	output_low(LED);
	_delay_ms(500);
	output_high(LED);
	_delay_ms(250);
	output_low(LED);
	_delay_ms(250);
	output_high(LED);
	_delay_ms(250);
	output_low(LED);
	delay_1s();
}

void LED_sign4s (void)
{
	output_low(LED);
	delay_1s();
	output_high(LED);
	_delay_ms(50);
	output_low(LED);
	_delay_ms(950);
	output_high(LED);
	_delay_ms(50);
	output_low(LED);
	_delay_ms(950);
	output_high(LED);
	_delay_ms(50);
	output_low(LED);
	_delay_ms(950);
	output_high(LED);
	_delay_ms(50);
	output_low(LED);
	_delay_ms(950);
}

void LED_flash (uint8_t data)
{
	uint8_t i;
	
	LED_sign();
	
	for(i=0x80; i; i>>=1)
	{
		output_high(LED);
		_delay_ms(50);
		output_low(LED);
		_delay_ms(450);
		if((data & i) == 0)
			output_low(LED);
		else
			output_high(LED);
		delay_1s();
		output_low(LED);
		_delay_ms(500);
	}	
}

void LED_number (uint8_t number)
{
	uint8_t i;

	for(i=0; i<number; i++)
	{
		output_low(LED);
		_delay_ms(500);
		output_high(LED);
		_delay_ms(500);
	}
	output_low(LED);
}

void LED_data (uint32_t data)
{
	uint8_t count = 0, *digit;
	uint32_t n;
	int8_t i;
	
	for(n=data; n > 0; n /= 10)
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

uint8_t SPI_transfer_byte(uint8_t byte_out)
{
    uint8_t byte_in = 0;

	USIDR = byte_out;
	USISR = _BV(USIOIF);
	
	do
	{
		//delay_1s();
		//_delay_ms(10);
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

uint8_t *SPI_read_rawdata (uint8_t address, uint8_t num_bytes)
{
	int8_t i;
	uint8_t *data = malloc(num_bytes);
	if(!data)
		return 0;
	
	//Baixar pino CSB (chip select)
	PORTB &= ~_BV(CSB);
	
	//Enviar endereco
	SPI_transfer_byte(address | READ);
	
	//Receber dados
	i = num_bytes;
	do
	{
		data[i-1] = SPI_transfer_byte(BLANK);
		i--;
	}
	while(i);
	
	//Subir pino CSB
	PORTB |= _BV(CSB);
	
	return data;
}

uint16_t SPI_read_calib_word (uint8_t calib_address)
{
	uint16_t calib_data = 0;
	//uint16_t i;
	
	//Baixar pino CSB (chip select)
	PORTB &= ~_BV(CSB);
	
	//Enviar endereco
	SPI_transfer_byte(calib_address | READ);
	
	//Receber dados
	calib_data = SPI_transfer_byte(BLANK);
	calib_data += SPI_transfer_byte(BLANK)*256;
	
	//Subir pino CSB
	PORTB |= _BV(CSB);
	
	return calib_data;
}

void EEPROM_write_byte(uint16_t address, uint8_t data)
{
	/* Wait for completion of previous write */
	while(EECR & _BV(EEPE));

	/* Set Programming mode */
	EECR &= (~_BV(EEPM1) & ~_BV(EEPM0));

	/* Set up address and data registers */
	EEARL = address - 256;
	EEARH = address / 256;
	EEDR = data;

	/* Write logical one to EEMPE */
	EECR |= _BV(EEMPE);

	/* Start eeprom write by setting EEPE */
	EECR |= _BV(EEPE);
}

void EEPROM_write_word(uint16_t address, uint16_t data)
{
	EEPROM_write_byte(address, data);
	EEPROM_write_byte(address + 1, data / 256);
}

uint8_t EEPROM_read_byte(uint16_t address)
{
	/* Wait for completion of previous write */
	while(EECR & _BV(EEPE));

	/* Set up address register */
	EEARL = address - 256;
	EEARH = address / 256;

	/* Start eeprom read by writing EERE */
	EECR |= _BV(EERE);

	/* Return data from data register */
	return EEDR;
}

uint16_t EEPROM_read_word(uint16_t address)
{
	uint16_t data = 0;
	
	data = EEPROM_read_byte(address);
	data += EEPROM_read_byte(address + 1) * 256;
	
	return data;
}

uint32_t adjust_data_size(uint32_t data, uint8_t data_type)
{
	if(data_type == PRESSURE)
	{
		switch(BMP280_SETUP_CTRL & BMP280_OSRS_P)
		{
			case BMP280_PRESS_SKIP:
				data = 0;
				break;
			case BMP280_PRESS_ULP:
				data >>= 8;
				break;
			case BMP280_PRESS_LP:
				data >>= 7;
				break;
			case BMP280_PRESS_SR:
				data >>= 6;
				break;
			case BMP280_PRESS_HR:
				data >>= 5;
				break;
			case BMP280_PRESS_UHR:
				data >>= 4;
				break;
		}
	}
	else if(data_type == TEMPERATURE)
	{
		switch(BMP280_SETUP_CTRL & BMP280_OSRS_T)
		{
			case BMP280_TEMP_SKIP:
				data = 0;
				break;
			case BMP280_TEMP_ULP:
				data >>= 8;
				break;
			case BMP280_TEMP_LP:
				data >>= 7;
				break;
			case BMP280_TEMP_SR:
				data >>= 6;
				break;
			case BMP280_TEMP_HR:
				data >>= 5;
				break;
			case BMP280_TEMP_UHR:
				data >>= 4;
				break;
		}
	}
	return data;
}

void extract_rawdata(uint8_t *rawdata, uint32_t *pressure_raw, uint32_t *temperature_raw)
{
	*pressure_raw = adjust_data_size((rawdata[5]*65536 + rawdata[4]*256 + rawdata[3]), PRESSURE);
	*temperature_raw = adjust_data_size((rawdata[2]*65536 + rawdata[1]*256 + rawdata[0]), TEMPERATURE);
}


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t bmp280_compensate_temp(int32_t t_raw)
{
	int32_t var1, var2, T;
	
	var1 = ((((t_raw>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((t_raw>>4) - ((int32_t)dig_T1)) * ((t_raw>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t bmp280_compensate_press(int32_t p_raw)
{
	int64_t var1, var2, p;
	
	var1 = ((int64_t)t_fine)-128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-p_raw;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}

void bmp280_init (void)
{
	//uint8_t bmp280_adr, bmp280_calib_data;
	
	SPI_write(CONFIG, BMP280_SETUP_CONFIG);
	SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL);
}

void ioinit (void)
{
    //Ativar TIMER 1 com clock principal do sistema
	TCCR1 = _BV(CS10);

	//Configurar pinos DO, USCK, LED e CSB como Saída, e DI como entrada
	DDRB = (_BV(DO) | _BV(USCK) | _BV(LED) | _BV(CSB)) & ~_BV(DI);
	
	//Configurar Universal Serial Interface para SPI (Three Wire mode) e modo SPI 00;
	//PORTB &= ~_BV(CSB); //Iniciar com CSB = 0 para BMP280 se fixar em modo SPI;
	output_low(CSB);
	_delay_ms(10);
	//PORTB |= _BV(CSB); //Mantém CSB = 1 (condição para início de novas transmissões);
	output_high(CSB);
	//PORTB &= ~_BV(USCK); //Mantém UCSK = 0 (assim o BMP280 entende como SPI modo 00)
	output_low(USCK);
	USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
	
	//Habilitar interrupção por overflow do TIMER 1
    TIMSK = _BV(TOIE1);
	
	//Habilitar interrupções globais
    //sei();
}

ISR(TIMER1_OVF_vect)
{
	static uint16_t scaler = POST_SCALER_MEASUREMENT;
	static uint8_t cycle_count = 0;

	if(en_flags.rawdata_readout)
	{
		if(scaler == POST_SCALER_MEASUREMENT)
		{
			int_flags.measurement_go = TRUE;
		}
		else if(scaler == POST_SCALER_READOUT)
		{
			int_flags.readout_go = TRUE;
		}
		else if(scaler == POST_SCALER_WRITE_EEPROM)
		{
			int_flags.write_eeprom_go = TRUE;
		}		
		else if(scaler == POST_SCALER_LED_ON)
		{
			output_high(LED);
		}
		else if(scaler == POST_SCALER_LED_OFF)
		{
			output_low(LED);
		}
		else if(scaler == 0)
		{
			if(++cycle_count >= BMP280_MAX_CYCLE_COUNT)
			{
				int_flags.readout_cycles_full = TRUE;
				en_flags.rawdata_readout = FALSE;
			}
			scaler = POST_SCALER_MEASUREMENT + 1;
		}
		scaler--;
	}
}

/*/////////////////////////////////////////////////////////////////
	MAIN
*//////////////////////////////////////////////////////////////////
int main (void)
{
	/* Pressure and temperature data */
	uint8_t *rawdata;
	uint32_t pressure_raw = 0, temperature_raw = 0;
	//int32_t press_raw = 0, temp_raw = 0, temp = 0;
	int32_t pressure_true = 0, temperature_true = 0;
	
	/* BMP280 related */
	//uint8_t bmp280_adr = 0;
	//uint16_t dig_T1, dig_P1;
	//int16_t  dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

	/* EEPROM related */
	uint16_t eeprom_initial_adr = 0;
	uint16_t eeprom_final_adr = 0;
	uint16_t eeprom_adr = 0;
	uint16_t eeprom_data = 0;
	uint16_t eeprom_data_highest = 0;
	uint16_t eeprom_data_lowest = 0;

	/* Soft flags initialization */
	int_flags.measurement_go = FALSE;
	int_flags.readout_go = FALSE;
	int_flags.write_eeprom_go = FALSE;
	int_flags.readout_cycles_full = FALSE;
	en_flags.rawdata_readout = FALSE;
	
	ioinit();
	bmp280_init();
	
	LED_sign();
	
	/* Retrieve calibration values from BMP280 NVM */
	dig_T1 = SPI_read_calib_word(BMP280_REG_DIG_T1);
	dig_T2 = SPI_read_calib_word(BMP280_REG_DIG_T2);
	dig_T3 = SPI_read_calib_word(BMP280_REG_DIG_T3);
	dig_P1 = SPI_read_calib_word(BMP280_REG_DIG_P1);
	dig_P2 = SPI_read_calib_word(BMP280_REG_DIG_P2);
	dig_P3 = SPI_read_calib_word(BMP280_REG_DIG_P3);
	dig_P4 = SPI_read_calib_word(BMP280_REG_DIG_P4);
	dig_P5 = SPI_read_calib_word(BMP280_REG_DIG_P5);
	dig_P6 = SPI_read_calib_word(BMP280_REG_DIG_P6);
	dig_P7 = SPI_read_calib_word(BMP280_REG_DIG_P7);
	dig_P8 = SPI_read_calib_word(BMP280_REG_DIG_P8);
	dig_P9 = SPI_read_calib_word(BMP280_REG_DIG_P9);
	
	/* Record calibration values to EEPROM if not performed yet */
	if(EEPROM_read_word(EEPROM_ADDRESS_DIG_T1) == 0xFFFF)
	{
		EEPROM_write_word(EEPROM_ADDRESS_DIG_T1, dig_T1);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_T2, dig_T2);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_T3, dig_T3);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P1, dig_P1);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P2, dig_P2);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P3, dig_P3);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P4, dig_P4);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P5, dig_P5);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P6, dig_P6);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P7, dig_P7);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P8, dig_P8);
		EEPROM_write_word(EEPROM_ADDRESS_DIG_P9, dig_P9);
	}
	
	
	eeprom_initial_adr = EEPROM_read_word(EEPROM_ADDRESS_LAST);
	if(eeprom_initial_adr >= EEPROM_ADDRESS_DATA_MAX)
		eeprom_initial_adr = 0;
	eeprom_adr = eeprom_initial_adr;
	
	//LED_sign4s();
	
	sei();
	en_flags.rawdata_readout = TRUE;
	while(en_flags.rawdata_readout)
	{
		if(int_flags.measurement_go)
		{
			SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL | BMP280_MODE_FORCED);
			int_flags.measurement_go = FALSE;
		}
		else if(int_flags.readout_go)
		{
			rawdata = SPI_read_rawdata(PRESS_AND_TEMP);
			if(rawdata)
			{
				extract_rawdata(rawdata, &pressure_raw, &temperature_raw);
				free(rawdata);
			}
			int_flags.readout_go = FALSE;
		}
		else if(int_flags.write_eeprom_go)
		{
			int_flags.write_eeprom_go = FALSE;
			if(eeprom_adr >= EEPROM_ADDRESS_DATA_MAX)
				//eeprom_adr = 0;
				en_flags.rawdata_readout = FALSE;
			else
			{
				cli();
				EEPROM_write_word(eeprom_adr, pressure_raw);
				sei();
				eeprom_adr += 2;
			}
		}
	}
	
	cli();
	TCCR1 &= ~_BV(CS10);
	
	//LED_sign4s();
	
	eeprom_final_adr = eeprom_adr;
	EEPROM_write_word(EEPROM_ADDRESS_LAST, eeprom_final_adr);
	
	/* Record highest and lowest pressure values */
	eeprom_data_lowest = EEPROM_read_word(eeprom_initial_adr);
	eeprom_data_highest = eeprom_data_lowest;
	for(eeprom_adr = eeprom_initial_adr; eeprom_adr < eeprom_final_adr; eeprom_adr += 2)
	{
		eeprom_data = EEPROM_read_word(eeprom_adr);
		if(eeprom_data < eeprom_data_lowest)
			eeprom_data_lowest = eeprom_data;
		else if(eeprom_data > eeprom_data_highest)
			eeprom_data_highest = eeprom_data;	
	}
	EEPROM_write_word(EEPROM_ADDRESS_LOWEST_PRESS, eeprom_data_lowest);
	EEPROM_write_word(EEPROM_ADDRESS_HIGHEST_PRESS, eeprom_data_highest);
	
	rawdata = SPI_read_rawdata(PRESS_AND_TEMP);
	if(rawdata)
	{
		extract_rawdata(rawdata, &pressure_raw, &temperature_raw);
		free(rawdata);
	}
	temperature_true = bmp280_compensate_temp(((int32_t)temperature_raw<<4)&0x00FFFF00);
	EEPROM_write_word(474, (temperature_true & 0x0000FFFF));
	EEPROM_write_word(476, (temperature_true >>16));
	pressure_true = bmp280_compensate_press(((int32_t)pressure_raw<<4)&0x00FFFF00);
	EEPROM_write_word(478, (pressure_true & 0x0000FFFF));
	EEPROM_write_word(480, (pressure_true >>16));
	//LED_sign4s();
	
	//LED_data(temp);
	
	LED_sign();

    return(0);
}
