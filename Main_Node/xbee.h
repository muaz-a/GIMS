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
#include "LCD.h"		// To show recieved Data, Not Needed
#include "sleep_lib.h"


#define ADDRESS_LENGTH 8
#define RXD_LENGTH 10
#define PACKET_LENGTH 30
#define FRAME_ADDRESS 3
#define DATA_ADDRESS 15

// Status used to determine what reciever is doing
typedef enum {
	RDY,
	OFFLINE,
	TIMEOUT,
	WAITING
} Status;

// NODE used for Reciever Information
typedef struct{
	uint8_t address[8];
	int index;
	Status status;
} NODE;
// Struct RXD used to hold Recieved data
typedef struct{
	uint8_t address[8];
	char data[20];
	int length;
}RXD;

extern volatile uint8_t alarm;
extern uint8_t buffer[30];
extern uint8_t bsize;
extern bool bfull;

// Initialize UART3, PB10(TXD) and PB11(RXD) for 9600 baud rate
// With recieve interrupt on
void init_usart(void);		

// Send Data through UART
// Main Node: Use xbeeSend to send data to Recievers
void send_usart(uint16_t sending);

// Recieve UART data
// Used in XbeeRecieve with interrupt handler
uint16_t recieve_usart(void);

// UART Interrupt Handler
// When a byte is recieved handler checks for starting deliminator
// once found stores packet of bytes in buffer 
// Sets flag when buffer is full and exits
void USART3_IRQHandler(void);

// XbeeSend takes uint8_t address array for Reciever
// int messge length, and char array of message to send
// For Main_Node: address to specific Reciever
// For Reciever Node: address = 0x 00 00 00 00 00 00 00 00 to send to Main_Node
// Creates a packet of data for API command with message and sends
void xbeeSend(uint8_t address[], int msg_leng,char *message);

// XbeeRecieve takes the buffer, buffersize and recieved struct
// Goes through packet and extracts data into recieved struct
// Struct includes: device #, data, data length.
void XbeeRecieve(RXD *recieved);

// XbeeSetUp initializes NODE struct with
// xbee addresses, index and status
void XbeeSetUp(NODE Devices[]);

#endif

