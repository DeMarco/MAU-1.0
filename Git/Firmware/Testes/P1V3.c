/*
 * PrototypeOne.c
 * Version1
 *
 * Created: 15/07/2016 23:31:54
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

#include "definesV2.h"
 
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/eeprom.h>

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

void LED_sign2 (void)
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
	delay_1s();
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
		_delay_ms(250);
		output_high(LED);
		_delay_ms(250);
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
	EEPROM_write_byte(address, data - 256);
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

void bmp280_init (void)
{
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
    sei();
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
	uint8_t *rawdata;
	uint32_t pressure_raw = 0;
	uint32_t temperature_raw = 0;
	//uint8_t byte;
	//uint32_t *truedata;
	uint16_t initial_address = 0;
	uint16_t eeprom_address = 0;
	//uint8_t eeprom_data = 0;
	//uint16_t count = 0;
	
	int_flags.measurement_go = FALSE;
	int_flags.readout_go = FALSE;
	int_flags.write_eeprom_go = FALSE;
	int_flags.readout_cycles_full = FALSE;
	en_flags.rawdata_readout = FALSE;
	
	ioinit();
	bmp280_init();
	
	LED_sign();
	
	initial_address = EEPROM_read_word(EEPROM_ADDRESS_LAST);
	if(initial_address >= EEPROM_ADDRESS_MAX)
		initial_address = 0;
	eeprom_address = initial_address;
	
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
			if(eeprom_address >= EEPROM_ADDRESS_MAX)
				//eeprom_address = 0;
				en_flags.rawdata_readout = FALSE;
			else
			{
				cli();
				EEPROM_write_word(eeprom_address, pressure_raw);
				eeprom_address += 2;
				EEPROM_write_word(EEPROM_ADDRESS_LAST, eeprom_address);
				sei();
			}
		}
	}
	
	
	LED_sign();

    return(0);
}

