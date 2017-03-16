/*
 * File:   ImpedanceController.c
 * Author: Hao
 *
 */

#include <math.h>
#include <stdbool.h>
#include <libpic30.h>
#include "PWM.h"
#include "ImpedanceController.h"


const float gear_ratio = 200;
const float tq_const = 37.0;        // mNm/A
double loop_time = 0.001;           // second
double rising_slew_rate = 20;       // unit/sec
double falling_slew_rate = -20;     // unit/sec
double impedance = 0;               // Nm
double desired_current = 0;         // Amp
float peak_current = 20.0;          // Amp
float max_duty_cycle = 0.2;         // Maximum duty cycle
float max_LS_duty_cycle = 0.25;     // Maximum LATE_STANCE duty cycle
float percent = 0,percent_new = 0,percent_old = 0;
bool flag_SW = false, flag_ES = true;       // Used at the beginning of SWING and EARLY_STANCE for easier foot clear and foot flat

enum states{
    EARLY_STANCE,
    MIDDLE_STANCE,
    LATE_STANCE,
    SWING,
    LATE_SWING,
};
enum states state = EARLY_STANCE;
struct st_impedance ankle_st_impedance;

/* This function calculates Impedance */
double Impedance(double angle, double velocity, float k, float b, float eq){
    // k: stiffness, b: Nm/deg/sec, eq: Equilibrium angle
    return -1*( k*(angle-eq) + b*velocity );
}

/* This function calculates desired current based on Impedance */
double DesiredCurrent(double Impedance){
    double desired_current;
    desired_current = Impedance/gear_ratio/tq_const/0.9*1000;
    return desired_current;
}

/* This function limits the rate of PWM duty cycle change */
float RateLimiter(float pc_old, float pc_new){
    float rate;
    rate = (pc_new - pc_old)/loop_time;     // pc: percentage
    if (rate > rising_slew_rate)
        pc_new = pc_old + loop_time*rising_slew_rate;
    else if (rate < falling_slew_rate)
        pc_new = pc_old + loop_time*falling_slew_rate;
    return pc_new;
}

struct st_impedance ankle_controller(float angle, float velocity, int16_t ac_x, float current_factor){
    /* STATE MODEL */
    switch (state){
        /* EARLY_STANCE control */
        case EARLY_STANCE:
            if ((angle >= -1)&&(velocity >= 5))         // ES -> MS transition threshold
            {
                state = MIDDLE_STANCE;
                break;
            }
            impedance = Impedance(angle, velocity, 0.8, 0.01, -10.0);       // Impedance calculation
            desired_current = DesiredCurrent(impedance);                    // Desired current based on Impedance
            percent = desired_current/peak_current;                         // PWM duty cycle
            if (percent >= max_duty_cycle)
                percent = max_duty_cycle;
            else if (percent <= - max_duty_cycle)
                percent = - max_duty_cycle;
            
            percent_new = RateLimiter(percent_old,percent);                 
            percent_old = percent_new;
            
            /* This block is added for easier foot flat */
            if (flag_ES == true){
                flag_ES = false;
                PF(0.2);
                int32_t p = 1000000;        // setup initial constant period for foot flat
                while (p--);
                break;
            }
            
            if (desired_current < 0)
                PF(-percent_new);
            else if (desired_current > 0)
                DF(percent_new);
            else
                Stop();
            break;
            
        /* MIDDLE_STANCE control */
        case MIDDLE_STANCE:
            if (angle >= 5)                 // MS -> LS transition threshold
            {
                state = LATE_STANCE;
                break;
            }
            impedance = Impedance(angle, velocity, 3.75, 0.01, -3.0);       // Impedance calculation
            desired_current = DesiredCurrent(impedance);                    // Desired current based on Impedance
            percent = desired_current/peak_current;                         // PWM duty cycle
            if (percent >= max_duty_cycle)
                percent = max_duty_cycle;
            else if (percent <= - max_duty_cycle)
                percent = - max_duty_cycle;
            
            percent_new = RateLimiter(percent_old,percent);                 
            percent_old = percent_new;
            
            if (desired_current < 0)
                PF(-percent_new*current_factor);
            else if (desired_current > 0)
                DF(percent_new*current_factor);
            else
                Stop();
            break;
        
        /* LATE_STANCE control */
        case LATE_STANCE:
            if (angle <= -18 && velocity >= -20)
            {
                state = SWING;
                flag_SW = true;
                break;
            }
            impedance = Impedance(angle, velocity, 1.8, 0.01, -20.0);
            impedance = ((impedance >= - 8.5)&&(impedance <= -5.4))? (-8.5): (impedance);
            desired_current = DesiredCurrent(impedance);                    // Desired current based on Impedance
            percent = desired_current/peak_current;                         // PWM duty cycle
            if (percent >= max_LS_duty_cycle)
                percent = max_LS_duty_cycle;
            else if (percent <= - max_LS_duty_cycle)
                percent = - max_LS_duty_cycle;
            
            percent_new = RateLimiter(percent_old,percent);                 
            percent_old = percent_new;
            
            if (desired_current < 0)
                PF(-percent_new*current_factor);
            else if (desired_current > 0)
                DF(percent_new*current_factor);
            else
                Stop();
            break;
            
        /* SWING control */    
        case SWING:
            if ((angle >= 1.5)&&(fabsf(velocity) <= 5))
            {
                state = LATE_SWING;
                break;
            }
            impedance = Impedance(angle, velocity, 0.7, 0.01, 5);
            impedance = ((impedance >= 6)&&(impedance <= 8.5))? (8.5): (impedance);
            desired_current = DesiredCurrent(impedance);                    // Desired current based on Impedance
            percent = desired_current/peak_current;                         // PWM duty cycle
            if (percent >= max_duty_cycle)
                percent = max_duty_cycle;
            else if (percent <= - max_duty_cycle)
                percent = - max_duty_cycle;
            
            percent_new = RateLimiter(percent_old,percent);                 
            percent_old = percent_new;
            
            /* This block is added for easier foot clear */
            if (flag_SW == true)
            {
                flag_SW = false;
                DF (0.2);
                int16_t p = 10000;              // setup initial constant period for foot clear
                while (p--);
                break;
            }
            
            if (desired_current < 0)
                PF(-percent_new);
            else if (desired_current > 0)
                DF(percent_new);
            else
                Stop();
            break;
            
        /* LATE_SWING control */
        case LATE_SWING:
            if (fabsf(ac_x)>= 32500)            // 32767 is 2g in IMU
            {
                state = EARLY_STANCE;
                flag_ES = true;
                break;
            }
            impedance = Impedance(angle, velocity, 0, 0, 2.0);
            desired_current = DesiredCurrent(impedance);                    // Desired current based on Impedance
            percent = desired_current/peak_current;                         // PWM duty cycle
            if (percent >= max_duty_cycle)
                percent = max_duty_cycle;
            else if (percent <= - max_duty_cycle)
                percent = - max_duty_cycle;
            
            percent_new = RateLimiter(percent_old,percent);                 
            percent_old = percent_new;
            
            if (desired_current < 0)
                PF(-percent_new);
            else if (desired_current > 0)
                DF(percent_new);
            else
                Stop();
            break;
    }
    ankle_st_impedance.st = state;
    ankle_st_impedance.impedance = impedance;
    ankle_st_impedance.percent_new = percent_new;
    return ankle_st_impedance;
}
