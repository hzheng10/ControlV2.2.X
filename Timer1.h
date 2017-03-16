/* 
 * File:   Timer1.h
 * Author: Hao
 * 
 * This header contains Timer1 configuration and micros and milli function for 
 * getting time step
 */

#ifndef TIMER1_H
#define	TIMER1_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    void ConfigTimer1();
    uint32_t millis();
    double micros();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* TIMER1_H */

