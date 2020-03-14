#include "main.h"



uint8_t indexED(NODE *);
void stagger(uint8_t, NODE *);
void listen(uint8_t, NODE *);
void analyze(uint8_t, NODE *);

int main(void)
{
    uint8_t num = 0;
    NODE *Devices;
    uint8_t BROADCAST[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
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
                for(int i=0;i<12000000;i++);
                next_state = SEND_SYNCH_2;
                break;
                
            case SEND_SYNCH_2:  //
                commandToLCD(LCD_CLR);
                stringToLCD("Send Synch");
                xbeeSend(BROADCAST,1,"1");			// send "1" as synch message
                next_state = INDEX_3;
                break;
            
            case INDEX_3:
                num = indexED(Devices);
                commandToLCD(LCD_CLR);
                stringToLCD("End Device :");
								dataToLCD(num+0x30);
                if (num==0)
								{		
										commandToLCD(LCD_CLR);
                    stringToLCD("No End Devices");
										commandToLCD(LCD_LN2);
										stringToLCD("Check Node Connections");
										next_state = POWER_UP_1;
                }
								else	next_state = STAGGER_RESP_4;
								
                break;
            case STAGGER_RESP_4:
                stagger(num, Devices);
                next_state = LISTEN_5;
                break;
                        
            case LISTEN_5:
                listen(num, Devices);
                next_state = ANALYZE_6;
                break;
             case ANALYZE_6:
                analyze(num, Devices);
                next_state = STAGGER_RESP_4;
                break;
        }
        cur_state = next_state;
        
    }
    
    
}

uint8_t indexED(NODE *Device)
{
    RXD Received;   // To hold received data
    uint8_t num = 0,i;
    
    commandToLCD(LCD_CLR);
        alarm = 0;
        Configure_RTC(8);
        stringToLCD("Waiting For Responses");
        while (!alarm) {
            XbeeReceive(&Received);
            if(Received.data[0] == SYNCH)
            {
                //Device = (NODE*) realloc(Device, sizeof(NODE)*(num+1));
                Device[num].status = RDY;
								Device[num].index = num+1;
                for(i=0;i<ADDRESS_LENGTH;i++)
                {
                    Device[num].address[i] = Received.address[i];
                }
							Received.data[0] = ERROR;
								num++;
            }
            
        }
        alarm = 0;
        Disable_RTC();
    return num;
}

void stagger(uint8_t num, NODE * Devices)
{
	RXD Received;
	
    commandToLCD(LCD_CLR);
    stringToLCD("Stagger Start");
    for(int j = 0; j < num; j++)
    {
        xbeeSend(Devices[j].address,1,"2");
        Devices[j].status=WAITING;
				alarm=0;
        Configure_RTC(RXDTIM);
				XbeeReceive(&Received);
				Disable_RTC();
				if (alarm || Received.data[0]!=SLEEP)
        {
            alarm=0;
            Devices[j].status=TIMEOUT;
						xbeeSend(Devices[j].address,1,"4");
        }
				
				if ((j+1)<num)
				{
						alarm=0;
						Configure_RTC(STAGGER);
						while(!alarm);
						Disable_RTC();
						alarm=0;
				}
				
    }
}

void listen(uint8_t num, NODE * Devices)
{
    RXD Received;
    alarm = 0;
    commandToLCD(LCD_CLR);
		Configure_RTC(302-(STAGGER*(num-1)));		// ~5 minutes
    stringToLCD("Waiting for");
		commandToLCD(LCD_LN2);
		stringToLCD("request");
    for(int j = 0; j < num; j++)
    {
        XbeeReceive(&Received);
				if(Received.data[0] == RESP)
				{
					xbeeSend(Received.address,1,"3");
				}
        commandToLCD(LCD_CLR);
        if (alarm)
        {
            Disable_RTC();
            alarm=0;
            Devices[j].status=TIMEOUT;
        }
        else
        {
            Disable_RTC();
            while(Devices[j].address != Received.address)
            {
               Devices[j].status=TIMEOUT;
                j++;
            }
            Devices[j].status=RDY;
            commandToLCD(LCD_CLR);
            stringToLCD("Send: ");
            dataToLCD((j+1)+0x30);
            Configure_RTC(2);
            sine(1);
            XbeeReceive(&Received);
            Devices[j].freq = Received.data[1];
            Devices[j].ampl = (Received.data[2]<<8)|Received.data[3];
            while(!alarm);
            Disable_RTC();
            alarm=0;
            sineOff();
        }
        if ((j+1)<num)
        {
            alarm=0;
            Configure_RTC(STAGGER);
        }
    }
}

void analyze(uint8_t num, NODE * Devices)
{
    for(int j = 0; j < num; j++)
    {
        
    }
}
