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

#include "nrf24_ble_hardware.h"
#include "stm8s.h"

// initialize SPI peripheral to communicate with nRF24L01(+)
void nrf_spi_init()
{
// CE pin (PC3) and CS pin (PC4) as fast push-pull outputs
	CS_PORT_ODR |= (1<<CS_PIN);
	CS_PORT_DDR |= (1<<CS_PIN);
	CS_PORT_CR1 |= (1<<CS_PIN);
	CS_PORT_CR2 |= (1<<CS_PIN);

	CE_PORT_ODR |= (1<<CE_PIN);
	CE_PORT_DDR |= (1<<CE_PIN);
	CE_PORT_CR1 |= (1<<CE_PIN);
	CE_PORT_CR2 |= (1<<CE_PIN);

	// configure SPI
	// MSB First, BAUDRATE_PRESCALER=011, Master, CPOL=0, CPHA=0
	SPI_CR1 = (uint8_t) ((0<<SPI_CR1_LSBFIRST) | (0b011<<SPI_CR1_BR0) | (1<<SPI_CR1_MSTR) | (0<<SPI_CR1_CPOL) | (0<<SPI_CR1_CPHA));
	SPI_CR2 = (1<<SPI_CR2_SSM) | (1<<SPI_CR2_SSI); // software Chip Select (CS a.k.a. NSS)
	SPI_CR1 |= (1<<SPI_CR1_SPE);
}
// Write to nRF24's register
void nrf_cmd(uint8_t cmd, uint8_t data)
{
	nrf_cs_low();
	SPI_DR = cmd;
	while (!(SPI_SR & (1 << SPI_SR_TXE))); /* wait until TX buffer empty */
	while (!(SPI_SR & (1 << SPI_SR_RXNE))); /* wait until RX buffer not empty */
	SPI_DR; /* dummy read SPI data register to clear RXNE bit */
	SPI_DR = data;
	while (!(SPI_SR & (1 << SPI_SR_TXE))); /* wait until TX buffer empty */
	while (!(SPI_SR & (1 << SPI_SR_RXNE))); /* wait until RX buffer not empty */
	SPI_DR; /* dummy read SPI data register to clear RXNE bit */
	nrf_cs_high();
}
// send one byte via SPI
void nrf_simplebyte(uint8_t cmd)
{
// transfer only one byte
	nrf_cs_low();
	SPI_DR = cmd;
	while (!(SPI_SR & (1 << SPI_SR_TXE))); /* wait until TX buffer empty */
	while (!(SPI_SR & (1 << SPI_SR_RXNE))); /* wait until RX buffer not empty */
	SPI_DR; /* dummy read SPI data register to clear RXNE bit */
	nrf_cs_high();
}
// shift out one byte via SPI without pulling the CS line
void nrf_simplebyte_no_cs(uint8_t cmd)
{
	SPI_DR = cmd;
	while (!(SPI_SR & (1 << SPI_SR_TXE))); /* wait until TX buffer empty */
	while (!(SPI_SR & (1 << SPI_SR_RXNE))); /* wait until RX buffer not empty */
	SPI_DR; /* dummy read SPI data register to clear RXNE bit */
}
// transfer several bytes via SPI
void nrf_manybytes(uint8_t* data, uint8_t len)
{
	nrf_cs_low();
	do
	{
		SPI_DR = (*data++);
		while (!(SPI_SR & (1 << SPI_SR_TXE))); /* wait until TX buffer empty */
		while (!(SPI_SR & (1 << SPI_SR_RXNE))); /* wait until RX buffer not empty */
		SPI_DR; /* dummy read SPI data register to clear RXNE bit */
	}
	while(--len);
	nrf_cs_high();
}
