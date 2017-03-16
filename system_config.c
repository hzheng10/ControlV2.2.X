/*
 * File:   SystemConfig.c
 * Author: Hao
 *
 * Created on March 4, 2017, 5:06 PM
 */

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif

#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "system_config.h"    /* variables/params used by system_config.c      */

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration functions, reset source evaluation         */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in SystemConfig.c                                    */
/*                                                                            */
/******************************************************************************/

/* Refer to the device Family Reference Manual Oscillator section for
information about available oscillator configurations.  Typically
this would involve configuring the oscillator tuning register or clock
switching using the compiler's __builtin_write_OSCCON functions.
Refer to the C Compiler for PIC24 MCUs and dsPIC DSCs User Guide in the
compiler installation directory /doc folder for documentation on the
__builtin functions.*/

// dsPIC33FJ128MC802 Configuration Bit Settings
#pragma config FNOSC = FRCPLL           // Oscillator Mode (Internal Fast RC (FRC) w/ PLL)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config POSCMD = NONE            // Primary Oscillator Source (Primary Oscillator Disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config PWMPIN = OFF             // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PWM module at device Reset)

/* TODO Add clock switching code if appropriate.  An example stub is below.   */
void ConfigOscillator(void)
{
// 7.37 MHz clock, 0.8MHz < Fin < 8MHz => 12.5MHz < Fosc < 80MHz
// Configure PLL prescaler, PLL postscaler, PLL divisor
// 7.37*(M/N1/N2)=79.84MHz
// F_OSC = 79.84MHz, Fcy = 39.92MHz

PLLFBD = 63;          // M = PLLFBD+2
CLKDIVbits.PLLPOST=0; // N2 = 2
CLKDIVbits.PLLPRE=1; // N1(1) = 3; N1(0) = 2
// Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
/*
__builtin_write_OSCCONH(0x01);          // Oscillator control register
__builtin_write_OSCCONL(OSCCON | 0x01);
// Wait for Clock switch to occur
while(OSCCONbits.COSC != 0b001) {};
// Wait for PLL to lock
while(OSCCONbits.LOCK!=1) {};
*/
}


