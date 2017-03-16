/*
 * File:   ADC.c
 * Author: Hao
 *
 */

#include <xc.h>
#include "system_config.h"  // Include system configuration first then libpic30.h
#include <libpic30.h>       // To use delay
#include <float.h>
#include "ADC.h"


void ConfigADC() {
    // Configuration for ADC module
    AD1CON1bits.ADON=0;     // Make sure ADC is off when setting general config settings
    AD1CON1bits.AD12B=0;    // 10-bit sampling
    AD1CON1bits.FORM=0;     // Output format: unsigned integer between 0-1023 (10-bit)
    AD1CON1bits.SIMSAM=0;   // Sample multiple channels individually in sequence
    AD1CON1bits.ASAM=0;     // Start of sampling: Manual sampling
    AD1CON1bits.SSRC=0;     // Start of conversion (SOC) trigger selection: Manual trigger by clearing SAMP bit
    
    AD1CON2bits.VCFG=0;     // Use AVDD and AVSS for voltage reference (0V-3.3V)
    AD1CON2bits.CHPS=0;     // Channel selection: CH0
    AD1CON2bits.ALTS=0;     // Alternate Input Selection Mode Select bit
    
    /* ADC Clock Period: TAD = TCY * (ADCS + 1) = (1/40Mhz) * 3 = 75 ns (13.3 MHz)
     * ADC Conversion Time for 10-bit Tconv = 12 * TAD = 900 ns (1.1 MHz)"    */
    AD1CON3bits.ADRC=0;     // ADC conversion clock source bit: system clock
    AD1CON3bits.ADCS=2;     // ADC clock cycle divider
    
    // Configure Analog Port Pins (AD1PCFGL register is cleared at Reset)
    AD1PCFGLbits.PCFG0=0;   // AN0 as Analog Input
    AD1PCFGLbits.PCFG1=0;   // AN1 as Analog Input
    AD1PCFGLbits.PCFG2=0;   // AN2 as Analog Input
    AD1PCFGLbits.PCFG3=0;   // AN3 as Analog Input
    AD1PCFGLbits.PCFG4=0;   // AN4 as Analog Input
    AD1PCFGLbits.PCFG5=0;   // AN5 as Analog Input
    
    AD1CHS0bits.CH0NA=0;    // Select VREF-(which is AVSS) for CH0 -ve input
    AD1CON1bits.ADON=1;     // ADC module is on
    
}


unsigned int ADSample(unsigned int input_pin) {
    AD1CHS0bits.CH0SA=input_pin;    // Select ANx for +V input (input_pin 0 to 5)
    
    AD1CON1bits.SAMP=1;             // Start sampling
    __delay_ms(5);
    AD1CON1bits.SAMP=0;             // Start conversion
    while(!AD1CON1bits.DONE){
        // wait for conversion is done
    }
    return ADC1BUF0;                // Return conversion result (0-1024 for 10-bit)
}

double Samp2Volt(unsigned int sample) {
    // Return voltage value of sample acquired from ADC (10-bit)
    // Vref+ = 3.3V
    return sample*3.3/1024;
}