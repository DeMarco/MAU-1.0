#ifndef DEFINES_H_
#define DEFINES_H_

/* General definitions */
#define HIGH	1
#define LOW		0

#define TRUE	1
#define FALSE	0

#define output_high(pin)		PORTB |= _BV(pin)
#define output_low(pin)			PORTB &= ~_BV(pin)


/* CPU Clock */
#define F_CPU 1000000UL


/* ATtiny85 Pinnout */
#define DI   PB0
#define DO   PB1
#define USCK PB2
#define LED  PB3
#define CSB  PB4


/* BMP280 Registers */
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

#define BMP280_REG_DIG_T1	0x88
#define BMP280_REG_DIG_T2	0x8a
#define BMP280_REG_DIG_T3	0x8c
#define BMP280_REG_DIG_P1	0x8e
#define BMP280_REG_DIG_P2	0x90
#define BMP280_REG_DIG_P3	0x92
#define BMP280_REG_DIG_P4	0x94
#define BMP280_REG_DIG_P5	0x96
#define BMP280_REG_DIG_P6	0x98
#define BMP280_REG_DIG_P7	0x9a
#define BMP280_REG_DIG_P8	0x9c
#define BMP280_REG_DIG_P9	0x9e

//IIR Filter Coefficient = 0;
#define BMP280_FILTER_DISABLED		0

//IIR Filter Coefficient = 4;
#define BMP280_FILTER_4				(_BV(FILTER1))

//Temperature Oversampling = Ultra Low Power; Pressure Oversampling = Ultra Low Power; Mode = Normal;
//#define BMP280_SETUP_CTRL	_BV(OSRS_T0) | _BV(OSRS_P0) | _BV(MODE1) | _BV(MODE0) //

//Temperature Oversampling = Ultra Low Power; Pressure Oversampling = Ultra Low Power; Mode = Sleep;
#define BMP280_SETUP_CTRL	(_BV(OSRS_T0) | _BV(OSRS_P0))

//Reading time interval = 125ms; IIR Filter Coefficient = 4;
//#define BMP280_SETUP_CONFIG	(_BV(T_SB1) | _BV(FILTER1))

//IIR Filter Coefficient = 4;
#define BMP280_SETUP_CONFIG	BMP280_FILTER_4


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

#define PRESSURE		PRESS_MSB
#define TEMPERATURE		TEMP_MSB

/* Barometric Equation Constants - Model 1: from sea level to 11km */
#define	EQ1_Tb	(273,15 + 20)
#define	EQ1_Lb	-0.0065
#define	EQ1_R	8.31432
#define	EQ1_g	9.80665
#define	EQ1_M	0.0289644
#define	EQ1_Pb	101325.0

#define GROUND_FIXED_TEMP		20 //in degC
#define GROUND_FIXED_TEMP_FINE	(((GROUND_FIXED_TEMP*100) << 8) - 128) / 5

/* Timer Setup */
#define TIMER1_FULL_VALUE	256UL

//#define MAX_FLIGHT_TIME					48 //in seconds, MAX = 46.272 ///OBSOLETE
//#define BMP280_MAX_CYCLE_COUNT			(MAX_FLIGHT_TIME / CYCLE_DURATION) /// OBSOLETE
#define MAX_SAMPLE_AMOUNT 							240
#define WINDOW_SIZE_ASCENT_DETECTION		4
#define WINDOW_SIZE_GROUND_PRESS_CALC		16
//#define WINDOW_SIZE_LANDING_DETECTION		20 //Aproximately 4 seconds --> ///became a variable

#define ROCKET_ASSEMBLY_DELAY						10 //in seconds

#define CYCLE_DURATION_LF			1		  //in seconds
#define CYCLE_DURATION_HF			0.192 //in seconds
#define MEASUREMENT_INSTANT		0.192
#define READOUT_INSTANT				0.142
#define EEPROM_WRITE_INSTANT	0.132
#define PROCESS_INSTANT				0.082
#define CONTROL_LED_INSTANT		0.010
//#define LED_OFF_INSTANT			0.072 ///OBSOLETE

/* TIMER1 post scaler calculation */
#define _PS(instant)	(uint16_t)((instant * F_CPU) / TIMER1_FULL_VALUE)

#define POST_SCALER_BEGIN_CYCLE_LF	_PS(CYCLE_DURATION_LF) // post scaler for cycle duration = 1s
#define POST_SCALER_BEGIN_CYCLE_HF	_PS(CYCLE_DURATION_HF) // post scaler for cycle duration = 0.191s
#define POST_SCALER_MEASUREMENT			_PS(MEASUREMENT_INSTANT)
#define POST_SCALER_READOUT					_PS(READOUT_INSTANT)
#define POST_SCALER_WRITE_EEPROM		_PS(EEPROM_WRITE_INSTANT)
#define POST_SCALER_PROCESS					_PS(PROCESS_INSTANT)
#define POST_SCALER_CONTROL_LED			_PS(CONTROL_LED_INSTANT)

/* Sampling cycle state names */
#define DO_NOTHING	0
#define MEASURE		1
#define READ_OUT	2
#define STORE_NVM	3
#define PROCESS		4
#define CONTROL_LED	5
#define CYCLES_FULL	6

/* Process names used during the sampling cycle */
#define NO_PROCESSING				0
#define MEASURE_GROUND_PRESSURE		1
#define DETECT_ASCENSION			2
#define DETECT_LANDING				3

/* Variance thresholds for ascent and landing detection */
#define MIN_VARIANCE_ASCENSION_DETECT	100
#define MAX_VARIANCE_LANDING_DETECT		10

/* NVM map */
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
	#define EEPROM_ADDRESS_DEBUG_1			478
	#define EEPROM_ADDRESS_DEBUG_2			482
	#define EEPROM_ADDRESS_H_MAX			486
	#define EEPROM_ADDRESS_H_GND			490
	#define EEPROM_ADDRESS_APOGEE			494
	#define EEPROM_ADDRESS_TRUE_TEMP_GND	498
	#define EEPROM_ADDRESS_TRUE_PRESS_LST	502
	#define EEPROM_ADDRESS_TRUE_PRESS_HST	506
	#define EEPROM_ADDRESS_LAST				510
#else
	#define EEPROM_ADDRESS_DATA_MAX			482
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

#endif /* DEFINES_H_ */
