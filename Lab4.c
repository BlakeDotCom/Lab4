/*
===============================================================================
 Name        : Lab4.c
 Author      : $(Blake Howell, Jacob Weeks, Tanner Smith)
 Version     : 0
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>


// PINSEL to set pins 0.27&28 to sda and scl
#define PINSEL3 (*(volatile unsigned int *)  0x4002C004)
#define I2C0SCLH (*(volatile unsigned int *)  0x4001C010)
#define I2C0SCLL (*(volatile unsigned int *)  0x4001C014)
#define I2C0CONSET (*(volatile unsigned int *)  0x4001C000)
#define I2C0CONCLR (*(volatile unsigned int *)  0x4001C018)
#define I2C0DAT (*(volatile unsigned int *)  0x4001C008)    // use assignment not bitwise
#define I2C0STAT (*(volatile unsigned int *)  0x4001C004)

/* I2C ADDRESSES
        DIGIT = 0100000R/W
        DECIMAL = 0100001R/W
        TEMP = 1001000
*/
enum segConvert {
    one = 0b00001100
}



int main(void) {

    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
        // "Dummy" NOP to allow source level single
        // stepping of tight while() loop
        __asm volatile ("nop");
    }
    return 0 ;
}