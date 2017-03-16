/*
 * File:   UART.c
 * Author: Hao
 * 
 */

#include <xc.h>
#include <stdint.h>
#include "system_config.h"
#if __XC16_VERSION < 1011
#warning "Please upgrade to XC16 v1.11 or newer."
#endif
#include "UART.h"

#define BAUDRATE 115200
#define BRGVAL ((FCY/BAUDRATE)/16)-1    // FCY = 40000000

/******************************************************************************
 * Function:   void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with transmit interrupt enabled.
 *
 * Overview:     UART transmit interrupt service routine called whenever a data
 *               is sent from UART Tx buffer
 *****************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _U1TXInterrupt( void )
{
    IFS0bits.U1TXIF = 0;
}

/******************************************************************************
 * Function:  void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with receive interrupt enabled.
 *
 * Overview:     UART receive interrupt service routine called whenever a byte
 *               of data received in UART Rx buffer.
 *****************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _U1RXInterrupt( void )
{
    LATA = U1RXREG;
    IFS0bits.U1RXIF = 0;
}

void ConfigUART() {
    U1MODEbits.UARTEN=0;        // Disable UART
    U1MODEbits.USIDL=0;         // Continue operation in idle mode
    U1MODEbits.IREN=0;          // IrDA encoder and decoder are disabled
    U1MODEbits.RTSMD=0;         // U1RTS is in Flow Control Mode
    U1MODEbits.UEN=0;           // U1TX and U1RX pins are enabled and used
    U1MODEbits.WAKE=0;          // Wake-up is disabled
    U1MODEbits.LPBACK=0;        // Loopback is disabled
    U1MODEbits.ABAUD=0;         // Baud rate measurement is disabled
    U1MODEbits.BRGH=0;          // BRG generated 16 clocks per bit period (standard speed mode)
    U1MODEbits.PDSEL=0;         // 8-bit data, no parity
    U1MODEbits.STSEL=0;         // 1 stop bit
    
    U1STAbits.UTXISEL1=0;       // UTXISEL bit 1
    U1STAbits.UTXISEL0=0;       // UTXISEL bit 0  Interrupt is generated when char is transferred
    U1STAbits.UTXINV=0;         // when IREN=0, U1TX idle state is 1
    U1STAbits.UTXBRK=0;         // Sync break transmission disabled
    U1STAbits.UTXBF=0;          // Transmit buffer status bit (read-only), 1: buffer is full, 0: empty
    U1STAbits.TRMT=0;           // Transmit shift register empty bit (read-only)
    U1STAbits.URXISEL=0;        // Interrupt is set when any char received and transferred
    U1STAbits.ADDEN=0;          // Address Detect Disabled
    U1STAbits.RIDLE=0;          // Receiver is active
    U1STAbits.PERR=0;           // Parity error status bit
    U1STAbits.FERR=0;           // Framing error status bit
    U1STAbits.OERR=0;           // Receive Buffer Overrun Error Status bit
    U1STAbits.URXDA=0;          // Receive Buffer Data Available bit
    
    /* Baud Rate Generator (U1BRG)
     * 
     * U1BRG = (FCY/(16*BaudRate))-1
     * U1BRG = (40M/(16*115200))-1 
     */
    U1BRG = BRGVAL;             // Baud rate set to 115200
    
    IPC3bits.U1TXIP=4;          // UART1 Transmit interrupt priority: mid-range
    IPC2bits.U1RXIP=4;          // UART1 Receive interrupt priority: mid-range
    IFS0bits.U1TXIF=0;          // Clear transmit interrupt flag
    IEC0bits.U1TXIE=1;          // Enable UART transmit interrupt
    IFS0bits.U1RXIF=0;          // Clear receive interrupt flag
    IEC0bits.U1RXIE=1;          // Enable UART receive interrupt
    
    //Peripheral pin select
    RPOR5bits.RP11R=3;          // RB11 as U1TX
    RPINR18bits.U1RXR=14;       // RB14 as U1RX
    
    U1MODEbits.UARTEN=1;        // Enable UART module
    U1STAbits.UTXEN=1;          // Enable transmission
}

