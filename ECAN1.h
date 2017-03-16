/* 
 * File:   ECAN.h
 * Author: Hao (based upon Jatinder Gharoo application note)
 * 
 */

#ifndef ECAN1_H
#define	ECAN1_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    /* CAN Baud Rate Configuration 		*/
    #define FCAN  	40000000        // 40 MHz
    #define BITRATE 1000000         // 1 Mbps bit rate
    #define NTQ 	20              // 20 Time Quanta in a Bit Time (between 8 to 25)
    #define BRP_VAL	((FCAN/(2*NTQ*BITRATE))-1)  // Baud Rate Prescaler

    /* ECAN message buffer length */
    #define ECAN1_MSG_BUF_LENGTH 4
    /* Reserve and align message buffer space in DMA RAM */
    typedef unsigned int ECAN1MSGBUF[ECAN1_MSG_BUF_LENGTH][8];
    extern ECAN1MSGBUF ecan1msgBuf __attribute__((space(dma)));
    
    /* message structure in RAM */
    typedef struct{
    /* Keep track of the buffer status */
    unsigned char buffer_status;
    /* RTR message or data message */
    unsigned char message_type;
    /* Frame type extended or standard */
    unsigned char frame_type;
    /* Buffer being used to send and receive messages */
    unsigned char buffer;
    /* 29 bit ID max of 0x1FFF FFFF */
    /* 11 bit ID max of 0x7FF */
    unsigned long id;
    unsigned char data[8];
    unsigned char data_length;
    }mID;
    
    /* ECAN message type identifiers */
    #define CAN_MSG_DATA 0x01
    #define CAN_MSG_RTR 0x02
    #define CAN_FRAME_EXT 0x03
    #define CAN_FRAME_STD 0x04
    #define CAN_BUF_FULL 0x05
    #define CAN_BUF_EMPTY 0x06

        /* Filter and mask defines */
    /* Macros used to write filter/mask ID to Register CiRXMxSID and CiRXFxSID */
    /* For example, to set up the filter to accept a value of 0x123, the macro when called as */
    /* CAN_FILTERMASK2REG_SID(0x123) will write the register space to accept message with ID 0x123 */
    /* Use for Standard Messages Only */
    #define CAN_FILTERMASK2REG_SID(x) ((x & 0x07FF)<<5)
    /* The Macro will set the MIDE bit in CiRXMxSID */
    #define CAN_SETMIDE(sid) (sid|0x0008)
    /* The Macro will set the EXIDE bit in CiRXFxSID to only accept extended messages */
    #define CAN_FILTERXTD(sid) (sid|0x0008)
    /* The macro will clear the EXIDE bit in CiRXFxSID to only accept standard messages */
    #define CAN_FILTERSTD(sid) (sid & 0xFFF7)
    /* Macro used to write filter/mask ID to Register CiRXMxSID, CiRXMxEID, CiRXFxSID and CiRXFxEID */
    /* For example, to set up the filter to accept a value of 0x123, the macro when called as */
    /* CAN_FILTERMASK2REG_SID(0x123) will write the register space to accept message with ID 0x123 */
    /* Use for Extended Messages only*/
    #define CAN_FILTERMASK2REG_EID0(x) (x & 0xFFFF)
    #define CAN_FILTERMASK2REG_EID1(x) (((x & 0x1FFC)<<3)|(x & 0x3))
    
    void ConfigECAN1();
    void ConfigDMAECAN1();
    void TxECAN1(mID *message);
    void RxECAN1(mID *message);
    void clearRxFlags(unsigned char buffer_number);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* ECAN1_H */

