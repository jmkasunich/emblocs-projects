/***************************************************************
 *
 * stm32-crc.h - library for hardware accelerated CRC-16
 *               on STM32 MCUs - supports bundle.c protocol
 *
 */

#ifndef STM32_CRC16_H
#define STM32_CRC16_H

#include <stdint.h>

uint16_t bdl_crc16_stm32(uint16_t seed, const uint8_t *data, uint8_t len);

#endif // STM32_CRC16_H
