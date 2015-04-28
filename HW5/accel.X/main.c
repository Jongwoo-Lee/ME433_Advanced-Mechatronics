/* 
 * File:   main.c
 * Author: Lee
 *
 * Created on April 18, 2015, 2:47 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "i2c_display.h"
#include "i2c_master_int.h"
#include <sys/attribs.h>
#include <xc.h>
#include "accel.h"

// DEVCFGs here

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF// no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins by turning sosc off
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_10// divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_16 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 40MHz
#pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON // allow only one reconfiguration
#pragma config IOL1WAY = ON // allow only one reconfiguration
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module


int main() {

    // string used for OLED
    char str[200];

    ANSELBbits.ANSB13 = 0; // make analog input digital
    U1RXRbits.U1RXR = 0b0000; // set U1RX to pin A2
    RPB15Rbits.RPB15R = 0b0101; // set B15 to U1TX

    TRISBbits.TRISB13 = 1;     // set up USER pin as input
    TRISBbits.TRISB7 = 0;    // set up LED1 pin as a digital output

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that
    // kseg0 is cacheable (0x3) or uncacheable (0x2)
    // see Chapter 2 "CPU for Devices with M4K Core"
    // of the PIC32 reference manual
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // no cache on this chip!

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
    DDPCONbits.JTAGEN = 0;

     // set up accelerometer
    acc_setup();

    __builtin_enable_interrupts();

    short accels[3];    // accelerations for the 3 axes
    short mags[3];      // magnetometer readings for the 3 axes
    short temp;         // temperature

    // set up OLED display
    display_init();

    while(1){
        // read the accelerometer from all three axes
        // the accelerometer and the pic32 are both little endian by default (the lowest address has the LSB)
        // the accelerations are 16-bit twos compliment numbers, the same as a short
        acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
        // need to read all 6 bytes in one transaction to get an update.
        acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);

        // read the temperature data. Its a right justified 12 bit two's compliment number
        acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);


        display_clear();
//      sprintf(str, "Hello world %d!", accels);

        int x_acc = accels[0]*64/16000;
        int y_acc = accels[1]*32/16000;
        int xxx,yyy;

        if (x_acc>=0){
            if(x_acc>64) x_acc=64;
            for (xxx=0;xxx<x_acc;xxx++){
                display_pixel_set(31,64-xxx,1);
                display_pixel_set(32,64-xxx,1);
                display_pixel_set(33,64-xxx,1);
            }
        }
        else{
            if(x_acc<-64) x_acc=-64;
            for (xxx=0;xxx>x_acc;xxx--){
                display_pixel_set(31,64-xxx,1);
                display_pixel_set(32,64-xxx,1);
                display_pixel_set(33,64-xxx,1);
            }
        }

        if (y_acc>=0){
            if(y_acc>32) y_acc=32;
            for (yyy=0;yyy<y_acc;yyy++){
                display_pixel_set(32-yyy,63,1);
                display_pixel_set(32-yyy,64,1);
                display_pixel_set(32-yyy,65,1);
            }
        }
        else{
            if(y_acc<-32) y_acc=-32;
            for (yyy=0;yyy>y_acc;yyy--){
                display_pixel_set(32-yyy,63,1);
                display_pixel_set(32-yyy,64,1);
                display_pixel_set(32-yyy,65,1);
            }
        }
        
        display_draw();
    }

    return 0;
}