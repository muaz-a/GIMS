#include "xbee.h"

int main(){
	

	char message[] = "Test";			// Used for testing
	char message1[] = "Yass!";			// Used for testing
	int length = strlen(message);			// Used for testing
	int length2 = strlen(message1);			// Used for testing
	uint8_t cordaddress[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	// Used for Reciever to Main Node
	uint8_t usebuffer[30];
	uint8_t usebsize = 30;
	struct NODE Device[2];		// setup Device Struct with Addresses and index's

	
	struct RXD Recieved;		// To hold recieved data
	Recieved.device = 0;
	Recieved.length = 0;
	
	clockInit();
	init_usart();
	initLCD();
	
	XbeeSetUp(Device);		// Set up Device Struct with Xbee addresses, index and status
	
	// Sending a message to specific Xbee
	xbeeSend(Device[0].address, length, message);		// coordinator to Reciever 1
	xbeeSend(Device[0].address, length2, message1);		// coordinator to Reciever 1
	xbeeSend(cordaddress, length2, message1);				// Reciever to coordinator
	
	
	// Minimum main needed to recieve UART data from Xbee
	while(1)
	{
		if(bfull)
		{
			usebsize = bsize;		// copy data to avoid buffer being changed by interrupt
			for(int m=0; m<usebsize;m++)
			{
				usebuffer[m] = buffer[m];			
			}	
			XbeeRecieve(usebuffer, usebsize,&Recieved);		// Actual function to go through packet
			
		}
		
		
	}
}

