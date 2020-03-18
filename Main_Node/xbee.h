/*
Includes all functions related to Xbee's, and UART

*/



#ifndef XBEE_H
#define XBEE_H

#include <stdint.h>
#include "stm32f10x.h"
#include <stdbool.h>
#include <string.h>
#include "init_lib.h"	// To initialize STM standard clocks and Onboard LED's (Not needed if already have)
#include "LCD.h"		// To show received Data, Not Needed
#include "sleep_lib.h"


#define ADDRESS_LENGTH 8
#define RXD_LENGTH 10
#define PACKET_LENGTH 30
#define FRAME_ADDRESS 3
#define DATA_ADDRESS 15
#define LENGTH 8

// Status used to determine what receiver is doing
typedef enum {
	RDY,
	OFFLINE,
	TIMEOUT,
	WAITING
} Status;

// NODE used for Receiver Information
typedef struct{
	uint8_t address[ADDRESS_LENGTH];
	int index;
	Status status;
    uint8_t freq[LENGTH];
    uint8_t ampl[LENGTH];
		double Dfreq;
		double Dampl;
} NODE;
// Struct RXD used to hold Received data
typedef struct{
	uint8_t address[ADDRESS_LENGTH];
	char data[RXD_LENGTH];
	int length;
}RXD;

extern volatile uint8_t alarm;
extern volatile uint8_t buffer[PACKET_LENGTH];
extern uint8_t bsize;
extern bool bfull;

// Initialize UART3, PB10(TXD) and PB11(RXD) for 9600 baud rate
// With receive interrupt on
void init_usart(void);		

// Send Data through UART
// Main Node: Use xbeeSend to send data to Receivers
void send_usart(uint16_t sending);

// Receive UART data
// Used in XbeeReceive with interrupt handler
uint16_t receive_usart(void);

// UART Interrupt Handler
// When a byte is received handler checks for starting deliminator
// once found stores packet of bytes in buffer 
// Sets flag when buffer is full and exits
void USART3_IRQHandler(void);

// XbeeSend takes uint8_t address array for Receiver
// int messge length, and char array of message to send
// For Main_Node: address to specific Receiver
// For Receiver Node: address = 0x 00 00 00 00 00 00 00 00 to send to Main_Node
// Creates a packet of data for API command with message and sends
void xbeeSend(uint8_t address[], int msg_leng, char *message);

// XbeeReceive takes the buffer, buffersize and received struct
// Goes through packet and extracts data into received struct
// Struct includes: device #, data, data length.
void XbeeReceive(RXD *received);

// XbeeSetUp initializes NODE struct with
// xbee addresses, index and status
void XbeeSetUp(NODE Devices[]);

#endif

