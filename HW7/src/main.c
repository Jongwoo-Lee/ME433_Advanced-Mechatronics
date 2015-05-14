/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes
#include "i2c_display.h"
#include "i2c_master_int.h"
#include <sys/attribs.h>
#include <xc.h>
#include "accel.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );
    // string used for OLED
    char str[200];

    ANSELBbits.ANSB13 = 0; // make analog input digital
    U1RXRbits.U1RXR = 0b0000; // set U1RX to pin A2
    RPB15Rbits.RPB15R = 0b0101; // set B15 to U1TX

    TRISBbits.TRISB13 = 1;     // set up USER pin as input
    TRISBbits.TRISB7 = 0;    // set up LED1 pin as a digital output

    APP_USBDeviceEventHandler();
    
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

    display_init();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
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

        SYS_Tasks ( );

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

