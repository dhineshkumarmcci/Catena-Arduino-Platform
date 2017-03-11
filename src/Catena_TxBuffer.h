/* Catena_TxBuffer.h	Sat Mar 11 2017 14:01:23 tmm */

/*

Module:  Catena_TxBuffer.h

Function:
	namespace McciCatena, class TxBuffer_t;

Version:
	V0.5.0	Sat Mar 11 2017 14:01:23 tmm	Edit level 2

Copyright notice:
	This file copyright (C) 2016-2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	December 2016

Revision history:
   0.4.0  Mon Dec  5 2016 00:02:11  tmm
	Module created.

*/

#ifndef _CATENA_TXBUFFER_H_		/* prevent multiple includes */
#define _CATENA_TXBUFFER_H_

#include <cstdint>

namespace McciCatena {

// build a transmit buffer
class TxBuffer_t
        {
private:
        uint8_t buf[32];   // this sets the largest buffer size
        uint8_t *p;
public:
        TxBuffer_t() : p(buf) {};
        void begin()
                {
                p = buf;
                }
        void put(uint8_t c)
                {
                if (p < buf + sizeof(buf))
                        *p++ = c;
                }
        void put1u(int32_t v)
                {
                if (v > 0xFF)
                        v = 0xFF;
                else if (v < 0)
                        v = 0;
                put((uint8_t) v);
                }
        void put2(uint32_t v)
                {
                if (v > 0xFFFF)
                        v = 0xFFFF;

                put((uint8_t) (v >> 8));
                put((uint8_t) v);
                }
        void put2(int32_t v)
                {
                if (v < -0x8000)
                        v = -0x8000;
                else if (v > 0x7FFF)
                        v = 0x7FFF;

                put2((uint32_t) v);
                }
        void put3(uint32_t v)
                {
                if (v > 0xFFFFFF)
                        v = 0xFFFFFF;

                put((uint8_t) (v >> 16));
                put((uint8_t) (v >> 8));
                put((uint8_t) v);
                }
        void put2u(int32_t v)
                {
                if (v < 0)
                        v = 0;
                else if (v > 0xFFFF)
                        v = 0xFFFF;
                put2((uint32_t) v);
                }
        void put3(int32_t v)
                {
                if (v < -0x800000)
                        v = -0x800000;
                else if (v > 0x7FFFFF)
                        v = 0x7FFFFF;
                put3((uint32_t) v);
                }
        uint8_t *getp(void)
                {
                return p;
                }
        size_t getn(void)
                {
                return p - buf;
                }
        uint8_t *getbase(void)
                {
                return buf;
                }
        void put2sf(float v)
                {
                int32_t iv;

                if (v > 32766.5f)
                        iv = 0x7fff;
                else if (v < -32767.5f)
                        iv = -0x8000;
                else
                        iv = (int32_t)(v + 0.5f);

                put2(iv);
                }
        void put2uf(float v)
                {
                uint32_t iv;

                if (v > 65535.5f)
                        iv = 0xffff;
                else if (v < 0.5f)
                        iv = 0;
                else
                        iv = (uint32_t)(v + 0.5f);

                put2(iv);
                }
        void put1uf(float v)
                {
                uint8_t c;

                if (v > 254.5)
                        c = 0xFF;
                else if (v < 0.5)
                        c = 0;
                else
                        c = (uint8_t) v;

                put(c);
                }
        void putT(float T)
                {
                put2sf(T * 256.0f + 0.5f);                
                }
        void putRH(float RH)
                {
                put1uf((RH / 0.390625f) + 0.5f);
                }
        void putV(float V)
                {
                put2sf(V * 4096.0f + 0.5f);
                }
        void putP(float P)
                {
                put2uf(P / 4.0f + 0.5f);
                }
        void putLux(float Lux)
                {
                put2uf(Lux);
                }
        };

/* the magic byte at the front of the buffer */
enum    {
        FormatSensor1 = 0x11,
        };

/* the flags for the second byte of the buffer */
enum    {
        FlagVbat = 1 << 0,
        FlagVcc = 1 << 1,
        FlagTPH = 1 << 2,
        FlagLux = 1 << 3,
        FlagWater = 1 << 4,
        FlagSoilTH = 1 << 5,
        };

} /* namespace McciCatena */

/**** end of Catena_TxBuffer.h ****/
#endif /* _CATENA_TXBUFFER_H_ */
