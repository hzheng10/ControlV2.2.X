/* 
 * File:   ImpedanceController.h
 * Author: Hao
 * 
 * This header file contains functions for calculating Impedance, Desired current,
 * Output Rate Limiter and finite state model controller
 */

#include <stdint.h>
#ifndef IMPEDANCE_CONTROLLER_H
#define	IMPEDANCE_CONTROLLER_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    double Impedance(double angle, double velocity, float k, float b, float eq);
    double DesiredCurrent(double Impedance);
    float RateLimiter(float pc_old, float pc_new);
    
    struct st_impedance
    {
        int  st;
        float impedance;
        float percent_new;
    };
    
    struct st_impedance ankle_controller(float angle, float velocity, int16_t ac_x, float current_factor);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* IMPEDANCE_CONTROLLER_H */
