/*
 * File:   main.c
 * Author: Hao
 * 
 * Ankle Orthosis Controller
 */

#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif
#endif

#include <stdint.h>        /* Includes uint16_t definition */
#include <stdbool.h>       /* Includes true/false definition */
#include <stdio.h>
#include <float.h>
#include "system_config.h"
#include <libpic30.h>
#include "ADC.h"
#include "UART.h"
#include "PWM.h"
#include "Timer1.h"
#include "ImpedanceController.h"
#include "QEI.h"
#include "MagEnc.h"
#include "i2c_emem.h"
#include "MPU6050.h"
#include "ECAN1.h"

ECAN1MSGBUF ecan1msgBuf __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));

int main(void) {
    const float gear_ratio = 200;       
    const float tq_const = 37;          // Motor torque constant (mNm/A)
    const float pi = 3.14;
    double cv, cv_filter;               // Filtered motor current voltage
    struct st_impedance ankle_st_impedance;
    double impedance;
    float mc = 0,mc_sum = 0, mc_thres = 70;  // mc: motor current
    double percent = 0;                 // PWM duty cycle
    double power = 0, torque = 0;       // Output power and torque
    double angle = 0, angle_old = 0,ankle_velocity;
    struct imu_data imuBase;            // IMU data
    extern I2CEMEM_DRV i2cmem;
    int16_t AcY1 = 0;
    
    /* System and Module configuration */
    ConfigOscillator();
    ConfigTimer1();
    ConfigADC();
    ConfigUART();
    printf("UART is ready \n");
    ConfigDIR();
    ConfigPWM();
    printf("PWM module is ready \n");
    ConfigQEI();
    printf("Quadrature Encoder is ready \n");
    ConfigMagEnc();
    printf("Magnetic Encoder is ready \n");
    i2cmem.init( &i2cmem );         // Initialize I2C peripheral and Driver
    ConfigImu();                    // Initialized the IMU and test the I2C Communication
    printf("I2C and IMU is ready \n");
    imuBase = calibrateImu();       // Calibrate the IMU data
    printf("Base data:");           
    printImuData (imuBase);         // Print base data
    ConfigECAN1();
    ConfigDMAECAN1();
    
    //SWITCH
    AD1PCFGLbits.PCFG1=1;           // Set pin 3 RA3/AN1 as digital pin
    TRISAbits.TRISA1 = 1;           // Set pin 3 RA1/AN1 as input
    cv_filter = Samp2Volt(ADSample(0)); // Motor current read from AN0(Pin2)
    angle_old = AngleFromQEnc();
    __delay_ms(100);
    IEC0bits.T1IE = 1;              // Enable Timer1 interrupt
    double ankle_v_filter = 0, ankle_v_radian = 0;
    float t_old = 0;
    float t_now = 0;
    float dt = 2e-3, time_sum = 0, mc_factor = 1;        // control cycle time in s
    float time_threshold_us = 200000;    //500 ms
    float a=0.85;                   // Parameter for digital filter
    int st;                         // State number
    const int ST_LATE_SWING = 4;
    bool odd = true;                // Used for data print
    
    while(1){
        angle = AngleFromQEnc();    // Ankle angle
        angle = a*angle_old+(1-a)*angle;    // Filtered ankle angle
        t_now = micros();
        dt = t_now - t_old;
        t_old = t_now;
        ankle_velocity = (angle - angle_old)/dt*1000000;    //deg/s
        ankle_v_filter = a*ankle_v_filter + (1-a)*ankle_velocity;
        ankle_v_radian = ankle_v_filter*pi/180;         // rad/s
        angle_old = angle;

        cv = Samp2Volt(ADSample(0));        // Read motor current voltage from AN0
        cv_filter = a*cv_filter+(1-a)*cv;
        mc = 6.4*cv_filter;                 // Motor current (A)

        // This is to control what data to log in.
        if (odd == true)
        {
            printf("%.1f,",angle);
            printf("%.1f,",ankle_v_filter);
            printf("%.1f,",impedance);
            odd = false;
        }
        else
        {
            printf("%d,",st);
            printf("%.1f,",torque);
            printf("%.1f\n",power);
            odd = true;
        }

        if (PORTAbits.RA1==0)           // Switch OFF motor if the pin is LOW 
        {
            Stop();                     // Stop the motor
            printf("Please Turn on USER Switch\n");
        }
        else
        {
            if (st == ST_LATE_SWING)
            {
                AcY1 = getAccelY();     // Read IMU value for heel strike
            }
            
            // Current limiting
            if (time_sum < time_threshold_us)
            {
                time_sum = time_sum + dt;
                if (mc > 3)
                {
                    mc_sum = mc + mc_sum;
                }
                if (mc_sum > mc_thres)
                {
                    mc_factor = 0.5;
                }
                else
                {
                    mc_factor = 1;
                }
            }
            else
            {
                time_sum = 0;
                mc_sum = 0;
            }

            ankle_st_impedance = ankle_controller(angle,ankle_v_filter,AcY1,mc_factor);
            st = ankle_st_impedance.st;
            impedance = ankle_st_impedance.impedance;
            percent = ankle_st_impedance.percent_new;
            torque = 0.9*percent*20*gear_ratio*tq_const/1000;
            power = 0.9*percent*20*gear_ratio*tq_const*ankle_v_radian/1000;
            
        }
    }
    return 0;
}
