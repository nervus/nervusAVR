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
 * Notes:
 *    Adapted from HD44780 Library v1.11 by SA Development 
 *    (http://community.atmel.com/projects/hd44780-library)
 *
 * Changes:
 *    Converted to propagate constants as much as to avoid branch generation
 *
 ******************************************************************************/

#pragma once

#define kLCDResetDelayMS		16

struct HD44780
{
	struct Config { const Pin rs, en, db4, db5, db6, db7; };
	
	enum Commands
	{
		CLEARSCREEN         = 0,    // DB0: clear display
		RETURNHOME          = 1,    // DB1: return to home position
		ENTRY_MODE          = 2,    // DB2: set entry mode
		ENTRY_INC           = 1,    // DB1: 1=increment, 0=decrement
		ENTRY_SHIFT         = 0,    // DB0: 1=display shift on
		DISPLAYMODE         = 3,    // DB3: turn lcd/cursor on
		DISPLAYMODE_ON      = 2,    // DB2: turn display on
		DISPLAYMODE_CURSOR  = 1,    // DB1: turn cursor on
		DISPLAYMODE_BLINK   = 0,    // DB0: blinking cursor
		MOVE                = 4,    // DB4: move cursor/display
		MOVE_DISP           = 3,    // DB3: move display (0-> cursor)
		MOVE_RIGHT          = 2,    // DB2: move right (0-> left)
		FUNCTION            = 5,    // DB5: function set
		FUNCTION_8BIT       = 4,    // DB4: set 8BIT mode (0->4BIT mode)
		FUNCTION_2LINES     = 3,    // DB3: two lines (0->one line)
		FUNCTION_10DOTS     = 2,    // DB2: 5x10 font (0->5x7 font)
		CGRAM               = 6,    // DB6: set CG RAM address
		DDRAM               = 7,    // DB7: set DD RAM address
		BUSY                = 7,    // DB7: LCD is busy
	};
	
	static FORCE_INLINE void ReturnHome(const Config& ctx) { Write(ctx, _BV(RETURNHOME), 1); }
	static FORCE_INLINE void ClearScreen(const Config& ctx) { Write(ctx, _BV(CLEARSCREEN), 1); }
	static FORCE_INLINE void SetPosition(const Config& ctx, const uint8_t pos) { Write(ctx, _BV(DDRAM) + pos, 1); }
	static FORCE_INLINE void PrintChar(const Config& ctx, const char c) { Write(ctx, c); }
	static NEVER_INLINE void PrintString(const Config& ctx, const char* s)
	{
		register char c;
		while ((c=*s++))
			Write(ctx, c);
	}
	static NEVER_INLINE void PrintInt(const Config& ctx, const long i)
	{
		register char c[16];
		PrintString(ctx, itoa(i, c, 10));
	}
	
	static FORCE_INLINE void PulseENLine(const Config& ctx)
	{
		Delay_ns(100);
		ctx.en.SetHigh();
		Delay_ns(500);
		ctx.en.SetLow();
	}
	
	static FORCE_INLINE void PrintStringFromFlash(const Config& ctx, const char* progmem_s)
	{
		register char c;
		while ((c=pgm_read_byte(progmem_s++)))
			Write(ctx, c);
	}
	
	static void NEVER_INLINE Write(const Config& ctx, const uint8_t data, const uint8_t isCmd = 0)
	{

		// output command or data descriptor
		ctx.rs.Set(!isCmd);

		//Output High Nibble
		ctx.db7.Set(data&_BV(7));
		ctx.db6.Set(data&_BV(6));
		ctx.db5.Set(data&_BV(5));
		ctx.db4.Set(data&_BV(4));
		
		PulseENLine(ctx);
		
		//Output Low Nibble
		ctx.db7.Set(data&_BV(3));
		ctx.db6.Set(data&_BV(2));
		ctx.db5.Set(data&_BV(1));
		ctx.db4.Set(data&_BV(0));
		
		PulseENLine(ctx);
		
		// All Data Pins High (Inactive)
		ctx.db7.SetHigh();
		ctx.db6.SetHigh();
		ctx.db5.SetHigh();
		ctx.db4.SetHigh();
		
		// some commands require a lot of delay
		if (isCmd && data<=(_BV(CLEARSCREEN) | _BV(RETURNHOME)))
			Delay_us(1640);
		else
			Delay_us(40);
	}
	
	static FORCE_INLINE void Setup(const Config& ctx, const uint8_t lcdDisplayLines = 2)
	{
		PinConfigure(ctx.rs,  kPortConfig::Output);
		PinConfigure(ctx.en,  kPortConfig::Output);
		PinConfigure(ctx.db4, kPortConfig::Output);
		PinConfigure(ctx.db5, kPortConfig::Output);
		PinConfigure(ctx.db6, kPortConfig::Output);
		PinConfigure(ctx.db7, kPortConfig::Output);

		//Set All Control Lines Low
		ctx.en.SetLow();
		ctx.rs.SetLow();

		//Set All Data Lines High
		ctx.db4.SetHigh();
		ctx.db5.SetHigh();
		ctx.db6.SetHigh();
		ctx.db7.SetHigh();
		
		//Startup Delay
		Delay_ms(kLCDResetDelayMS);
		
		//Initialize Display
		ctx.db7.SetLow();
		ctx.db6.SetLow();
		PulseENLine(ctx);
		Delay_us(4100);

		ctx.en.SetHigh();
		Delay_ns(500);
		
		ctx.en.SetLow();
		PulseENLine(ctx);
		Delay_us(40);

		//configure for 4-bit interface
		ctx.db4.SetLow();
		PulseENLine(ctx);
		Delay_us(40);
		
		ctx.db4.SetLow();
		PulseENLine(ctx);
		Delay_ns(500);
		
		//if (lcdDisplayLines > 1)
			ctx.db7.SetHigh();
//			ctx.db7.SetLow();
		
		PulseENLine(ctx);
		Delay_us(40);
		
		//Display Off
		Write(ctx, _BV(DISPLAYMODE), 1);
		
		//Display Clear
		ClearScreen(ctx);
		
		//Entry Mode Set
		Write(ctx, _BV(ENTRY_MODE) | _BV(ENTRY_INC), 1);
		
		//Display On
		Write(ctx, _BV(DISPLAYMODE) | _BV(DISPLAYMODE_ON), 1);
	}
};


