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

struct Pin;

static FORCE_INLINE volatile uint8_t* GetDDRRegFromPin(const Pin& pin);
static FORCE_INLINE volatile uint8_t* GetPINRegFromPin(const Pin& pin);


struct Pin
{
	volatile uint8_t* const port;   // pin's port physicall address
	uint8_t mask;                   // pin's bit
	volatile uint8_t* const timer;
	uint8_t timerBit;
	volatile uint16_t* const pwmReg;
	
	void FORCE_INLINE SetHigh() const { *port |= mask; }
	void FORCE_INLINE SetLow() const { *port &= ~mask; }

	void FORCE_INLINE SetDDRHigh() const { *GetDDRRegFromPin(*this) |= mask; }
	void FORCE_INLINE SetDDRLow() const { *GetDDRRegFromPin(*this) &= ~mask; }
		
	uint8_t FORCE_INLINE Get() const { return *GetPINRegFromPin(*this) & mask;}

	void FORCE_INLINE Set(const uint8_t data) const { if (data) *port |= mask; else *port &= ~mask; }
	void FORCE_INLINE SetPWM(const uint16_t pwm) const 
	{
		if (pwm == 0)
			SetLow();
		else if (pwm == 255)
			SetHigh();
		else
		if (timer != 0)
		{
			*timer |= timerBit;

			// 16 bit timers
			if( (timer == &TCCR3A) || (timer == &TCCR1A) || (timer == &TCCR1A))
				*pwmReg = pwm;
			else
				*(uint8_t*)pwmReg = pwm >> 8;
		}
		else if (pwm<128)
			SetLow();
		else 
			SetHigh();

	}
	void FORCE_INLINE Toggle() const { *port ^= ~mask; }
};


// pin desc

#define PINDESC_D0   { &PORTD, (1 << 2), 0,			0			, 0}		// D0 - PD2
#define PINDESC_D1   { &PORTD, (1 << 3), 0,			0			, 0}		// D1 - PD3
#define PINDESC_D2   { &PORTD, (1 << 1), 0,			0			, 0}		// D2 - PD1
#define PINDESC_D3   { &PORTD, (1 << 0), &TCCR0A,	1 << COM0B1 , (volatile uint16_t*)&OCR0B}		// D3 - PD0
#define PINDESC_D4   { &PORTD, (1 << 4), 0,			0			, 0}		// D4 - PD4
#define PINDESC_D5   { &PORTC, (1 << 6), &TCCR3A,	1 << COM3A1	, &OCR3A}		// D5 - PC6
#define PINDESC_D6   { &PORTD, (1 << 7), &TCCR4C,	1 << COM4D1	, (volatile uint16_t*)&OCR4D}		// D6 - PD7
#define PINDESC_D7   { &PORTE, (1 << 6), 0,			0			, 0}		// D7 - PE6
#define PINDESC_D8   { &PORTB, (1 << 4), 0,			0			, 0}		// D8 - PB4
#define PINDESC_D9   { &PORTB, (1 << 5), &TCCR1A,	1 << COM1A1	, &OCR1A}		// D9 - PB5
#define PINDESC_D10  { &PORTB, (1 << 6), &TCCR1A,	1 << COM1B1	, &OCR1B}		// D10 - PB6
#define PINDESC_D11  { &PORTB, (1 << 7), &TCCR0A,	1 << COM0A1	, (volatile uint16_t*)&OCR0A} 		// D11 - PB7
#define PINDESC_D12  { &PORTD, (1 << 6), 0,			0			, 0}		// D12 - PD6
#define PINDESC_D13  { &PORTC, (1 << 7), &TCCR4A,	1 << COM4A1	, (volatile uint16_t*)&OCR4A}		// D13 - PC7
#define PINDESC_D14  { &PORTB, (1 << 3), 0,			0			, 0}		// D14 - MISO - PB3
#define PINDESC_D15  { &PORTB, (1 << 1), 0,			0			, 0}		// D15 - SCK - PB1
#define PINDESC_D16  { &PORTB, (1 << 2), 0,			0			, 0}		// D16 - MOSI - PB2
#define PINDESC_D17  { &PORTB, (1 << 0), 0,			0			, 0}		// D17 - SS - PB0
#define PINDESC_D18  { &PORTF, (1 << 7), 0,			0			, 0}		// D18 - A0 - PF7
#define PINDESC_D19  { &PORTF, (1 << 6), 0,			0			, 0}		// D19 - A1 - PF6
#define PINDESC_D20  { &PORTF, (1 << 5), 0,			0			, 0}		// D20 - A2 - PF5
#define PINDESC_D21  { &PORTF, (1 << 4), 0,			0			, 0}		// D21 - A3 - PF4
#define PINDESC_D22  { &PORTF, (1 << 1), 0,			0			, 0}		// D22 - A4 - PF1
#define PINDESC_D23  { &PORTF, (1 << 0), 0,			0			, 0}		// D23 - A5 - PF0
#define PINDESC_D24  { &PORTD, (1 << 4), 0,			0			, 0}		// D24 / D4 - A6 - PD4
#define PINDESC_D25  { &PORTD, (1 << 7), 0,			0			, 0}		// D25 / D6 - A7 - PD7
#define PINDESC_D26  { &PORTB, (1 << 4), 0,			0			, 0}		// D26 / D8 - A8 - PB4
#define PINDESC_D27  { &PORTB, (1 << 5), 0,			0			, 0}		// D27 / D9 - A9 - PB5
#define PINDESC_D28  { &PORTB, (1 << 6), 0,			0			, 0}		// D28 / D10 - A10 - PB6
#define PINDESC_D29  { &PORTD, (1 << 6), 0,			0			, 0}		// D29 / D12 - A11 - PD6
																 
