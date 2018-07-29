/*
 * defines.h
 *
 * Created: 16/07/2016 20:48:40
 *  Author: DRAYCON
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

/* General */
#define HIGH	1
#define LOW		0

#define TRUE	1
#define FALSE	0

#define PRESSURE	0
#define TEMPERATURE	1

#define output_high(pin)		PORTB |= _BV(pin)
#define output_low(pin)			PORTB &= ~_BV(pin)


/* CPU frequency */
#define F_CPU 1000000UL


/* Calculate a post scaler for Timer1 */
#define _PS(instant)	(uint16_t)((instant * F_CPU) / TIMER1_FULL_VALUE)


/* ATtiny85 used pins */
#define DI   PB0
#define DO   PB1
#define USCK PB2
#define LED  PB3
#define CSB  PB4


/* BMP280 registers */
#define READ		0x80
#define WRITE		0x7f
#define BLANK		0x00

#define	TEMP_XLSB	0xfc
#define	TEMP_LSB	0xfb
#define	TEMP_MSB	0xfa
#define	PRESS_XLSB	0xf9
#define	PRESS_LSB	0xf8
#define	PRESS_MSB	0xf7

#define	CONFIG		0xf5
#define T_SB2		7
#define T_SB1		6
#define T_SB0		5
#define FILTER2		4
#define FILTER1		3
#define FILTER0		2
#define SPI3W_EN	0

#define	CTRL_MEAS	0xf4
#define OSRS_T2		7
#define OSRS_T1		6
#define OSRS_T0		5
#define OSRS_P2		4
#define OSRS_P1		3
#define OSRS_P0		2
#define MODE1		1
#define MODE0		0

#define	STATUS		0xf3
#define MEASURING	3
#define IM_UPDATE	0

#define	RESET		0xe0
#define RST_BYTE	0xb6

#define	ID			0xd0

//Temperature Oversampling = Ultra Low Power; Pressure Oversampling = Ultra Low Power; Mode = Normal;
//#define BMP280_SETUP_CTRL	_BV(OSRS_T0) | _BV(OSRS_P0) | _BV(MODE1) | _BV(MODE0) //

//Temperature Oversampling = Ultra Low Power; Pressure Oversampling = Ultra Low Power; Mode = Sleep;
#define BMP280_SETUP_CTRL	(_BV(OSRS_T0) | _BV(OSRS_P0))

//Stand by time = 125ms; Filter coeficient = 4;
#define BMP280_SETUP_CONFIG	(_BV(T_SB1) | _BV(FILTER1))

#define BMP280_OSRS_P		(_BV(OSRS_P2) | _BV(OSRS_P1) | _BV(OSRS_P0))

#define BMP280_PRESS_SKIP	~_BV(OSRS_P2) & ~_BV(OSRS_P1) & ~_BV(OSRS_P0)
#define BMP280_PRESS_ULP	_BV(OSRS_P0)
#define BMP280_PRESS_LP		_BV(OSRS_P1)
#define BMP280_PRESS_SR		_BV(OSRS_P1) | _BV(OSRS_P0)
#define BMP280_PRESS_HR		_BV(OSRS_P2)
#define BMP280_PRESS_UHR	_BV(OSRS_P2) | _BV(OSRS_P0)

#define BMP280_OSRS_T		(_BV(OSRS_T2) | _BV(OSRS_T1) | _BV(OSRS_T0))

#define BMP280_TEMP_SKIP	~_BV(OSRS_T2) & ~_BV(OSRS_T1) & ~_BV(OSRS_T0)
#define BMP280_TEMP_ULP		_BV(OSRS_T0)
#define BMP280_TEMP_LP		_BV(OSRS_T1)
#define BMP280_TEMP_SR		_BV(OSRS_T1) | _BV(OSRS_T0)
#define BMP280_TEMP_HR		_BV(OSRS_T2)
#define BMP280_TEMP_UHR		_BV(OSRS_T2) | _BV(OSRS_T0)

#define BMP280_MODE_SLEEP	~_BV(MODE1) & ~_BV(MODE0)
#define BMP280_MODE_FORCED	_BV(MODE0)
#define BMP280_MODE_NORMAL	_BV(MODE1) | _BV(MODE0)

#define PRESSURE_ONLY		0xf7, 3
#define TEMPERATURE_ONLY	0xfa, 3
#define PRESS_AND_TEMP		0xf7, 6


/* Timer Settings */
#define TIMER1_FULL_VALUE	256UL

#define MAX_FLIGHT_TIME				44 //in seconds
#define BMP280_MAX_CYCLE_COUNT		MAX_FLIGHT_TIME / MEASUREMENT_PERIOD

#define MEASUREMENT_PERIOD		0.192 //in seconds
#define MEASUREMENT_INSTANT		0.192
#define READOUT_INSTANT			0.142
#define EEPROM_WRITE_INSTANT	0.132
#define LED_ON_INSTANT			0.082
#define LED_OFF_INSTANT			0.072

#define POST_SCALER_MEASUREMENT		_PS(MEASUREMENT_INSTANT)
#define POST_SCALER_READOUT			_PS(READOUT_INSTANT)
#define POST_SCALER_WRITE_EEPROM	_PS(EEPROM_WRITE_INSTANT)
#define POST_SCALER_LED_ON			_PS(LED_ON_INSTANT)
#define POST_SCALER_LED_OFF			_PS(LED_OFF_INSTANT)


/* EEPROM settings */
#define EEPROM_ADDRESS_LAST		510
#define EEPROM_ADDRESS_MAX		EEPROM_ADDRESS_LAST - 10

#endif /* DEFINES_H_ */