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

#include "stm8s.h"
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include "stdint.h"
#include "delay.h"
#include "nrf24_ble.h"

#include "uart.h"
#include <stdio.h>

uint8_t testdata[3] = {3, 1, 5};
char name[7] = "Marcel";

// Redirect stdout to UART
void putchar(char c)
{
	uart_write(c);
}

void main()
{
	// no main clock prescaler, so f_cpu= 16 MHz
	CLK_CKDIVR = 0;
	// onboard LED
	PB_DDR |= (1<<5);
	PB_CR1 |= (1<<5);

	uart_init();

	NRF24_BLE_setup();

	while (1)
	{
		if( !NRF24_BLE_advertise(name, sizeof(name)-1, testdata, sizeof(testdata)) )
		{
			PB_ODR ^= (1<<5); // toggle onboard LED
			delay_ms(BLE_BROADCASTING_INTERVAL);
		}
	}
}
