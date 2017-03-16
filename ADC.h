/* 
 * File:   ADC.h
 * Author: Hao
 *
 * This header contains function prototypes and constant definitions for
 * AD conversion module of the MCU
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    void ConfigADC();     // Must be called before ADSample() is used
    unsigned int ADSample(unsigned int input_pin);
    double Samp2Volt(unsigned int sample);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* ADC_H */