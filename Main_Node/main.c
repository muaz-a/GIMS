#include "main.h"

int main(void)
{
    
    uint8_t usebuffer[PACKET_LENGTH];
    uint8_t usebsize = PACKET_LENGTH;
    struct NODE Device[NUM_DEVICES];		// setup Device Struct with Addresses and index's
    char synchmessage[] = "1";
    int synchcount = 0;
    struct RXD Recieved;		// To hold recieved data
    Recieved.device = 0;
    Recieved.length = 0;
    static state cur_state = POWER_UP_1;
    static state next_state;
    
    
    clockInit();
    init_usart();
    initLCD();

    while(1)
    {
        switch(cur_state)
        {
            case POWER_UP_1:						// configure NODE struct for 2 devices
                        stringToLCD("Power Up");
                        XbeeSetUp(Device);
                        next_state = SEND_SYNCH_2;
                        break;
                
            case SEND_SYNCH_2:
                        commandToLCD(LCD_CLR);
                        stringToLCD("Send Synch");
                        xbeeSend(Device[synchcount].address,1,"1");			// send "1" as synch message
                        next_state = WAIT_RXD_3;
                            break;
            
            case WAIT_RXD_3:
                        commandToLCD(LCD_CLR);
                        stringToLCD("Wait For Response");
                        while(!bfull)			// wait unit buffer is full
                        {
                            
                        }
                        usebsize = bsize;		// copy data to avoid buffer being changed by interrupt
                        for(int m=0; m<usebsize;m++)
                        {
                            usebuffer[m] = buffer[m];
                            buffer[m] = 0x00;
                        }	
                        bfull = false;					// reset buffer full flag
                        bcount = 0;
                        XbeeRecieve(usebuffer, usebsize, &Recieved);
                        if(Recieved.data[0] == synchmessage[0])
                        {
                            Device[Recieved.device-1].status = RDY;			// recieved synch, device is ready
                        }
                        else 
                        {
                            Device[Recieved.device-1].status = OFFLINE;		// did't recieve synch, device not ready
                            synchcount = 0;
                            next_state = POWER_UP_1;					// restart from  POWER_UP
                            xbeeSend(Device[synchcount].address,1,"4");		// send error back to Reciever
                            break;
                        }
                        if(synchcount < NUM_DEVICES-1)		// have we sent a synch to each device
                        {
                            next_state = SEND_SYNCH_2;		// if not go back to SEND_SYNCH
                            synchcount++;
                            break;
                        }
                        else 
                        {
                            next_state = STAGGER_RESP_4;		// all devices synched
                            synchcount = 0;
                            break;
                        }
                
            case STAGGER_RESP_4:
                        commandToLCD(LCD_CLR);
                        stringToLCD("Stagger Start");
                        for(int j = 0; j < NUM_DEVICES; j++)
                        {
                                xbeeSend(Device[j].address,1,"2");
                                while(!bfull)			// wait unit buffer is full
                                {
                                    
                                }
                                usebsize = bsize;		// copy data to avoid buffer being changed by interrupt
                                for(int m=0; m<usebsize;m++)
                                {
                                    usebuffer[m] = buffer[m];
                                    buffer[m] = 0x00;
                                }	
                                bfull = false;					// reset buffer full flag
                                XbeeRecieve(usebuffer, usebsize, &Recieved);
                                if(Recieved.data[0] == '2')
                                {
                                    Device[Recieved.device-1].status = RDY;			// recieved synch, device is ready
                                }
                                else 
                                {
                                    Device[Recieved.device-1].status = OFFLINE;		// did't recieve synch, device not ready
                                    synchcount = 0;
                                    next_state = POWER_UP_1;					// restart from  POWER_UP
                                    xbeeSend(Device[synchcount].address,1,"4");		// send error back to Reciever
                                    break;
                                }
                                delay(168000000); 			// 7 second intervals
                        }
                        next_state = STOP_5;
                        break;
                        
            case STOP_5:
                        commandToLCD(LCD_CLR);
                            stringToLCD("Stopped");
                        while(1)
                        {
                            
                        }
                        break;
        }
        cur_state = next_state;
        
    }

    
    
}

