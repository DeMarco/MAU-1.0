/*
 * Testes.c
 *
 * Created: 16/07/2016 18:48:18
 * Author : DRAYCON
 */ 

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Simple AVR demonstration.  Controls a LED that can be directly
 * connected from OC1/OC1A to GND.  The brightness of the LED is
 * controlled with the PWM.  After each period of the PWM, the PWM
 * value is either incremented or decremented, that's all.
 *
 * $Id$
 */

#include "defines.h" 
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

//#include "iocompat.h"		/* Note [1] */

#define SOFTCLOCK_FREQ 100	/* internal software clock */
#define TMR1_SCALE ((F_CPU * 10) / (256UL * SOFTCLOCK_FREQ) + 9) / 10 /* Gera um post-scaler para 10ms */

volatile struct
{
  uint8_t tmr_int: 1;
  //uint8_t adc_int: 1;
  //uint8_t rx_int: 1;
}
intflags;

ISR(TIMER1_OVF_vect)
{
	static uint8_t scaler = TMR1_SCALE;

	if(--scaler == 0)
    {
		scaler = TMR1_SCALE;
		intflags.tmr_int = 1;
    }
}

void ioinit (void)			/* Note [6] */
{
    TCCR1 = _BV(CS10);

	DDRB = _BV(PB3);

    /* Enable timer 1 overflow interrupt. */
    TIMSK = _BV(TOIE1);
    sei ();
}

static void delay_1s (void)
{
  uint8_t i;

  for (i = 0; i < 100; i++)
    _delay_ms(10);
}

int main (void)
{

	uint8_t flash = 0;
	
    ioinit();

    for(;;)
	{
		if (intflags.tmr_int)
		{
			intflags.tmr_int = 0;	
			
			flash++;
			if (flash == 5)
				PORTB &= ~_BV(PB3);
			else if (flash == 100)
			{
				flash = 0;
				PORTB |= _BV(PB3);
			}
			
		}
		
		//delay_1s();
		//PORTB ^= _BV(PB3);
	}
	


    sleep_mode();

    return (0);
}

