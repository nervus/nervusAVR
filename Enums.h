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


struct kAtmega
{
    enum atmega32u4
    {
        ExternalInterruptCount = 5,

        // Timers
        TIMER0A = 0,
        TIMER0B = 1,
        
        TIMER1A = 2,
        TIMER1B = 3,
        TIMER1C = 4,
    
        TIMER3A = 5,
        TIMER3B = 6,
        TIMER3C = 7,

        TIMER4A = 8,
        TIMER4B = 9,
        TIMER4D = 10,

    };
};


struct kTriggerMode
{
    enum
    {
        Change      = 0x1,
        FallingEdge = 0x2,
        RisingEdge  = 0x3
    };
};

struct kPortConfig
{
    enum
    {
        Input       = 0,
        Output      = 1,
        InputPullup = 2,
        PWMBit      = 1<<7
    };
};
