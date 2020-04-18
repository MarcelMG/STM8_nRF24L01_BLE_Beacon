/*
 * Copyright (C) 2012 Dmitry Grinberg
 *
 * http://dmitry.gr/index.php?r=05.Projects&proj=11.%20Bluetooth%20LE%20fakery
 *
 * Quote from Author on above Website:
 * " All the code as well as the research that went into this and is published
 * here is under this license: you may use it in any way you please if and
 * only if it is for non-commercial purposes, you must provide a link to this
 * page as well. Any commercial use must be discussed with me. "
 */

#include "nrf24_ble.h"
#include "nrf24_ble_hardware.h"
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include "delay.h"

#include "uart.h"

uint8_t chRf[3] = {2, 26,80}; // frequencies of BLE channels
uint8_t chLe[3] = {37,38,39}; // BLE RF channels
uint8_t buf[32] = {0}; // buffer for BLE packet
uint8_t ch = 0;  // RF channel for frequency hopping

/*
    arguments:
    ––––––––––
    name: advertising name, e.g. "NRF24"
    name_len: name length EXcluding '\0' terminator, i.e. e.g. sizeof("NRF24)-1 or strlen("NRF24")
    data: custom data
    data_len: length of custom data
*/
uint8_t NRF24_BLE_advertise(char* name, uint8_t name_len, uint8_t* data, uint8_t data_len)
{
	uint8_t i = 0;
	uint8_t L = 0;

	buf[L++] = 0x42;  //PDU type, given address is random; 0x42 for Android and 0x40 for iPhone

	// check if payload length is not exceeded
	if( ( name_len + data_len ) > 14 )
		return 1;
	buf[L++] = 6 + 3 + 2 + name_len + 2 + data_len; // length of payload

	buf[L++] = MY_MAC_0;
	buf[L++] = MY_MAC_1;
	buf[L++] = MY_MAC_2;
	buf[L++] = MY_MAC_3;
	buf[L++] = MY_MAC_4;
	buf[L++] = MY_MAC_5;

	buf[L++] = 0x02;   //flags (LE-only, limited discoverable mode)
	buf[L++] = 0x01;
	buf[L++] = 0x06;

	buf[L++] = name_len + 1; // length of the name, plus type byte
	buf[L++] = 0x08; // shortened name (else 0x09 for complete name)
	for(i=0; i < name_len; ++i)
	{
		buf[L++] = (uint8_t) name[i];
	}

	buf[L++] = data_len + 1;   // length of custom data, plus type byte
	buf[L++] = 0xFF; // data type byte, 0xFF is "Manufacturer Specific Data"
	for(i=0; i < data_len; ++i)
	{
		buf[L++] = data[i];
	}

	buf[L++] = 0x55;  //CRC start value: 0x555555
	buf[L++] = 0x55;
	buf[L++] = 0x55;

	nrf_cmd(0x25, chRf[ch]);
	nrf_cmd(0x27, 0x6E);  // Clear flags

	btLePacketEncode(buf, L, chLe[ch]);
	nrf_simplebyte(0xE2); //Clear RX Fifo
	nrf_simplebyte(0xE1); //Clear TX Fifo

	nrf_cs_low();
	nrf_simplebyte_no_cs(0xA0);
	for(i = 0 ; i < L ; i++)
	{
		nrf_simplebyte_no_cs(buf[i]);
	}
	nrf_cs_high();

	nrf_cmd(0x20, 0x12);  // TX on
	nrf_ce_high();
	delay_ms(1);
	nrf_ce_low();   // CE pin (PC3) LOW (in preparation of switching to RX quickly)
	// Channel hopping
	if(ch < 2)
		++ch;
	else
		ch = 0;
	return 0;
}

