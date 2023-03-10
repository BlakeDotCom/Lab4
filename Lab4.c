/*
===============================================================================
 Name        : Lab4.c
 Author      : $(Blake Howell)
 Version     : 0
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>


// PINSEL to set pins 0.27&28 to sda and scl
#define PINSEL1 (*(volatile unsigned int *)  0x4002C004)
#define I2C0SCLH (*(volatile unsigned int *)  0x4001C010)
#define I2C0SCLL (*(volatile unsigned int *)  0x4001C014)
#define I2C0CONSET (*(volatile unsigned int *)  0x4001C000)
#define I2C0CONCLR (*(volatile unsigned int *)  0x4001C018)
#define I2C0DAT (*(volatile unsigned int *)  0x4001C008)    // use assignment not bitwise
#define I2C0STAT (*(volatile unsigned int *)  0x4001C004)
#define T0TC (*(volatile unsigned int *) 0x40004008)
#define T0TCR (*(volatile unsigned int *) 0x40004004)


// addresses for i2c devices
enum I2CAdr {
    DigitAdrR = 0b01000001,
    DigitAdrW = 0b01000000,
    DecimalAdrR = 0b01000011,
    DecimalAdrW = 0b01000010,
    TempAdrR = 0b10010001,
    TempAdrW = 0b10010000,
}
// I2C start condition
void start(void){
    I2C0CONSET = (1<<3);
    I2C0CONSET = (1<<5);
    I2C0CONCLR = (1<<3);
    while((I2C0CONSET >> 3) & 1 == 0);
    I2C0CONCLR = (1<<5);
}
// I2C stop condition
void stop(void){
    I2C0CONSET = (1<<4);
    I2C0CONCLR = (1<<3);
    while((I2C0CONSET>>4) & 1 == 1);
}
// i2c write sequence
void writeSeq(int data){
    I2C0DAT = data;
    I2C0CONCLR = (1<<3);
    while((I2C0CONSET >> 3) & 1 == 0);
}
// full i2c write to IO expanders
void writeIO(int op, int adr, int data1, int data2){
    start();
    writeSeq(op);
    writeSeq(adr);
    writeSeq(data1);
    writeSeq(data2);
    stop();
}
// i2c read sequence
int readSeq(void){
    I2C0CONCLR = (1<<3);
    while((I2C0CONSET >> 3) & 1 == 0)
    int ret = I2C0DAT;
    return ret;
}
// full i2c read to get temp
int readTemp(void){
    start();
    writeSeq(TempAdrW);
    writeSeq(0b00000000);
    start();
    writeSeq(TempAdrR);
    I2C0CONSET = (1<<2);
    int ret = (readSeq())<<8;
    I2C0CONCLR = (1<<2);
    ret += readSeq();
    stop();
    ret = ret>>5;
    return ret;
}
// full i2c read to get switch value
int readSw(void){
    start();
    writeSeq(DecimalAdrW);
    writeSeq(0x13);
    start();
    writeSeq(DecimalAdrR)
    I2C0CONCLR = (1<<2);
    int ret = readSeq();
    stop();
    return (ret & 1);
}
// wait function and checks if switched is pressed
int wait_us(int us){

    T0TC = 0;
    T0TCR |= (1<<0);
    int sw = 0;
    while((T0TC)<us){
        sw |= readSw();
    }
    T0TCR &= ~(1<<0);
    return sw;
}
int wait(float sec){
    return wait_us(sec*1000000);
}
// seven segment decoder
int segConvert(int val){
    switch (val){
        case 0:
            return 0b1111110;
        case 1:
            return 0b00001100;
        case 2:
            return 0b10110110;
        case 3:
            return 0b10011110;
        case 4:
            return 0b11001100;
        case 5: 
            return 0b11011010;
        case 6:
            return 0b11111010;
        case 7:
            return 0b00001110;
        case 8:
            return 0b11111110;
        case 9:
            return 0b11011110;
        case F:
            return 0b11100010;
        case C:
            return 0b01110010;
    }
}

int main(void) {
    // configure sda/scl output
    PINSEL1 |= (1<<22) | (1<<24);   // Set bits 22 & 24
    PINSEL1 &= ~(1<<23) & ~(1<<25); // Clear bits 23 & 25
    // Set I2C freq = 5KHz
    I2C0SCLH = 101;
    I2C0SCLL = 100;
    // reset subsystem state machine
    I2C0CONCLR = (1<<6);
    I2C0CONSET = (1<<6);
    //configure io expender to output
    writeIO(DecimalAdrW, 0, 0, 1);
    writeIO(DigitAdrW, 0, 0, 0);

    int isCelsius = 1;

    while(1){
        float temp = readTemp() * 0.125;    // temp in C
        temp = (isCelsius == 1) ? temp : (temp * 1.8 + 32); // convert to feren if needed
        int tens = segConvert(temp/10);
        int ones = segConvert(temp%10);
        int tenths = segConvert((temp * 10) % 10);
        int unit = segConvert((isCelsius == 1) ? (int)(C) : (int)(F));
        ones |= 1;   // set decimal point

        writeIO(DigitAdrW, 0x12, tens, ones);   
        writeIO(DecimalAdrW, 0x12, tenths, unit); // ask if writing to an input pin can cause issues
        int sw = wait(.7);
        if(sw != 0)
            isCelsius = (isCelsius == 1) ? 0 : 1;
    }

}