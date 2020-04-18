/*
 * delay utilite for STM8 family
 * COSMIC and SDCC
 * Terentiev Oleg
 * t.oleg@ymail.com
 */
#ifndef _UTIL_DELAY_H_
#define _UTIL_DELAY_H_ 1



#ifndef F_CPU
    #warning F_CPU is not defined! using 16MHz by default
    #define F_CPU 16000000UL
#endif

inline void delay_ms(unsigned short ms) {
    unsigned long i;
    for (i = 0; i < ((F_CPU / 18 / 1000UL) * ms); i++) {
        __asm__("nop");
    }
}

#endif