void NRF24_BLE_setup()
{
	// initialize hardware SPI peripheral
	nrf_spi_init();

// initialize nRF24L01+, setting general parameters
	nrf_cmd(0x20, 0x12);  //on, no crc, int on RX/TX done
	nrf_cmd(0x21, 0x00);  //no auto-acknowledge
	nrf_cmd(0x22, 0x00);  //no RX
	nrf_cmd(0x23, 0x02);  //4-byte address
	nrf_cmd(0x24, 0x00);  //no auto-retransmit
	nrf_cmd(0x26, 0x06);  //1MBps at 0dBm
	nrf_cmd(0x27, 0x3E);  //clear various flags
	nrf_cmd(0x3C, 0x00);  //no dynamic payloads
	nrf_cmd(0x3D, 0x00);  //no features
	nrf_cmd(0x31, 32);    //always RX 32 bytes
	nrf_cmd(0x22, 0x01);  //RX on pipe 0

	// Set access addresses (TX address in nRF24L01) to BLE advertising 0x8E89BED6
	// Remember that both bit and byte orders are reversed for BLE packet format
	// save resources by writing the result of the byte swap as Literal:
	buf[0] = 0x30;
	buf[1] = 0x71;
	buf[2] = 0x91;
	buf[3] = 0x7D;
	buf[4] = 0x6B;
// instead of writing:
//   buf[1] = swapbits(0x8E);
//   buf[2] = swapbits(0x89);
//   buf[3] = swapbits(0xBE);
//   buf[4] = swapbits(0xD6);

// Set access address
	nrf_manybytes(buf, 5);
	buf[0] = 0x2A;    // set RX address in nRF24L01, doesn't matter because RX is ignored in this case
	nrf_manybytes(buf, 5);
}

// calculate BLE CRC value
void btLeCrc(const uint8_t* data, uint8_t len, uint8_t* dst)
{
// implementing CRC with LFSR
	uint8_t v, t, d;

	while(len--)
	{
		d = *data++;
		for(v = 0; v < 8; v++, d >>= 1)
		{
			t = dst[0] >> 7;
			dst[0] <<= 1;
			if(dst[1] & 0x80)
				dst[0] |= 1;
			dst[1] <<= 1;
			if(dst[2] & 0x80)
				dst[1] |= 1;
			dst[2] <<= 1;

			if(t != (d & 1))
			{
				dst[2] ^= 0x5B;
				dst[1] ^= 0x06;
			}
		}
	}
}

// reverse the bit order in a single byte
uint8_t swapbits(uint8_t a)
{
	uint8_t v = 0;
	if(a & 0x80)
		v |= 0x01;
	if(a & 0x40)
		v |= 0x02;
	if(a & 0x20)
		v |= 0x04;
	if(a & 0x10)
		v |= 0x08;
	if(a & 0x08)
		v |= 0x10;
	if(a & 0x04)
		v |= 0x20;
	if(a & 0x02)
		v |= 0x40;
	if(a & 0x01)
		v |= 0x80;
	return v;
}

// Implementing whitening with LFSR
void btLeWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff)
{
	uint8_t  m;
	while(len--)
	{
		for(m = 1; m; m <<= 1)
		{
			if(whitenCoeff & 0x80)
			{
				whitenCoeff ^= 0x11;
				(*data) ^= m;
			}
			whitenCoeff <<= 1;
		}
		data++;
	}
}

uint8_t btLeWhitenStart(uint8_t chan)
{
//the value we actually use is what BT'd use left shifted one...makes our life easier
	return swapbits(chan) | 2;
}

// Assemble the packet to be transmitted
// Length is of packet, including crc. pre-populate crc in packet with initial crc value!
void btLePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan)
{
	uint8_t i;
	uint8_t dataLen = len - 3;
	btLeCrc(packet, dataLen, packet + dataLen);
	for(i = 0; i < 3; i++, dataLen++)
	{
		packet[dataLen] = swapbits(packet[dataLen]);
	}
	btLeWhiten(packet, len, btLeWhitenStart(chan));
	for(i = 0; i < len; i++)
	{
		packet[i] = swapbits(packet[i]); // the byte order of the packet should be reversed as well
	}
}
