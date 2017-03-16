/* 
 * File:   MagEnc.h
 * Author: Hao
 * 
 * This header file contains function declarations for detecting positions using 
 * magnetic encoder (Austria Microsystems AS5145 and AS5045)
 */
  
#ifndef MAG_ENC_H
#define	MAG_ENC_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
    typedef union
    {
        uint8_t data;
        struct
        {
            uint8_t OCF : 1;
            uint8_t COF : 1;
            uint8_t LIN : 1;
            uint8_t MagInc : 1;
            uint8_t MagDec : 1;
            uint8_t EvenPar : 1;
        };
    } ASDataFlags;
    void ConfigMagEnc(void);
    int GetPosition(void);
    ASDataFlags GetDataFlags(void);
    float GetAngle(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* MAG_ENC_H */

