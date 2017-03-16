/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */

/* Microcontroller MIPs (FCY) [FCY = F_OSC / 2] */
#define SYS_FREQ        79840000LL  // 79.84Mhz  System clock
#define FCY             SYS_FREQ/2  // 39.92Mhz  Instruction cycle

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration functions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigOscillator(void); /* Handles clock switching/OSC initialization */
