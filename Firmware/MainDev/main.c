/*
 * MainDev.c
 *
 * Created: 02/02/2019 12:33:53
 * Author : Draycon
 */

////////////////////////////// OPERATION MODES /////////////////////////////////

//#define	TEST_MODE
//#define FLIGHT_SIM
//#define	FIXED_TEMPERATURE

///////////////////////////////// LIBRARIES ////////////////////////////////////

#include "defines.h"

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

#ifdef FLIGHT_SIM
//#include "FlightProfile_Urano.h"
//#include "FlightProfile_130.h"
#include "FlightProfile_500.h"
#endif

////////////////////////////// GLOBAL VARIABLES ////////////////////////////////

volatile struct
{
	uint8_t readout_cycle_state: 3;
}
int_flags;

volatile struct
{
	uint8_t rawdata_readout: 1;
	uint8_t rawdata_record: 1;
	uint8_t low_freq_sampling_rate: 1;
	uint8_t sampl_rate_reduc_not_needed: 1;
}
en_flags;

// BMP280 Calibration related
uint16_t calib_data_temp[3], calib_data_press[9];
int32_t t_fine;

// Window arrays
uint16_t *array_window_landing = 0, *array_window_ascension = 0, *array_window_ground = 0;
float *window_avg_press_change_rates = 0;
float *window_samples_timestamps = 0;

//////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////

void delay_1s (void)
{
	uint8_t i;

	for (i = 0; i < 10; i++)
		_delay_ms(100);
}

void delay_60s (void)
{
	uint8_t i;

	for (i = 0; i < ROCKET_ASSEMBLY_DELAY; i++)
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
	//Set CSB pin (chip select) to OFF
	PORTB &= ~_BV(CSB);
	//Send address
	SPI_transfer_byte(address & WRITE);
	//Send data
	SPI_transfer_byte(data);
	//Set CSB pin (chip select) to ON
	PORTB |= _BV(CSB);
}

uint8_t SPI_read_byte (uint8_t address)
{
	uint8_t data = 0;

	//Set CSB pin (chip select) to OFF
	PORTB &= ~_BV(CSB);
	//Send address
	SPI_transfer_byte(address | READ);
	//Receive data
	data = SPI_transfer_byte(BLANK);
	//Set CSB pin (chip select) to ON
	PORTB |= _BV(CSB);

	return data;
}

uint32_t SPI_read_rawdata (uint8_t address) //The return type can be uint16_t as long as BMP280's Ultra Low Power mode is used
{
	uint8_t data[3];

	//Set CSB pin (chip select) to OFF
	output_low(CSB);

	//Send address
	SPI_transfer_byte(address | READ);

	//Receive data
	data[2] = SPI_transfer_byte(BLANK);
	data[1] = SPI_transfer_byte(BLANK);
	data[0] = SPI_transfer_byte(BLANK);

	//Set CSB pin (chip select) to ON
	output_high(CSB);

	//Returns raw data shifted to the right according to BMP280's sampling mode:
	return ((data[2]<<8) + data[1]); // Ultra Low Power
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 7); // Low Power
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 6); // Standard Resolution
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 5); // High Resolution
	//return ((data[2]*65536 + data[1]*256 + data[0]) >> 4); // Ultra High Resolution
}

