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


FORCE_INLINE void AttachToISR(const uint8_t slot, const uint8_t mode)
{
    if(slot < kAtmega::ExternalInterruptCount)
    {
        // __AVR_ATmega32U4__
        if (0 == slot)
        {
            EICRA = (EICRA & ~((1<<ISC00) | (1<<ISC01))) | (mode << ISC00);
            EIMSK |= (1<<INT0);
        }
        else if (1 == slot)
        {
            EICRA = (EICRA & ~((1<<ISC10) | (1<<ISC11))) | (mode << ISC10);
            EIMSK |= (1<<INT1);
        }
        else if (2 == slot)
        {
            EICRA = (EICRA & ~((1<<ISC20) | (1<<ISC21))) | (mode << ISC20);
            EIMSK |= (1<<INT2);
        }
        else if (3 == slot)
        {
            EICRA = (EICRA & ~((1<<ISC30) | (1<<ISC31))) | (mode << ISC30);
            EIMSK |= (1<<INT3);
        }
        else if (4 == slot)
        {
            EICRB = (EICRB & ~((1<<ISC60) | (1<<ISC61))) | (mode << ISC60);
            EIMSK |= (1<<INT6);
        }
    }
}


FORCE_INLINE void DetachFromISR(const uint8_t slot)
{
    if(slot < kAtmega::ExternalInterruptCount)
    {
        if (0 == slot)
            EIMSK &= ~(1<<INT0);
        else if (1 == slot)
            EIMSK &= ~(1<<INT1);
        else if (2 == slot)
            EIMSK &= ~(1<<INT2);
        else if (3 == slot)
            EIMSK &= ~(1<<INT3);
        else if (4 == slot)
            EIMSK &= ~(1<<INT6);
    }
}
