#include "main.h"

typedef enum 
{
    SYNCH = '1',
    REQ = '2',
    RESP  = '3',
    OTHER = '4'
} message;

uint8_t indexED(NODE *);

int main(void)
{
    uint8_t num = 0;
    NODE *Devices;
    uint8_t usebuffer[PACKET_LENGTH];
    uint8_t usebsize = PACKET_LENGTH;
    char synchmessage[] = "1";
    int synchcount = 0, i =0;
    uint8_t BROADCAST[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xF};
    uint8_t ED1[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93};		// Address of End Device 1
    uint8_t ED2[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x15, 0x4B};		// Address of End Device 2
    

    static state cur_state = POWER_UP_1;
    static state next_state;
    
    
    clockInit();
    init_usart();
    initLCD();

    while(1)
    {
        switch(cur_state)
        {
            case POWER_UP_1:    // idle startup time, identifiale by LCD message
                stringToLCD("Power Up");
                for(i=0;i<12000000;i++);
                next_state = SEND_SYNCH_2;
                break;
                
            case SEND_SYNCH_2:  //
                commandToLCD(LCD_CLR);
                stringToLCD("Send Synch");
                xbeeSend(ED1,1,"1");			// send "1" as synch message
                next_state = INDEX_3;
                break;
            
            case INDEX_3:
                num = indexED(Devices);
                commandToLCD(LCD_CLR);
                stringToLCD("End Device 1:");
                for(int t=0; t<2; t++)
                {
                    dataToLCD(Devices[0].address[7+t]);
                }
                next_state = STAGGER_RESP_4;
                break;
            case STAGGER_RESP_4:
                while(1);
//                        commandToLCD(LCD_CLR);
//                        stringToLCD("Stagger Start");
//                        for(int j = 0; j < NUM_DEVICES; j++)
//                        {
//                                xbeeSend(Device[j].address,1,"2");
//                                while(!bfull)			// wait unit buffer is full
//                                {
//                                    
//                                }
//                                usebsize = bsize;		// copy data to avoid buffer being changed by interrupt
//                                for(int m=0; m<usebsize;m++)
//                                {
//                                    usebuffer[m] = buffer[m];
//                                    buffer[m] = 0x00;
//                                }	
//                                bfull = false;					// reset buffer full flag
//                                XbeeRecieve(usebuffer, usebsize, &Recieved);
//                                if(Recieved.data[0] == '2')
//                                {
//                                    Device[Recieved.device-1].status = RDY;			// recieved synch, device is ready
//                                }
//                                else 
//                                {
//                                    Device[Recieved.device-1].status = OFFLINE;		// did't recieve synch, device not ready
//                                    synchcount = 0;
//                                    next_state = POWER_UP_1;					// restart from  POWER_UP
//                                    xbeeSend(Device[synchcount].address,1,"4");		// send error back to Reciever
//                                    break;
//                                }
//                                delay(168000000); 			// 7 second intervals
//                        }
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

uint8_t indexED(NODE *Device)
{
    RXD Recieved;   // To hold recieved data
    uint8_t num = 0,i;
    
    commandToLCD(LCD_CLR);
        alarm = 0;
        Configure_RTC(8);
        stringToLCD("Waiting For Responses");
        while (!alarm) {
            XbeeRecieve(&Recieved);
            if(Recieved.data[0] == SYNCH)
            {
                Device = (NODE*) realloc(Device, sizeof(NODE)*(num+1));
                Device[num].status = RDY;
                for(i=0;i<ADDRESS_LENGTH;i++)
                {
                    Device[num].address[i] = Recieved.address[i];
                }
            }
            num++;
        }
        alarm = 0;
        Disable_RTC();

}