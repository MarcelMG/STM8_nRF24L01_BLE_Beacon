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
 *
 * Code adapted and modified in 2020 by Marcel Meyer-Garcia. All Rights remain with the original author.
 */
#ifndef NRF_24_BLE_H
#define NRF_24_BLE_H

#include <stdint.h>

// The MAC address of BLE advertizer -- just make one up
#define MY_MAC_0  0x11
#define MY_MAC_1  0x22
#define MY_MAC_2  0x33
#define MY_MAC_3  0x44
#define MY_MAC_4  0x55
#define MY_MAC_5  0x66

#define BLE_BROADCASTING_INTERVAL 250 // value in ms, must be >=100 (?)

extern uint8_t chRf[3];
extern uint8_t chLe[3];
extern uint8_t buf[32];
extern uint8_t ch;

uint8_t NRF24_BLE_advertise(char* name, uint8_t name_len, uint8_t* data, uint8_t data_len);
void NRF24_BLE_setup(void);
void btLeCrc(const uint8_t* data, uint8_t len, uint8_t* dst);
uint8_t swapbits(uint8_t a);
void btLeWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff);
uint8_t btLeWhitenStart(uint8_t chan);
void btLePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan);



#endif
