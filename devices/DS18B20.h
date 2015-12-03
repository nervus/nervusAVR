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


#define kDS18B20UseAtomicRead	1
#define kDS18B20ResolutionBits	9

struct DS18B20
{
	struct Config { const Pin dq; };

	enum ROMCommands
	{
		SearchROM		= 0xf0,
		ReadROM			= 0x33,
		MatchROM		= 0x55,
		SkipROM			= 0xcc,
		AlarmSearch		= 0xec,
	};
	
	enum FunctionCommands
	{
		ConvertT		= 0x44,
		ReadScratchpad	= 0xbe,
		WriteScratchpad	= 0x4e,
		CopyScratchpad	= 0x48,
		RecallEEPROM	= 0xb8,
		ReadPowerSupply	= 0xb4,
	};

	//	All communication with the DS18B20 begins with an initialization sequence that consists of a reset pulse from the master 
	//	followed by a presence pulse from the DS18B20. When the DS18B20 sends the presence pulse in response to the reset, it is 
	//	indicating to the master that it is on the bus and ready to operate.  During the initialization sequence the bus master 
	//	transmits (TX) the reset pulse by pulling the 1-Wire bus low for a minimum of 480탎. The bus master then releases the bus 
	//	and goes into receive mode (RX). When the bus is released, the 5k? pull-up resistor pulls the 1-Wire bus high. When the 
	//	DS18B20 detects this rising edge, it waits 15탎 to 60탎 and then transmits a presence pulse by pulling the 1-Wire bus 
	//	low for 60탎 to 240탎.

	static uint8_t FORCE_INLINE Reset(const Config& ctx )
	{
		// Pull the 1-Wire bus low for a minimum of 480탎
		ctx.dq.SetLow(); 
		ctx.dq.SetDDRHigh(); // DDR high ( output )
 		Delay_us(480);
 		
 		// Release the bus and go into receive mode (RX).
		// When the DS18B20 detects this rising edge, it waits 15탎 to 60탎 and then transmits a presence pulse by pulling the 1-Wire bus
		ctx.dq.SetDDRLow();
 		Delay_us(60);

 		//get value and wait 420us
 		uint8_t i = ctx.dq.Get();
 		Delay_us(420);
		
		return i;
	}
	
	static void FORCE_INLINE WriteBit(const Config& ctx, const uint8_t mask )
	{
		// Pull the 1-Wire bus low for 1탎
		ctx.dq.SetLow();
		ctx.dq.SetDDRHigh(); // DDR high ( output )
		Delay_us(1);

		// release the line if !zero bit
		if(mask)
			ctx.dq.SetDDRLow();
			
 		// Release the bus and go into receive mode (RX).
 		Delay_us(60); // mircea@TODO: is this too much? 
 		ctx.dq.SetDDRLow();
	}
	
	static uint8_t FORCE_INLINE ReadBit(const Config& ctx)
	{
		// Pull the 1-Wire bus low for 1탎
		ctx.dq.SetLow();
		ctx.dq.SetDDRHigh(); // DDR high ( output )
		Delay_us(1);
		
		//release line and wait for 14킪
		ctx.dq.SetDDRLow();
		Delay_us(14);
		
		//read the value
		const register uint8_t bit = ctx.dq.Get();
			
		//wait 45킪 and return read value
		Delay_us(45);
		return bit;
	}
	
	static void FORCE_INLINE Write(const Config& ctx, const uint8_t mask )
	{
		WriteBit(ctx, mask&_BV(0));
		WriteBit(ctx, mask&_BV(1));
		WriteBit(ctx, mask&_BV(2));
		WriteBit(ctx, mask&_BV(3));
		WriteBit(ctx, mask&_BV(4));
		WriteBit(ctx, mask&_BV(5));
		WriteBit(ctx, mask&_BV(6));
		WriteBit(ctx, mask&_BV(7));
	}

	static uint8_t FORCE_INLINE Read(const Config& ctx)
	{
		uint8_t i=8, n=0;
		while(i--){
			n >>= 1;
			n |= (ReadBit(ctx)<<7);
		}
		return n;
	}

	static void FORCE_INLINE StartTemperatureConvertJob(const Config& ctx )
	{
		Reset(ctx);
  		Write(ctx, SkipROM);	// ROM command to start converting temperature
  		Write(ctx, ConvertT);	// start temperature conversion
	}
	
	static void FORCE_INLINE SyncTemperatureConvertJob(const Config& ctx)
	{
		while(!ReadBit(ctx)) {}
	}
		
	static int16_t FORCE_INLINE QueryTemperatureConvertJobOutput(const Config& ctx)
	{ 
		Reset(ctx);
		
		Write(ctx, SkipROM);		// ROM command to start converting temperature
		Write(ctx, ReadScratchpad);	// read scratchpad
		
		//read 2 byte from scratchpad
		uint16_t temperature_l = Read(ctx);
		uint16_t temperature_h = Read(ctx);
		
		return ( ( temperature_h << 8 ) + temperature_l ) >> 4;
	}	
	
#if 0
	/*
	 * write one bit
	 */
	void ds18b20_writebit(uint8_t bit){
		//low for 1uS
		DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
		DS18B20_DDR |= (1<<DS18B20_DQ); //output
		_delay_us(1);
		
		//if we want to write 1, release the line (if not will keep low)
		if(bit)
			DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
		
		//wait 60uS and release the line
		_delay_us(60);
		DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
	}
	
	/*
	 * read one bit
	 */
	uint8_t ds18b20_readbit(void){
		uint8_t bit=0;
		
		//low for 1uS
		DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
		DS18B20_DDR |= (1<<DS18B20_DQ); //output
		_delay_us(1);
		
		//release line and wait for 14uS
		DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
		_delay_us(14);
		
		//read the value
		if(DS18B20_PIN & (1<<DS18B20_DQ))
			bit=1;
		
		//wait 45uS and return read value
		_delay_us(45);
		return bit;
	}
	
	/*
	 * write one byte
	 */
	void ds18b20_writebyte(uint8_t byte){
		uint8_t i=8;
		while(i--){
			ds18b20_writebit(byte&1);
			byte >>= 1;
		}
	}
	
	/*
	 * read one byte
	 */
	uint8_t ds18b20_readbyte(void){
		uint8_t i=8, n=0;
		while(i--){
			n >>= 1;
			n |= (ds18b20_readbit()<<7);
		}
		return n;
	}
	
	/*
	 * get temperature
	 */
	double ds18b20_gettemp() {
		uint8_t temperature_l;
		uint8_t temperature_h;
		double retd = 0;
		
#if DS18B20_STOPINTERRUPTONREAD == 1
		cli();
#endif
		
		ds18b20_reset(); //reset
		ds18b20_writebyte(DS18B20_CMD_SKIPROM); //skip ROM
		ds18b20_writebyte(DS18B20_CMD_CONVERTTEMP); //start temperature conversion
		
		while(!ds18b20_readbit()); //wait until conversion is complete
		
		ds18b20_reset(); //reset
		ds18b20_writebyte(DS18B20_CMD_SKIPROM); //skip ROM
		ds18b20_writebyte(DS18B20_CMD_RSCRATCHPAD); //read scratchpad
		
		//read 2 byte from scratchpad
		temperature_l = ds18b20_readbyte();
		temperature_h = ds18b20_readbyte();
		
#if DS18B20_STOPINTERRUPTONREAD == 1
		sei();
#endif
		
		//convert the 12 bit value obtained
		retd = ( ( temperature_h << 8 ) + temperature_l ) * 0.0625;
		
		return retd;
	}
	
#endif
};


