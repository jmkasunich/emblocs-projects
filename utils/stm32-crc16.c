/***************************************************************
 *
 * stm32-crc.h - library for hardware accelerated CRC-16
 *               on STM32 MCUs - supports bundle.c protocol
 *
 */

#include "stm32-crc16.h"
#include "stm32g4xx.h"

#ifndef CRC_INIT
#error "Enhanced CRC peripheral required"
#endif

#define CRC_POLYSIZE_16  0x00000008
#define CRC_RESET        0x00000001

uint16_t bdl_crc16_stm32(uint16_t seed, const uint8_t *data, uint8_t len)
{
    const uint32_t *wp;
    const uint8_t *bp;

    // set polynomial and seed
    CRC_POL = 0x00001021;
    CRC_INIT = (uint32_t)(seed);
    // set 16-bit CRC and reset CRC generator to load seed
    CRC_CR = CRC_POLYSIZE_16 | CRC_RESET;
    // deal with possibly unaligned buffer
    bp = data;
    while ((((uintptr_t)bp & 0x3) != 0) && (len > 0)) {
        CRC_DR = *(bp++);
        len--;
    }
    // process 32-bit chunks
    wp = (const uint32_t *)bp;
    while ( len >= 4 ) {
        CRC_DR = *(wp++);
        len -= 4;
    }
    // process remaining bytes
    bp = (const uint8_t *)wp;
    while ( len > 0 ) {
        CRC_DR = *(bp++);
        len--;
    }
    // return result
    return (uint16_t)CRC_DR;
}