#define PINDESC_A0   PINDESC_D18
#define PINDESC_A1   PINDESC_D19
#define PINDESC_A2   PINDESC_D20
#define PINDESC_A3   PINDESC_D21
#define PINDESC_A4   PINDESC_D22
#define PINDESC_A5   PINDESC_D23
#define PINDESC_A6   PINDESC_D24
#define PINDESC_A7   PINDESC_D25
#define PINDESC_A8   PINDESC_D26
#define PINDESC_A9   PINDESC_D27
#define PINDESC_A10  PINDESC_D28
#define PINDESC_A11  PINDESC_D29


// pin defs for direct access

const Pin D0  = PINDESC_D0;		// D0 - PD2
const Pin D1  = PINDESC_D1;		// D1 - PD3
const Pin D2  = PINDESC_D2;		// D2 - PD1
const Pin D3  = PINDESC_D3;		// D3 - PD0
const Pin D4  = PINDESC_D4;		// D4 - PD4
const Pin D5  = PINDESC_D5;		// D5 - PC6
const Pin D6  = PINDESC_D6;		// D6 - PD7
const Pin D7  = PINDESC_D7;		// D7 - PE6
const Pin D8  = PINDESC_D8;		// D8 - PB4
const Pin D9  = PINDESC_D9;		// D9 - PB5
const Pin D10 = PINDESC_D10;	// D10 - PB6
const Pin D11 = PINDESC_D11;	// D11 - PB7
const Pin D12 = PINDESC_D12;	// D12 - PD6
const Pin D13 = PINDESC_D13;	// D13 - PC7
const Pin D14 = PINDESC_D14;	// D14 - MISO - PB3
const Pin D15 = PINDESC_D15;	// D15 - SCK - PB1
const Pin D16 = PINDESC_D16;	// D16 - MOSI - PB2
const Pin D17 = PINDESC_D17;	// D17 - SS - PB0
const Pin D18 = PINDESC_D18;	// D18 - A0 - PF7
const Pin D19 = PINDESC_D19;	// D19 - A1 - PF6
const Pin D20 = PINDESC_D20;	// D20 - A2 - PF5
const Pin D21 = PINDESC_D21;	// D21 - A3 - PF4
const Pin D22 = PINDESC_D22;	// D22 - A4 - PF1
const Pin D23 = PINDESC_D23;	// D23 - A5 - PF0
const Pin D24 = PINDESC_D24;	// D24 / D4 - A6 - PD4
const Pin D25 = PINDESC_D25;	// D25 / D6 - A7 - PD7
const Pin D26 = PINDESC_D26;	// D26 / D8 - A8 - PB4
const Pin D27 = PINDESC_D27;	// D27 / D9 - A9 - PB5
const Pin D28 = PINDESC_D28;	// D28 / D10 - A10 - PB6
const Pin D29 = PINDESC_D29;	// D29 / D12 - A11 - PD6