uint16_t SPI_read_calib_word (uint8_t calib_address)
{
	uint16_t calib_data = 0;

	//Set CSB pin (chip select) to OFF
	output_low(CSB);

	//Send address
	SPI_transfer_byte(calib_address | READ);

	//Receive data
	calib_data = SPI_transfer_byte(BLANK);
	calib_data += (SPI_transfer_byte(BLANK) << 8);

	//Set CSB pin (chip select) to ON
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
	// Wait previous write operation to finish
	while(EECR & _BV(EEPE));

	// Activate EEPROM write mode
	EECR &= (~_BV(EEPM1) & ~_BV(EEPM0));

	// Prepare address and data registers
	EEARL = address - 256;
	EEARH = address / 256;
	EEDR = data;

	// Write "1" to EEMPE
	EECR |= _BV(EEMPE);

	// Begin EEPROM writing by activating EEPE
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
	// Wait previous write operation to finish
	while(EECR & _BV(EEPE));

	// Prepare address register
	EEARL = address - 256;
	EEARH = address / 256;

	// Begin EEPROM reading by activating EERE
	EECR |= _BV(EERE);

	// Return data stored in data register
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

void bmp280_init (void)
{
	// Setup BMP280
	SPI_write(CONFIG, BMP280_SETUP_CONFIG);
	SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL);

	// Read calibration constants from BMP280's internal memory
	SPI_read_calib_data();
}

void ioinit (void)
{
	// Activate TIMER 1 using system main clock
	TCCR1 = _BV(CS10);

	// Set pins DO, USCK, LED and CSB as outputs, and DI as input
	DDRB = (_BV(DO) | _BV(USCK) | _BV(LED) | _BV(CSB)) & ~_BV(DI);

	// Set Universal Serial Interface for SPI (Three Wire mode) and SPI mode 00;
	output_low(CSB);  //Start with CSB OFF in order to fix BMP280 to SPI mode;
	_delay_ms(10);    //Wait 10 ms
	output_high(CSB); //Activate CSB and keep it ON (condition to initiate new transmissions);
	output_low(USCK); //UCSK to OFF and keep it (so that BMP280 will be keept in SPI mode 00)
	USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);

	// Enable TIMER 1 Overflow Interrupt
  TIMSK = _BV(TOIE1);
}

//////////////////////// APOGEE CALCULATION FUNCTIONS //////////////////////////

// Returns temperature in degC. Example: output value T = 5123 corresponds to 51.23 degC.
// t_fine carries the temperature value as used for pressure compensation.
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
// Exemple: output value P = 24674867 corresponds to 24674867/256 = 96386.2 Pa.
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

	// Ensure EEPROM is not empty
	temperature_raw = EEPROM_read_word(EEPROM_ADDRESS_GROUND_TEMP);
	if(temperature_raw != 0xFFFF)
	{

		#ifdef TEST_MODE
		temperature_true = bmp280_compensate_temp((int32_t)temperature_raw<<4);
		EEPROM_write_dword(EEPROM_ADDRESS_TRUE_TEMP_GND, temperature_true);
		#else
		bmp280_compensate_temp((int32_t)temperature_raw<<4);
		#endif

		// Calculate apogee pressure of the recorded flight
		pressure_raw = EEPROM_read_word(EEPROM_ADDRESS_PRESS_APOGEE);
		pressure_true_apogee = bmp280_compensate_press((int32_t)pressure_raw <<4);
		#ifdef TEST_MODE
		EEPROM_write_dword(EEPROM_ADDRESS_TRUE_PRESS_LST, pressure_true_apogee);
		#endif

		// Calculate ground pressure of the recorded flight
		pressure_raw = EEPROM_read_word(EEPROM_ADDRESS_PRESS_GROUND);
		pressure_true_ground = bmp280_compensate_press((int32_t)pressure_raw <<4);
		#ifdef TEST_MODE
		EEPROM_write_dword(EEPROM_ADDRESS_TRUE_PRESS_HST, pressure_true_ground);
		#endif

		// Adapt atmospheric model breakpoints to Fixed Point format:
		press_breakpoint = SENSOR_MAX_PRESSURE;
		press_breakpoint <<= 8;
		press_breakpoint_step = PRESSURE_STEP;
		press_breakpoint_step <<= 8;

		for(i=1; i < NUMBER_OF_BREAKPOINTS; i++)
		{
			press_breakpoint_previous = press_breakpoint;
			press_breakpoint -= press_breakpoint_step;

			height_array_tmp_prev = pgm_read_float(&height_array[i-1]);
			coef = (pgm_read_float(&height_array[i]) - height_array_tmp_prev) / ((press_breakpoint - press_breakpoint_previous)>>8);

			// Calculate ground altitude
			if((pressure_true_ground <= press_breakpoint_previous) && (pressure_true_ground >= press_breakpoint))
			{
				height_ground = coef * ((pressure_true_ground - press_breakpoint_previous)>>8) + height_array_tmp_prev;

				#ifdef TEST_MODE
				EEPROM_write_dword(EEPROM_ADDRESS_H_GND, (uint32_t)height_ground);
				#endif
			}

			// Calculate apogee altitude
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

		// Report apogee in decimeters
		LED_data((int32_t)((height_apogee - height_ground)*10));

	}
}

