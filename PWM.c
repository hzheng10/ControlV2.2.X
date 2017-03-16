/*
 * File:   PWM.c
 * Author: Hao
 * 
 */

#include <xc.h>
#include <stdint.h>
#include "PWM.h"

void ConfigPWM(){
    /* ~~~~~~~~~~~~~~~~~~~~~~ PWM1 Configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    P1TCONbits.PTMOD=0b00;          // PWM Period Calculation for Free Running Count Mode
    P1TCONbits.PTCKPS=0b00;         // PWM time base input clock period is TCY (1:1 prescale)
    P1TCONbits.PTOPS=0b00;          // PWM time base output post scale is 1:1

    /* Choose PWM time period based on input clock selected */
    /* PWM switching frequency is 20 kHz */
    /* FCY is 39.92 MHz, Fpwm = 20 KHz, PxTMR prescaler = 1:1*/
    /* PxTPER = FCY/(Fpwm*PxTMR) - 1*/
    /* PxTPER = 39920000/(20000*1) - 1 = 1995*/
    P1TPER = 1995;
    
    /* PWM I/O pairs 1 to 3 are in complementary mode */
    /* PWM H3 (pin 21 RB10) is enabled for PWM output */
    PWM1CON1bits.PMOD1 = 0;
    PWM1CON1bits.PMOD2 = 0;
    PWM1CON1bits.PMOD3 = 0;
    PWM1CON1bits.PEN1H = 0;
    PWM1CON1bits.PEN2H = 0;
    PWM1CON1bits.PEN3H = 1;
    PWM1CON1bits.PEN1L = 0;
    PWM1CON1bits.PEN2L = 0;
    PWM1CON1bits.PEN3L = 0;
    
    /* Immediate update of PWM enabled */
    PWM1CON2bits.IUE = 1;
    
    /* PWM I/O pin controlled by PWM Generator */
    P1OVDCONbits.POVD3H = 1;
    P1OVDCONbits.POVD2H = 1;
    P1OVDCONbits.POVD1H = 1;
    P1OVDCONbits.POVD3L = 1;
    P1OVDCONbits.POVD2L = 1;
    P1OVDCONbits.POVD1L = 1;
    
    /* Initialize duty cycle values for PWM1, PWM2 and PWM3 signals */
    P1DC1 = 0;
    P1DC2 = 0;
    P1DC3 = 200;
    
    P1TCONbits.PTEN = 1;            // Enable PWM
}

void ConfigDIR(){
    // Set RB5 as open drain output for motor direction control
    ODCBbits.ODCB5 = 1;    // Turn on open drain feature
    LATBbits.LATB5 = 0;    // Set value to 0
    TRISBbits.TRISB5 = 1;  // Output 0

    // Set RB10 as open drain output for PWM
    ODCBbits.ODCB10 = 1;
}

void DF(float percent){
    uint16_t duty_cycle = 0;
    if (percent>=1)
        percent=1;
    duty_cycle = percent*3992;      // PxDCy value for 100% duty cycle: ((P1TPER+1)*2)
    TRISBbits.TRISB5 = 1;           // Pull RB5 up to HIGH
    P1DC3 = duty_cycle;             // Set duty cycle
}

void PF(float percent){
    uint16_t duty_cycle = 0;
    if (percent>=1)
        percent=1;
    duty_cycle = percent*3992;      // PxDCy value for 100% duty cycle: ((P1TPER+1)*2)
    TRISBbits.TRISB5 = 0;           // Sink RB5 to LOW
    P1DC3 = duty_cycle;             // Set duty cycle
}

void Stop(){
    P1DC3 = 0;
}