#define A0			D18
#define A1			D19
#define A2			D20
#define A3			D21
#define A4			D22
#define A5			D23
#define A6			D24
#define A7			D25
#define A8			D26
#define A9			D27
#define A10			D28
#define A11			D29



static FORCE_INLINE void PortConfigure(volatile uint8_t* const regOut, volatile uint8_t* const regMode, const uint8_t bit, const uint8_t pinMode, const bool useAtomics )
{
    const uint8_t mode = pinMode & ~(kPortConfig::PWMBit);
	
    if (kPortConfig::Input == mode)
    {
        BEGIN_ATOMIC_BLOCK_COND(useAtomics)
        {
            *regMode &= ~bit;
            *regOut &= ~bit;
        }
        END_ATOMIC_BLOCK_COND(useAtomics)
    }
    else if (kPortConfig::InputPullup == mode)
    {
        BEGIN_ATOMIC_BLOCK_COND(useAtomics)
        {
            *regMode &= ~bit;
            *regOut |= bit;
        }
        END_ATOMIC_BLOCK_COND(useAtomics)
    }
    else // kPortConfig::Output
    {
        BEGIN_ATOMIC_BLOCK_COND(useAtomics)
        {
            *regMode |= bit;
        }
        END_ATOMIC_BLOCK_COND(useAtomics)

    }
}

static FORCE_INLINE  volatile uint8_t* GetDDRRegFromPin(const Pin& pin)
{
	if ( &PORTB == pin.port )
		return &DDRB;
	else if ( &PORTC == pin.port )
		return &DDRC;
	else if ( &PORTD == pin.port )
		return &DDRD;
	else if ( &PORTE == pin.port )
		return &DDRE;
	//else if ( &PORTF == pin.port )
	
	return &DDRF;
}

static FORCE_INLINE  volatile uint8_t* GetPINRegFromPin(const Pin& pin)
{
	if ( &PORTB == pin.port )
		return &PINB;
	else if ( &PORTC == pin.port )
		return &PINC;
	else if ( &PORTD == pin.port )
		return &PIND;
	else if ( &PORTE == pin.port )
		return &PINE;
	//else if ( &PORTF == pin.port )
	
	return &PINF;
}

static FORCE_INLINE void PinConfigure(const Pin& pin, const uint8_t mode, const bool useAtomics = 1)
{
	PortConfigure(pin.port, GetDDRRegFromPin(pin), pin.mask, mode, useAtomics);
//    
//    if ( &PORTB == pin.port )
//        PortConfigure(&PORTB, &DDRB, pin.mask, mode);
//    else if ( &PORTC == pin.port )
//        PortConfigure(&PORTC, &DDRC, pin.mask, mode);
//    else if ( &PORTD == pin.port )
//        PortConfigure(&PORTD, &DDRD, pin.mask, mode);
//    else if ( &PORTE == pin.port )
//        PortConfigure(&PORTE, &DDRE, pin.mask, mode);
//    else if ( &PORTF == pin.port )
//        PortConfigure(&PORTF, &DDRF, pin.mask, mode);
}
