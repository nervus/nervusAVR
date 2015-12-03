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


// common

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#define FORCE_INLINE __attribute__((always_inline)) inline
#define NEVER_INLINE __attribute__ ((noinline))

#ifndef cbi
#	define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#	define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define BEGIN_ATOMIC_BLOCK_COND(cond) { uint8_t __backupSREG; if (cond) {__backupSREG = SREG; cli();}
#define END_ATOMIC_BLOCK_COND(cond) if (cond) {SREG = __backupSREG;} }

#define BEGIN_ATOMIC_BLOCK() { 	uint8_t __backupSREG = SREG; cli();
#define END_ATOMIC_BLOCK() SREG = __backupSREG; }


// custom
#include "Enums.h"
#include "Pins.h"
#include "ISR.h"
#include "Timing.h"
#include "devices/HD44780.h"
#include "devices/DS18B20.h"
