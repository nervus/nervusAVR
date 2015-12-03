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

#pragma once

#define ENABLE_DELAY 1

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#if ENABLE_DELAY

#define Delay_ns(__ns) \
	if((unsigned long) (F_CPU/1000000000.0 * __ns) != F_CPU/1000000000.0 * __ns)\
		__builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000000.0 * __ns)+1);\
	else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000000.0 * __ns))

#define Delay_us(__us) \
	if((unsigned long) (F_CPU/1000000.0 * __us) != F_CPU/1000000.0 * __us)\
		__builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000.0 * __us)+1);\
	else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000.0 * __us))

#define Delay_ms(__ms) \
	if((unsigned long) (F_CPU/1000.0 * __ms) != F_CPU/1000.0 * __ms)\
		__builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms)+1);\
	else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms))

#define Delay_s(__s) \
	if((unsigned long) (F_CPU/1.0 * __s) != F_CPU/1.0 * __s)\
		__builtin_avr_delay_cycles((unsigned long) ( F_CPU/1.0 * __s)+1);\
	else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1.0 * __s))

#else
#pragma message ("*****************  Warnings disabled!!!")

#	define Delay_ns(__ns)
#	define Delay_us(__us)
#	define Delay_ms(__ms)
#	define Delay_s(__s)
#endif

unsigned long millis(void);
unsigned long micros(void);

void TimingInit();

