/*
 * File:   MagEnc.c
 * Author: Hao
 *
 */

/*
    Austria Microsystems AS5145 and AS5045
    * The circuit (Datasheet AS5145 page 24)
    * GND power supply ground
    * DO connects to DataPin
    * CLK connects to ClockPin
    * CSn connects to ChipSelectPin
    * For 3.3V operation, 5V is connected with 3.3V. power supply
    * Pro connects to GND
    * 0.1uF capacitor is connected to pin 3.3V
 */

#include <xc.h>
#include <stdint.h>
#include "system_config.h"
#include <libpic30.h>
#include "MagEnc.h"

// Default constructor, SCLK connects to CLK, CSn to ChipSelect and DI to DO on AS5X45
void ConfigMagEnc(void)
{
	// Set pins up for output/input
    TRISAbits.TRISA2 = 0;       // Set pin 9 (RA2) as output CLK
    TRISAbits.TRISA4 = 0;       // Set pin 12 (RA4) as output CS
    AD1PCFGLbits.PCFG5=1;       // Set pin 7 RB3/AN5 as digital pins.
    TRISBbits.TRISB3 = 1;       // Set pin 7 RB3/AN5 as input DI
}

// Read position off sensor and return flags (wraps ASGetPosition for code size)
ASDataFlags GetDataFlags(void)
{
	//ASGetPosition();
    ASDataFlags Flags;
	GetPosition();
	return Flags;
}

// Read position off sensor, and update
int GetPosition(void)
{
	int tempPosition = 0;
    int Position = 0;
    int i = 0;
	uint8_t XOR = 0;            // XOR for parity check (Even Parity)
	uint8_t tempRead = 0;       // Temp reading byte
    int DataPrecision = 12;     // 12-bit data resolution
    LATAbits.LATA4 = 0;         // Set pin 12 (RA4) to digital/logic low CS
    ASDataFlags Flags;
    
    /* 12 data bits + 6 bits of data validity info */
	// Sensor feeds out position MSB first
	for(i = DataPrecision-1; i >= 0; i--)
	{
		// Toggle clock line, wait, and then read the data
		LATAbits.LATA2 = 0;     //digitalWrite(clkPin,LOW);
		// Delays must be at least 500ns
		__delay_us(1);
		LATAbits.LATA2 = 1;     //digitalWrite(clkPin,HIGH);
		__delay_us(1);
		tempRead = PORTBbits.RB3;//tempRead = digitalRead(dInPin)&0x01;
		// XOR data for checksum
		XOR ^= tempRead;
		// Read to tempPosition so we can buffer if data is invalid
		tempPosition |= (tempRead)<<i;
	}

	// Status bits are fed out
	for(i = 0; i < 6; i++)
	{
		// Toggle clock line, wait, and then read the data
		LATAbits.LATA2 = 0;     //digitalWrite(clkPin,LOW);
		__delay_us(1);
		LATAbits.LATA2 = 1;     //digitalWrite(clkPin,HIGH);
		__delay_us(1);
		tempRead = (PORTBbits.RB3)&0x01;//tempRead = digitalRead(dInPin)&0x01;
		// XOR data for checksum
		XOR ^= tempRead;
		// We don't buffer Flags, so user can check for errors
		Flags.data |= (tempRead)<<i;
	}

	// Disable chip select
	LATAbits.LATA4 = 1;         //digitalWrite(chipSelectPin, HIGH);

	// ValidityCheck
	if(XOR == 0 && Flags.OCF == 1 && Flags.COF == 0 && Flags.LIN == 0)
	{
		// if valid, return flags and update position
		Position=tempPosition;
	}
	return (Position);	// since i could never get LIN == 0 just let it go. the data is still valid.
}

float GetAngle()
{
    float degPerLSB = 0.088;        // 0.0879 degree resolution
    return (70.66-degPerLSB*GetPosition()); //dorsiflexion is positive
}