////////////////////////// TIMER INTERRUPT HANDLER /////////////////////////////

ISR(TIMER1_OVF_vect)
{
	static uint16_t scaler = POST_SCALER_BEGIN_CYCLE_HF + 1;
	//static uint8_t cycle_count = 0;

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
				if(en_flags.low_freq_sampling_rate)
					scaler = POST_SCALER_BEGIN_CYCLE_LF + 1;
				else
					scaler = POST_SCALER_BEGIN_CYCLE_HF + 1;
				break;
		}
	}
}

////////////////// PRESSURE CONTINUOUS SAMPLING FUNCTION ///////////////////////

void rawdata_readout_cycle (uint16_t *eeprom_address)
{
	uint32_t pressure_raw = 0;
	uint8_t count = 0;
	uint8_t i = 0;
	uint8_t cycle_count_LED = 0;
	int32_t numerator = 0;
	int32_t numerator_ground = 0;
	int32_t average = 0;
	int32_t average_ground = 0;
	int32_t variance = 0;
	int32_t term = 0;
	float avg_press_change_rate = 0.0;
	float sample_timestamp = 0.0;
	float remaining_EEPROM_time = 0.0;
	float probable_descent_time = 0.0;
	uint8_t window_size_landing_detection = 20;
	uint8_t process_type = DETECT_ASCENSION;

	#ifdef FLIGHT_SIM
	static uint16_t flight_profile_index = 0;
	#endif

	array_window_ascension = malloc(2*WINDOW_SIZE_ASCENT_DETECTION);
	array_window_ground = malloc(2*(WINDOW_SIZE_GROUND_PRESS_CALC + WINDOW_SIZE_ASCENT_DETECTION));
	array_window_landing = malloc(2*window_size_landing_detection);
	window_samples_timestamps = malloc(sizeof(float)*window_size_landing_detection);
	window_avg_press_change_rates = malloc(sizeof(float)*window_size_landing_detection);

	// Detect ascent + Calculate and record ground average pressure in NVM;
	en_flags.rawdata_readout = ENABLED;

	// Enable global interrupts
	sei();

	while(en_flags.rawdata_readout)
	{
		switch(int_flags.readout_cycle_state)
		{
			case MEASURE:
				SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL | BMP280_MODE_FORCED);
				int_flags.readout_cycle_state = DO_NOTHING;
				break;

			case READ_OUT:
				#ifdef FLIGHT_SIM
				pressure_raw = pgm_read_word(&pressure_samples[flight_profile_index]);
				flight_profile_index++;
				#else
				pressure_raw = SPI_read_rawdata(PRESSURE);
				#endif

				int_flags.readout_cycle_state = DO_NOTHING;
				break;

			case STORE_NVM:
				if(en_flags.rawdata_record)
				{
					if(*eeprom_address >= EEPROM_ADDRESS_DATA_MAX)
					{
						//*eeprom_address = 0;	//Enabling this line and commenting the one
																		//below would enable infinite recording in
																		//the NVM, but of course causing the overwrite
																		//of contents everytime the address reached
																		//its maximum allowed value.
						en_flags.rawdata_readout = DISABLED; //This line ensures the altimeter
																							//will stop recording once the
																							//NVM is full.
					}
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
				// Register the timestamp of the sample:
				if(en_flags.low_freq_sampling_rate)
					sample_timestamp += CYCLE_DURATION_LF;
				else
					sample_timestamp += CYCLE_DURATION_HF;

				// Do process:
				switch(process_type)
				{
					case NO_PROCESSING:
						break;

					case DETECT_ASCENSION:
						if(count < (WINDOW_SIZE_GROUND_PRESS_CALC + WINDOW_SIZE_ASCENT_DETECTION))
						{
							array_window_ground[count] = pressure_raw;
							if(count >= WINDOW_SIZE_GROUND_PRESS_CALC)
							{
								array_window_ascension[count - WINDOW_SIZE_GROUND_PRESS_CALC] = pressure_raw;
							}
							count++;
						}
						else
						{
							numerator = 0;
							variance = 0;

							// CALCULATE GROUND AVERAGE PRESSURE
							numerator_ground = 0;
							for(i = 1; i < (WINDOW_SIZE_GROUND_PRESS_CALC + WINDOW_SIZE_ASCENT_DETECTION); i++)
							{
								array_window_ground[i-1] = array_window_ground[i];
								if(i <= WINDOW_SIZE_GROUND_PRESS_CALC)
									numerator_ground += array_window_ground[i-1];
							}
							array_window_ground[i-1] = pressure_raw;
							average_ground = numerator_ground / WINDOW_SIZE_GROUND_PRESS_CALC;

							// CALCULATE PRESSURE VARIANCE FOR ASCENT DETECTION
							for(i = 1; i < WINDOW_SIZE_ASCENT_DETECTION; i++)
							{
								array_window_ascension[i-1] = array_window_ascension[i];
								numerator += array_window_ascension[i];
							}
							array_window_ascension[i-1] = pressure_raw;
							numerator += pressure_raw;
							average = numerator / WINDOW_SIZE_ASCENT_DETECTION;
							for(i = 0; i < WINDOW_SIZE_ASCENT_DETECTION; i++)
							{
								term = array_window_ascension[i] - average;
								variance += term*term;
							}
							variance /= WINDOW_SIZE_ASCENT_DETECTION;

							// IF ASCENT IS DETECTED:
							if(variance > MIN_VARIANCE_ASCENSION_DETECT)
							{
								// Record ground pressure in NVM:
								cli();
								EEPROM_write_word(EEPROM_ADDRESS_PRESS_GROUND, average_ground);
								sei();

								array_window_ascension[0] = average_ground; //So that the chart starts at 0m.
								//Instead of forcing the first sample to zero meters, shouldn't all smples be
								//offsetted according to: sample[0] +/- average ?

								/*#ifdef TEST_MODE
								cli();
								EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_1, variance);
								sei();
								#endif*/

								// Switch to Record flight data + Detect landing;
								SPI_write(CONFIG, BMP280_FILTER_DISABLED);
								en_flags.rawdata_record = ENABLED;
								process_type = DETECT_LANDING;
								count = 0;

							}
						}
						break;

					case DETECT_LANDING:
						// First, fulfill the window with its first 20 samples
						if(count < window_size_landing_detection)
						{
							array_window_landing[count] = pressure_raw;
							window_samples_timestamps[count] = sample_timestamp;

							count++;
						}
						// From this point on, the window already has at least 20 valid samples.
						// Now, fulfill the array of calculated pressure change rates with its first 20 values.
						else if((count >= window_size_landing_detection) && (count < window_size_landing_detection*2))
						{
							// Continue to register pressure samples:
							for(i=1; i < window_size_landing_detection; i++)
							{
								array_window_landing[i-1] = array_window_landing[i];
								window_samples_timestamps[i-1] = window_samples_timestamps[i];
							}
							array_window_landing[i-1] = pressure_raw;
							window_samples_timestamps[i-1] = sample_timestamp;

							// Calculate and register average pressure change rate with the first and last samples of the window
							avg_press_change_rate = abs(array_window_landing[window_size_landing_detection-1] - array_window_landing[0])
														/ (window_samples_timestamps[window_size_landing_detection-1] - window_samples_timestamps[0]);
							window_avg_press_change_rates[count-window_size_landing_detection] = avg_press_change_rate;

							count++;
						}
						// From this point on, the array of calculated pressure change rates has at least 20 valid values.
						// Now the actual variance calculation can start using those values.
						else
						{
							numerator = 0;
							variance = 0;

							// Continue to register pressure samples and calculate change rates:
							for(i=1; i < window_size_landing_detection; i++)
							{
								array_window_landing[i-1] = array_window_landing[i];
								window_samples_timestamps[i-1] = window_samples_timestamps[i];
								window_avg_press_change_rates[i-1] = window_avg_press_change_rates[i];
								numerator += window_avg_press_change_rates[i];
							}
							array_window_landing[i-1] = pressure_raw;
							window_samples_timestamps[i-1] = sample_timestamp;

							// Calculate and register average pressure change rate with the first and last samples of the window
							avg_press_change_rate = abs(array_window_landing[window_size_landing_detection-1] - array_window_landing[0])
														/ (window_samples_timestamps[window_size_landing_detection-1] - window_samples_timestamps[0]);
							window_avg_press_change_rates[i-1] = avg_press_change_rate;
							numerator += avg_press_change_rate;

							// Calculate the variance of the pressure change rates:
							average = numerator / window_size_landing_detection;
							for(i = 0; i < window_size_landing_detection; i++)
							{
								term = window_avg_press_change_rates[i] - average;
								variance += term*term;
							}
							variance /= window_size_landing_detection;

							// Check if variance is below the threshold that indicates parachute descent detected:
							if(variance < MAX_VARIANCE_PARACHUTE_DETECT)
							{
								// Check if pressure change rate is close to zero, indicating that landing already occurred:
								if(avg_press_change_rate <= 1) //avg_press_change_rate is absolute, thus never below zero.
								{
									en_flags.rawdata_readout = DISABLED;

									#ifdef TEST_MODE
									cli();
									EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_1, variance);
									EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_2, (int32_t)avg_press_change_rate);
									sei();
									#endif

									// After this point, the loop is broken and the program returns to main();
								}
								else
								{
									if(!en_flags.low_freq_sampling_rate && !en_flags.sampl_rate_reduc_not_needed)
									{
										// Calculate if landing is expected within the available remaining NVM slots at current sampling rate:
										// eeprom_address - 2 is the current address
										remaining_EEPROM_time = CYCLE_DURATION_HF * ((EEPROM_ADDRESS_DATA_MAX - *eeprom_address - 2) / 2);
										probable_descent_time = abs(pressure_raw - average_ground) / avg_press_change_rate;

										#ifdef TEST_MODE
										cli();
										EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_3, (int32_t)remaining_EEPROM_time);
										EEPROM_write_dword(EEPROM_ADDRESS_DEBUG_4, (int32_t)probable_descent_time);
										sei();
										#endif

										if(remaining_EEPROM_time > (probable_descent_time + PREDICTED_FLIGHT_TIME_ERROR))
										{
											en_flags.sampl_rate_reduc_not_needed = TRUE;
										}
										else
										{
											en_flags.low_freq_sampling_rate = TRUE;
											window_size_landing_detection = 5;
											cli();
											EEPROM_write_word(EEPROM_ADDRESS_SAMPL_RATE_CHNG, *eeprom_address);
											sei();
										}
									}
								}
							}
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
						if(count > WINDOW_SIZE_GROUND_PRESS_CALC)
						{
							if(++cycle_count_LED == 5)
							{
								output_high(LED);
								cycle_count_LED = 0;
							}
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

	cli();
}

//////////////////////////////////////////////////////////////////
//	MAIN FUNCTION
//////////////////////////////////////////////////////////////////
int main (void)
{
	//// RUN INITIALIZATIONS: ////

	// Temperature and pressure data
	uint32_t temperature_raw = 0;
	uint16_t raw_press_highest = 0;

	// NVM settings
	uint16_t eeprom_initial_adr = 0;
	uint16_t eeprom_final_adr = 0;
	uint16_t eeprom_adr = 0;
	uint16_t eeprom_data = 0;

	// Flags initialization
	int_flags.readout_cycle_state = DO_NOTHING;
	en_flags.rawdata_readout = DISABLED;
	en_flags.rawdata_record = DISABLED;
	en_flags.low_freq_sampling_rate = FALSE;
	en_flags.sampl_rate_reduc_not_needed = FALSE;

	// Misc
	uint8_t i = 0;

	ioinit();
	bmp280_init();

	//LED_sign(); ///Causes to much confusion as users tend to account this blinking into summing the flashes from apogee reporting.

	//// CALCULATE AND REPORT RECORDED FLIGHT APOGEE: ////

	#ifndef TEST_MODE
	#ifndef FLIGHT_SIM
	report_apogee();
	#endif
	#endif

	//// ALLOW TIME FOR INSTALLING ALTIMETER IN THE ROCKET: ////
	delay_60s();

	//// MEASURE AND RECORD GROUND TEMPERATURE: ////

	SPI_write(CTRL_MEAS, BMP280_SETUP_CTRL | BMP280_MODE_FORCED);
	_delay_ms(10);
	temperature_raw = SPI_read_rawdata(TEMPERATURE);
	EEPROM_write_word(EEPROM_ADDRESS_GROUND_TEMP, temperature_raw);

	//// START PRESSURE MONITORING, RECORDING AND PROCESSING: ////

	eeprom_adr = WINDOW_SIZE_ASCENT_DETECTION * 2; // The first time a sample will be recorded is after ascent has been detected,
																								 // which uses a window of 4 samples that will be only on RAM until the end of
																								 // the flight, after which it can be fixed to the NVM in its first 4 slots.
	rawdata_readout_cycle(&eeprom_adr);

	//// PREPARE FOR SHUTDOWN: ////

	// Deactivate TIMER1
	TCCR1 &= ~_BV(CS10);

	// Flush arrays that have no more use
	free(array_window_landing);
	free(window_samples_timestamps);
	free(window_avg_press_change_rates);

	// Save last NVM position to contain pressure data
	eeprom_final_adr = eeprom_adr;
	#ifdef TEST_MODE
	EEPROM_write_word(EEPROM_ADDRESS_LAST, eeprom_final_adr);
	#endif

	// Save samples used in ascent detection to NVM
	eeprom_adr = eeprom_initial_adr;
	for(i = 0; i < WINDOW_SIZE_ASCENT_DETECTION; i++)
	{
		EEPROM_write_word(eeprom_adr, array_window_ascension[i]);
		eeprom_adr += 2;
	}
	free(array_window_ascension);
	free(array_window_ground);

	// Determine apogee pressure and record it to NVM
	for(eeprom_adr = eeprom_initial_adr; eeprom_adr < eeprom_final_adr; eeprom_adr += 2)
	{
		eeprom_data = EEPROM_read_word(eeprom_adr);
		if(eeprom_data > raw_press_highest)
			raw_press_highest = eeprom_data;
	}
	EEPROM_write_word(EEPROM_ADDRESS_PRESS_APOGEE, raw_press_highest);

	// Erase NVM slots not used for flight data recording
	for(eeprom_adr = eeprom_final_adr; eeprom_adr < EEPROM_ADDRESS_DATA_MAX; eeprom_adr += 2)
		EEPROM_write_word(eeprom_adr, 0xffff);

	// Store BMP280's calibration constants into NVM
	EEPROM_store_calib_data();

	//// AUTO POWER DOWN: ////

	LED_sign(); //For the sake of testing, so that I know when the altimeter reached the end of the flow.

	cli();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();

    return(0);
}
