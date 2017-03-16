/* 
 * File:   PWM.h
 * Author: Hao
 * 
 * This header contains PWM configuration function declaration and functions
 * declaration for Dorsiflexion and Plantarflexion control
 */

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    void ConfigPWM();
    void ConfigDIR();
    void DF(float percent);
    void PF(float percent);
    void Stop();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* PWM_H */

