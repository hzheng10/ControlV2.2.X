/*
 * File:   QEI.c
 * Author: Hao
 * 
 */

#include <xc.h>
#include <float.h>

int POSCNTcopy = 0;                 // Variable for saving POSCNT(position counter)
int enc_resolution = 256;           // Motor Encoder resolution
float rev = 0, angle = 0;
int overflow =0;


void ConfigQEI(void)
{
    QEICONbits.QEIM = 0;            // Disable QEI Module
    QEICONbits.CNTERR = 0;          // Clear any count errors
    QEICONbits.QEISIDL = 0;         // Continue operation during sleep
    QEICONbits.SWPAB = 0;           // QEA and QEB not swapped
    QEICONbits.PCDOUT = 0;          // Normal I/O pin operation
    QEICONbits.POSRES = 1;          // Index pulse resets position counter
    DFLTCONbits.CEID = 1;           // Count error interrupts disabled
    DFLTCONbits.QEOUT = 1;          // Digital filters output enabled for QEn pins
    DFLTCONbits.QECK = 5;           // 1:64 clock divide for digital filter for QEn
    RPINR14bits.QEA1R = 6;          // RB6 as QEA1 PIN15
    RPINR14bits.QEB1R = 7;          // RB7 as QEB1 PIN16
    POSCNT = 0;                     // Reset position counter
    QEICONbits.QEIM = 6;            // x4 count resolution
}

float AngleFromQEnc()
{
    const float gear_ratio = 200;
    POSCNTcopy = (int)POSCNT;               // POSCNT: 16-bit counter (-32768 to 32767)
    rev = overflow*30+POSCNTcopy/(enc_resolution*4);    // No. of revs for motor
    if ( POSCNTcopy >= enc_resolution*4*30)         // Overflow over 30 turns in positive direction
    {
        overflow++;
        POSCNT = 0;                         // Reset position counter
    }
    else if ( POSCNTcopy <= enc_resolution*4*30)    // Overflow over 30 turns in negative direction
    {
        overflow--;
        POSCNT = 0;                         // Reset position counter
    }
    angle = -rev/gear_ratio*360;        // Angle calculation
    return angle;
}