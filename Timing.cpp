/****************************************************************************
 *                   ,__
 *                  ( ..)
 *  ______o00-------------00o_________________________________________________
 * | ___  __, ___          __,        _ _   ___                               |
 * | |  | |__ |__)\  /|  |(___        /||  /|__)                              |
 * | |  | |__ |  \ \/ |__|.___)      /-|| / | \                   nervusAVR   |
 * |________________________________/___|/_____\______________________________/
 *
 * Copyright (c) 2015 Mircea Marghidanu (nervus@nervus.org)
 * DWYWWIBMC license ( Do whatever you want with it but maintain copyright )
 *
 ******************************************************************************/


/*
  Parts used from  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#include "nervusAVR.h"

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}

unsigned long millis()
{
	unsigned long m;

    BEGIN_ATOMIC_BLOCK();
    {
        m = timer0_millis;
    }
    END_ATOMIC_BLOCK();

	return m;
}

unsigned long micros()
{
	unsigned long m;
	uint8_t t;

    BEGIN_ATOMIC_BLOCK();
    {
        m = timer0_overflow_count;
        t = TCNT0;

        if ((TIFR0 & _BV(TOV0)) && (t < 255))
            m++;
    }
    END_ATOMIC_BLOCK();
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms)
{
	uint16_t start = (uint16_t)micros();

	while (ms > 0) {
		if (((uint16_t)micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
}

/* Delay for the given number of microseconds.  Assumes a 1, 8, 12, 16, 20 or 24 MHz clock. */
void delayMicroseconds(unsigned int us)
{
	// call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)

	// calling avrlib's delay_us() function with low values (e.g. 1 or
	// 2 microseconds) gives delays longer than desired.
	//delay_us(us);
#if F_CPU >= 24000000L
	// for the 24 MHz clock for the aventurous ones, trying to overclock

	// zero delay fix
	if (!us) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/6 of a microsecond (4 cycles)
	// per iteration, so execute it six times for each microsecond of
	// delay requested.
	us *= 6; // x6 us, = 7 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 22 (24) cycles above, remove 5, (5*4=20)
	// us is at least 6 so we can substract 5
	us -= 5; //=2 cycles

#elif F_CPU >= 20000000L
	// for the 20 MHz clock on rare Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 18 (20) cycles, which is 1us
	__asm__ __volatile__ (
		"nop" "\n\t"
		"nop" "\n\t"
		"nop" "\n\t"
		"nop"); //just waiting 4 cycles
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/5 of a microsecond (4 cycles)
	// per iteration, so execute it five times for each microsecond of
	// delay requested.
	us = (us << 2) + us; // x5 us, = 7 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 26 (28) cycles above, remove 7, (7*4=28)
	// us is at least 10 so we can substract 7
	us -= 7; // 2 cycles

#elif F_CPU >= 16000000L
	// for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/4 of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2; // x4 us, = 4 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 19 (21) cycles above, remove 5, (5*4=20)
	// us is at least 8 so we can substract 5
	us -= 5; // = 2 cycles,

#elif F_CPU >= 12000000L
	// for the 12 MHz clock if somebody is working with USB

	// for a 1 microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1.5us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/3 of a microsecond (4 cycles)
	// per iteration, so execute it three times for each microsecond of
	// delay requested.
	us = (us << 1) + us; // x3 us, = 5 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 20 (22) cycles above, remove 5, (5*4=20)
	// us is at least 6 so we can substract 5
	us -= 5; //2 cycles

#elif F_CPU >= 8000000L
	// for the 8 MHz internal clock

	// for a 1 and 2 microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 2us
	if (us <= 2) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/2 of a microsecond (4 cycles)
	// per iteration, so execute it twice for each microsecond of
	// delay requested.
	us <<= 1; //x2 us, = 2 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 17 (19) cycles above, remove 4, (4*4=16)
	// us is at least 6 so we can substract 4
	us -= 4; // = 2 cycles

#else
	// for the 1 MHz internal clock (default settings for common Atmega microcontrollers)

	// the overhead of the function calls is 14 (16) cycles
	if (us <= 16) return; //= 3 cycles, (4 when true)
	if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to substract 22)

	// compensate for the time taken by the preceeding and next commands (about 22 cycles)
	us -= 22; // = 2 cycles
	// the following loop takes 4 microseconds (4 cycles)
	// per iteration, so execute it us/4 times
	// us is at least 4, divided by 4 gives us 1 (no zero delay bug)
	us >>= 2; // us div 4, = 4 cycles
	

#endif

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
	// return = 4 cycles
}

void TimingInit()
{
    // this needs to be called before setup() or some functions won't
    // work there
    sei();
    
    sbi(TCCR0A, WGM01);
    sbi(TCCR0A, WGM00);
    
    // set timer 0 prescale factor to 64
    sbi(TCCR0B, CS01);
    sbi(TCCR0B, CS00);
    
    // enable timer 0 overflow interrupt
    sbi(TIMSK0, TOIE0);
    
    // timers 1 and 2 are used for phase-correct hardware pwm
    // this is better for motors as it ensures an even waveform
    // note, however, that fast pwm mode can achieve a frequency of up
    // 8 MHz (with a 16 MHz clock) at 50% duty cycle
    
    TCCR1B = 0;
    
    // set timer 1 prescale factor to 64
    sbi(TCCR1B, CS11);
#if F_CPU >= 8000000L
    sbi(TCCR1B, CS10);
#endif
    
    // put timer 1 in 8-bit phase correct pwm mode
    sbi(TCCR1A, WGM10);
    
    sbi(TCCR3B, CS31);		// set timer 3 prescale factor to 64
    sbi(TCCR3B, CS30);
    sbi(TCCR3A, WGM30);		// put timer 3 in 8-bit phase correct pwm mode
    
    /* beginning of timer4 block for 32U4 and similar */
    sbi(TCCR4B, CS42);		// set timer4 prescale factor to 64
    sbi(TCCR4B, CS41);
    sbi(TCCR4B, CS40);
    sbi(TCCR4D, WGM40);		// put timer 4 in phase- and frequency-correct PWM mode
    sbi(TCCR4A, PWM4A);		// enable PWM mode for comparator OCR4A
    sbi(TCCR4C, PWM4D);		// enable PWM mode for comparator OCR4D
    
    // set a2d prescaler so we are inside the desired 50-200 KHz range.
#if F_CPU >= 16000000 // 16 MHz / 128 = 125 KHz
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
#elif F_CPU >= 8000000 // 8 MHz / 64 = 125 KHz
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
#elif F_CPU >= 4000000 // 4 MHz / 32 = 125 KHz
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
#elif F_CPU >= 2000000 // 2 MHz / 16 = 125 KHz
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
#elif F_CPU >= 1000000 // 1 MHz / 8 = 125 KHz
    cbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
#else // 128 kHz / 2 = 64 KHz -> This is the closest you can get, the prescaler is 2
    cbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
#endif
    // enable a2d conversions
    sbi(ADCSRA, ADEN);
}

