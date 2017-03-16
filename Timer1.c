/*
 * File:   Timer1.c
 * Author: Hao
 * 
 */

#include <xc.h>
#include <stdint.h>
#include "Timer1.h"

uint32_t ms = 0;
uint32_t overflow_count = 0;

void ConfigTimer1(){
    T1CONbits.TON = 0;          // Disable Timer
    T1CONbits.TSIDL = 0;        // Continue timer operation in idle mode
    T1CONbits.TCS = 0;          // Timer clock source: Internal clock (FOSC/2)
    T1CONbits.TGATE = 0;        // Gated time accumulation disabled
    T1CONbits.TCKPS = 0b01;     // Timer Input Clock Prescale (1:8) FCY/8, one tick is 0.2004us
    TMR1 = 0x00;                // Clear timer register
    PR1 = 4990;                 // Load the period value, 4990 = 1ms/0.2004us
    
    IPC0bits.T1IP = 0x01;       // Set Timer1 Interrupt priority level
    IFS0bits.T1IF = 0;          // Clear Timer1 Interrupt flag
    IEC0bits.T1IE = 1;          // Enable Timer1 Interrupt
    
    T1CONbits.TON = 1;          // Start Timer
}

/* Timer 1 ISR*/
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void){
    IFS0bits.T1IF = 0;          // Clear Timer1 Interrupt Flag
    ms += 1;                // Interrupt every 1ms
    overflow_count += 1;        // Overflow every 1ms
}

uint32_t millis(){
    return ms;
}

double micros(){
    return ((overflow_count*PR1)+TMR1)*0.2004;
}



