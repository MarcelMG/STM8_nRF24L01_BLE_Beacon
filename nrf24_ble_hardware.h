/*
MIT License

Copyright (c) 2020 Marcel Meyer-Garcia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#ifndef NRF24_BLE_HARDWARE_H
#define NRF24_BLE_HARDWARE_H

#include <stdint.h>
#include "stm8s.h"


#define CS_PORT_CR1 PC_CR1
#define CS_PORT_CR2 PC_CR2
#define CS_PORT_DDR PC_DDR
#define CS_PORT_ODR PC_ODR
#define CS_PIN 4

#define CE_PORT_CR1 PC_CR1
#define CE_PORT_CR2 PC_CR2
#define CE_PORT_DDR PC_DDR
#define CE_PORT_ODR PC_ODR
#define CE_PIN 3


void nrf_spi_init(void);
void nrf_cmd(uint8_t cmd, uint8_t data);
void nrf_simplebyte(uint8_t cmd);
void nrf_simplebyte_no_cs(uint8_t cmd);
void nrf_manybytes(uint8_t* data, uint8_t len);
inline void nrf_cs_low()
{
	CS_PORT_ODR &=~(1<<CS_PIN);
}
inline void nrf_cs_high()
{
	CS_PORT_ODR |= (1<<CS_PIN);
}
inline void nrf_ce_low()
{
	CE_PORT_ODR &=~(1<<CE_PIN);
}
inline void nrf_ce_high()
{
	CE_PORT_ODR |= (1<<CE_PIN);
}


#endif
