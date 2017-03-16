/* 
 * File:   QEI.h
 * Author: Hao
 * 
 * This header file contains functions for Quadrature Encoder Interface configuration
 * and angle measurement using quadrature encoder
 */

#ifndef QEI_H
#define	QEI_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    void ConfigQEI();
    float AngleFromQEnc();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* QEI_H */

