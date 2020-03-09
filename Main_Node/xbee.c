#include "xbee.h"


// Globals used to pass buffer data from UART_IRQHandler
	uint8_t buffer[30];
	uint8_t bsize = 30;
	bool bfull = false;
	int bcount = 0;		// buffer index
	uint8_t ED1[ADDRESS_LENGTH] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93};		// Address of End Device 1
	uint8_t ED2[ADDRESS_LENGTH] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x15, 0x4B};		// Address of End Device 2

 void init_usart(void)
 {
	 // start clocks for ports A,B,C, the USART3 and AFIO
	 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; 
	 RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	 
		//GPIOB->CRH = 0x4B00;		//configure PB10 to AFIO PP Output and PB11 to Floating Input
	
	 GPIOB->CRH |= GPIO_CRH_MODE10;
	 GPIOB->CRH |= GPIO_CRH_CNF10_1;
	 GPIOB->CRH &= ~GPIO_CRH_CNF10_0;
	  
	 GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8 ;	// set PC8/9 into push-pull outputs
   GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8 ;
	 
	 USART3->CR1 |= USART_CR1_UE;
	 USART3->BRR = 0x9c4;
	 USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
	 USART3->CR1 |= USART_CR1_RXNEIE;
	 NVIC->ISER[1] |= NVIC_ISER_SETENA_7;
 }
 
 void send_usart(uint16_t sending)
 {
	 while((USART3->SR & USART_SR_TXE) != USART_SR_TXE)
	 {

	 }
	 	 USART3->DR = sending;
		 return;
 }
 
 uint16_t recieve_usart(void)
 {
	 
	 if((USART3->SR & USART_SR_RXNE) == USART_SR_RXNE)
	 {
		 return USART3->DR;
		 }
	 
 }
 
 void USART3_IRQHandler(void){

	 GPIOC->ODR |= GPIO_ODR_ODR8;		// set Blue LED onboard
	 buffer[bcount] = recieve_usart();		// enter first byte to buffer
	 if(buffer[0]==0x7e){						// check to make sure it is a packet
		 if (bcount==2){
			 bsize = buffer[bcount]+3;				// get the packet length
		 }
		 if(bcount==bsize){								// once at end of pecket
			 bfull = true;							// set buffer full flag
			 bcount=0;												// reset k counter

			 GPIOC->ODR &= 0xEF;				// reset Blue LED onboard

			 
		 }else{
			 bfull = false;							// if not at end increment counter
			bcount++;
		 }
	 }else {							// if first byte wasn't 0x7E reset counter
		 bcount = 0;
		 GPIOC->ODR &= 0xEF;				// reset Blue LED onboard
	 }	 
 }
 
 void xbeeSend(uint8_t address[], int msg_leng,char *message)
 {
	 // set up packet
	 uint8_t startdelim = 0x7E;			// starting deliminator
	 uint8_t plength = 0x0E + msg_leng;		// packet length (from byte 3 to checksum)
	 uint8_t frametype = 0x10;		// frame type (Trasmit Request)
	 uint8_t frameID = 0x00;			// Frame ID
	 uint16_t addsum = 0, messsum = 0;
	 for(int i = 0; i <ADDRESS_LENGTH;i++)
	 {
		 addsum = addsum + address[i];		// find sum of 64-bit address
	 }
	 uint8_t bit16add1 = 0xFF;			// 16-bit address FF FE
	 uint8_t bit16add2 = 0xFE;
	 	 for(int i = 0; i <msg_leng;i++)
	 {
		 messsum = messsum + message[i];	// find sum of message
	 }
	 
	 // Checksum used for ending of packet
	 uint8_t checksum = 0xFF - (frametype + frameID + addsum + bit16add1 + bit16add2 + messsum);		//addsum = 0x240,messsum = 0x1A0
													
		// send full packet sequence
	 send_usart(startdelim);
	 send_usart(0x00);
	 send_usart(plength);
	 send_usart(frametype);
	 send_usart(frameID);
	 	 for(int i = 0; i <ADDRESS_LENGTH;i++)
	 {
		 send_usart(address[i]);
	 }
	 send_usart(bit16add1);
	 send_usart(bit16add2);
	 send_usart(0x00);
	 send_usart(0x00);
	 	 	 for(int i = 0; i <msg_leng;i++)
	 {
		 send_usart(message[i]);
	 }
	 send_usart(checksum);
	 
	 return;
	 
	 
 }
 
void XbeeRecieve(uint8_t buffer[], int buffersize, struct RXD *recieved)
 {
	 	uint8_t frametype;
	 	uint8_t rxdRF[RXD_LENGTH];
		int rxdsize= 0;
	 	bool device1, device2;
		frametype = buffer[FRAME_ADDRESS];	 
	 
	 for(int i=0; i < ADDRESS_LENGTH;i++)// check sending address starting on byte 5 (index 4)
	 {			
			if(buffer[i+4]==ED1[i])// check for End Device 1 match, set flag if matches
				{		
					device1 = true;		// used for printing to LCD
					recieved->device = 1;
				} else 
				{
					device1 = false;	// used for printing to LCD
				}
			if (buffer[i+4] == ED2[i])// check for End Device 2 match, set flag if matches
				{		
					device2 = true;		// used for printing to LCD
					recieved->device = 2;
				} else
				{ 
					device2 = false;		// used for printing to LCD
				}
	}
	 
	if(frametype == 0x90)
		{
			for(int j=DATA_ADDRESS;j<buffersize;j++)
				{
					rxdRF[j-DATA_ADDRESS] = buffer[j];		// used for printing to LCD
					recieved->data[j-DATA_ADDRESS] = buffer[j];
					recieved->length++;
					rxdsize++;					// used for printing to LCD
				}
				
// Code used for Printing to LCD. Can be commented out				
			if(device1)
				{						// if device 1 matches, print to LCD
					commandToLCD(LCD_CLR);
					stringToLCD("End Device 1:");
				}
			if(device2)
				{						// if device 2 matches, print to LCD
					commandToLCD(LCD_CLR);
					stringToLCD("End Device 2:");
					
				}
			commandToLCD(LCD_LN2);		// print if pin9 was ON or OFF depending on IO flag
			for(int t=0; t<	rxdsize; t++)
				{
					dataToLCD(rxdRF[t]);
				}				
			
			commandToLCD(LCD_LN1);		// reset cursor and buffer full flag
			
	// Need these last lines			
				bfull= false;

		}
		
	 return;
 }
 
 void XbeeSetUp(struct NODE Devices[])
 {
	 for(int i =0; i < 2; i++)
	 {
		 for(int j = 0; j < ADDRESS_LENGTH; j++)
		 {
			 if(i==0)
			 {
					Devices[i].address[j] = ED1[j];			// copy address over
			 }
			 else Devices[i].address[j] = ED2[j];			// copy address over
		 }
		 Devices[i].status = OFFLINE;				// set to ready
		 Devices[i].index = i+1;				// set to address
	 }
		
 }
 
 