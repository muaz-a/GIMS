

#include "main.h"

int main(void)
{
	uint8_t recieve;
	uint8_t pac[25];
	uint8_t ED1[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93};		// Address of End Device 1
	uint8_t ED2[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x15, 0x4B};		// Address of End Device 2
	uint8_t rxdRF[10];
	int rxdsize= 0;
	uint8_t usebuffer[30];
	uint8_t usebsize = 30;
	uint8_t frametype;
	uint16_t switches = 0;
	bool device1, device2, IO;
	int i=0,j=0;
	uint8_t sending = 0x21;
	uint16_t send[2] = {0xa4, 0x87};
	int k = 0, m = 0, t = 0;
		
	clockInit();
	init_usart();
	initLCD();
	led_IO_init();
	
	
while(1)
{
	if(bfull){						// check for buffer full flag
		
		usebsize = bsize;
		for(m=0; m<usebsize;m++){
			usebuffer[m] = buffer[m];
		}
		frametype = usebuffer[3];
		
				
		for(i=0; i < 8;i++){			// check sending address starting on byte 5 (index 4)
			if(usebuffer[i+4]==ED1[i]){		// check for End Device 1 match, set flag if matches
					device1 = true;
					} else {
						device1 = false;
					}
						if (usebuffer[i+4] == ED2[i]){		// check for End Device 2 match, set flag if matches
					device2 = true;
					} else{ 
						device2 = false;
					}
			}
				
			if(frametype == 0x92){
						if(usebuffer[19]==0x01){			// check IO data from byte 20 (index 19)
							IO=true;							// set flag for High, reset for LOW
						}else {
							IO =false;
						}
						if(device1){						// if device 1 matches, print to LCD
							commandToLCD(LCD_CLR);
							stringToLCD("End Device 1:");
						}
						if(device2){						// if device 2 matches, print to LCD
							commandToLCD(LCD_CLR);
							stringToLCD("End Device 2:");
							
						}
							commandToLCD(LCD_LN2);		// print if pin9 was ON or OFF depending on IO flag
							stringToLCD("pin9 is ");
							if(IO){
							
							stringToLCD("ON");
							}else stringToLCD("OFF");
						
					
					commandToLCD(LCD_LN1);		// reset cursor and buffer full flag
						bfull= false;
							
				}
			if(frametype == 0x90){
				for(j=15;j<usebsize;j++){
					rxdRF[j-15] = usebuffer[j];
					rxdsize++;
				}
				if(device1){						// if device 1 matches, print to LCD
						commandToLCD(LCD_CLR);
						stringToLCD("End Device 1:");
					}
					if(device2){						// if device 2 matches, print to LCD
						commandToLCD(LCD_CLR);
						stringToLCD("End Device 2:");
						
					}
				commandToLCD(LCD_LN2);		// print if pin9 was ON or OFF depending on IO flag
				for(t=0; t<	rxdsize; t++){
					dataToLCD(rxdRF[t]);
				}				
				
				commandToLCD(LCD_LN1);		// reset cursor and buffer full flag
				bfull= false;

			}
		
		}	
		use_switches(read_switches());	// read swithces and set LED's
		delay(10000000);

	}		
}

