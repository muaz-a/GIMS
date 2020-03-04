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
	Device[0].address[0] = 0x00;
	Device[0].address[1] = 0x13;
	Device[0].address[2] = 0xA2;
	Device[0].address[3] = 0x00;
	Device[0].address[4] = 0x41;
	Device[0].address[5] = 0xB1;
	Device[0].address[6] = 0x06;
	Device[0].address[7] = 0x93; 
	Device[0].index = 1;
	Device[0].status = RDY;
	
	Device[1].address[0] = 0x00;
	Device[1].address[1] = 0x13;
	Device[1].address[2] = 0xA2;
	Device[1].address[3] = 0x00;
	Device[1].address[4] = 0x41;
	Device[1].address[5] = 0xB1;
	Device[1].address[6] = 0x15;
	Device[1].address[7] = 0x4B; 
	Device[1].index = 2;
	Device[1].status = RDY;
	
	struct RXD Recieved;		// To hold recieved data
	Recieved.device = 0;
	Recieved.length = 0;
	
	clockInit();
	init_usart();
	initLCD();
	
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

