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
#include <xc.h>

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );
    int val;
    // startup
    ANSELBbits.ANSB13 = 0; // make analog input digital
    U1RXRbits.U1RXR = 0b0000; // set U1RX to pin A2
    RPB15Rbits.RPB15R = 0b0101; // set B15 to OC1
    RPB8Rbits.RPB8R = 0b0101; // set B8 to OC2
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

    __builtin_enable_interrupts();

    // set up USER pin as input
    TRISBbits.TRISB13 = 1;
    // set up LED1 pin as a digital output
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB14 = 0;

    // set up LED2 as OC1 using Timer2 at 1kHz
    T2CONbits.TCKPS = 2; // prescaler N = 4  (10MHz)
    PR2 = 4999;          // 10000*4*25ns = 1ms = 1kHz
    TMR2 = 0;

    //set up oc1
    OC1RS = 2000;
    OC1R = 2000;
    OC1CONbits.OCTSEL = 0; // use timer 2, timer 3 = 1;
    OC1CONbits.OCM = 0b110; // PWM mode ; fault pin disabled

    //set up oc2
    OC2RS = 2000;
    OC2R = 2000;
    OC2CONbits.OCTSEL = 0;
    OC2CONbits.OCM = 0b110;

    //start PWM
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    OC2CONbits.ON = 1;

    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;


    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

