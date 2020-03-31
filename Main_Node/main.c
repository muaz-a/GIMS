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
//    uint8_t ED1[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93};        // Address of End Device 1
//    uint8_t ED2[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x15, 0x4B};        // Address of End Device 2


    static state cur_state = POWER_UP_1;
    static state next_state;


    clockInit();
    init_usart();
    initLCD();

    sine(3);
    while(1);
    sineOff();

    while(1)
    {
        switch(cur_state)
        {
            case POWER_UP_1:    // idle startup time, identifiale by LCD message
                stringToLCD("Power Up");
                for(int i=0; i<12000000; i++);
                next_state = SEND_SYNCH_2;
                delay(6000000);
                break;

            case SEND_SYNCH_2:  //
                commandToLCD(LCD_CLR);
                stringToLCD("Send Synch");
                xbeeSend(BROADCAST,1,"1");  // send "1" as synch message
                next_state = INDEX_3;
                break;

            case INDEX_3:
                num = indexED(Devices); // indexes ED and returns number of ED
                commandToLCD(LCD_CLR);
                stringToLCD("End Device :");
                dataToLCD(num+0x30);    // print number of ED's
                if(num==0)
                {
                    commandToLCD(LCD_CLR);
                    stringToLCD("No End Devices");
                    commandToLCD(LCD_LN2);
                    stringToLCD("Check Node Connections");
                    next_state = POWER_UP_1;
                }
                else    next_state = STAGGER_RESP_4;

                break;
            case STAGGER_RESP_4:
                stagger(num, Devices);  // stagger ED for sleep
                next_state = LISTEN_5;
                break;

            case LISTEN_5:
                listen(num, Devices);   // wait for ED's to wake to send Sine Signal
                next_state = ANALYZE_6;
                break;
            case ANALYZE_6:
                analyze(num, Devices);  // analyze the data recieved from ED's
                next_state = LISTEN_5;
                break;
            case ERROR_STATE:
				commandToLCD(LCD_CLR);
                stringToLCD("Please reset device");
                next_state = ERROR_STATE;
				while(1);
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
    stringToLCD("Waiting for");
    commandToLCD(LCD_LN2);
    stringToLCD("request");
    while(!alarm) {
        XbeeReceive(&Received);
        if(Received.data[0] == SYNCH)
        {
            //Device = (NODE*) realloc(Device, sizeof(NODE)*(num+1));
            Device[num].status = RDY;
            Device[num].index = num+1;
            for(i=0; i<ADDRESS_LENGTH; i++)
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
        XbeeReceive(&Received); // check for response

        Disable_RTC();
        if(alarm || Received.data[0]!=SLEEP)    // Checking if timeout or received wrong data
        {
            alarm=0;
            Devices[j].status=TIMEOUT;
            xbeeSend(Devices[j].address,1,"4"); // send error to ED
            stringToLCD("Times Out");
        }
        commandToLCD(LCD_LN2);
        stringToLCD("2 Received");
        delay(6000000);

        if((j+1)<num)   // delay between ED for all but last device
        {
            alarm=0;
            Configure_RTC(STAGGER);
            while(!alarm);
            Disable_RTC();
            alarm=0;
        }

    }
    delay(1200000);
}

void listen(uint8_t num, NODE * Devices)
{
    RXD Received;
    bool Order = true, Same = true, ED = true;
    int numRXD = 0;
    alarm = 0;
    commandToLCD(LCD_CLR);
    //Configure_RTC(15-(STAGGER*(num-1)));  // ~5 minutes
    Configure_RTC(20);  // ~5 minutes
    stringToLCD("Waiting for");
    commandToLCD(LCD_LN2);
    stringToLCD("3");
    delay(6000000);
    for(int j = 0; j < num; j++)
    {
        XbeeReceive(&Received); // wait for ED to send Resp ("3")
        if(Received.data[0] == RESP)    // make sure "3" was recieved
        {
            commandToLCD(LCD_CLR);
            stringToLCD("3 Recieved");
            xbeeSend(Received.address,1,"3");
            delay(120000);
        }
//commandToLCD(LCD_CLR);
        if(alarm)   // check if alarm has gone
        {
            Disable_RTC();
            alarm=0;
            Devices[j].status=TIMEOUT;  // if timed out set device status
            commandToLCD(LCD_CLR);
            dataToLCD(j);
            stringToLCD(" Timed Out");
            delay(1200000);
        }
        else
        {
            Disable_RTC();
            for(int i=0; i < ADDRESS_LENGTH; i++)
            {
                while(Devices[j].address[i] != Received.address[i]) // find what device recieved from
                {
                    Devices[j].status=TIMEOUT;  // if not in order set as timed out
                    commandToLCD(LCD_CLR);
                    dataToLCD(j);
                    stringToLCD(" Timed Outs");
                    Order = false;  // set Order flag to false
                }
            }
            if(Order)   // check if Recieved ED is in the right order
            {
                Devices[j].status=RDY;
/**
                commandToLCD(LCD_CLR);
                stringToLCD("Send: ");
                dataToLCD((j+1)+0x30);
**/
                sine(1);    // send sine signal
                Configure_RTC(SINTIM);
                while(numRXD < 3)   // loop three times, once for each data point: Amp, Freq, TiltSW
                {
                    XbeeReceive(&Received); // wait for response
                    for(int p=0; p < ADDRESS_LENGTH; p++)
                    {
                        if(Devices[j].address[p] != Received.address[p])    // make sure recieved data was from the right ED
                        {
                            Same = false;   // if not set flag
                        }
                    }
                    if(Same)    // if addresses the same continue to get data
                    {
                        for(int i=0; i <Received.length; i++)
                        {
                            if(numRXD==0)
                                Devices[j].ampl[i] = Received.data[i];  // gather freq and amplitude data
                            else if(numRXD==1)
                                Devices[j].freq[i] = Received.data[i];
                            else
                                Devices[j].TiltSW = Received.data[i];   //  tilt switch data 0-All Good, 1-No Good
                        }   // 06 00 20 DD 01 00 08 E5
                        numRXD++;
                    }
                    else    // if addresses not the same, send "4" to ED and mark ED as Timeout
                    {
                        xbeeSend(Received.address,1,"4");   // send for to ED for it to go into Error State
                        for(int i=0; i < num; i++)
                        {
                            ED = true;
                            for(int k=0; k < ADDRESS_LENGTH; k++)
                            {
                                if(Devices[i].address[k] != Received.address[k])    // find what device sent unexpected message
                                {
                                    ED = false;
                                }
                                if(!ED)
                                {
                                    Devices[i].status = TIMEOUT;    // mark it as timeout
                                    break;
                                }
                            }
                            if(ED)
                                break;

                        }
                    }
                    Same = true;
                }
/**
                XbeeReceive(&Received); // wait for response
                for(int i=0; i <Received.length; i++)
                {
                    Devices[j].ampl[i] = Received.data[i];  // gather freq and amplitude data
                }   // 06 00 20 DD 01 00 08 E5
                XbeeReceive(&Received); // wait for response
                for(int i=0; i <Received.length; i++)
                {
                    Devices[j].freq[i] = Received.data[i];
                }   // B0 06 00 20 DD 01 00 08
                XbeeReceive(&Received); // wait for response
                for(int i=0; i <Received.length; i++)
                {
                    Devices[j].TiltSW = Received.data[i];
                }
                while(!alarm); // loop if timer hasn't finished
**/
                Disable_RTC();
                alarm=0;
                sineOff();
            }
            Order = true;
        }
        if((j+1)<num)
        {
            alarm=0;
            Configure_RTC(STAGGER);
            commandToLCD(LCD_CLR);
            stringToLCD("Wait For Next");
            delay(1200000);
        }
    }
}

void analyze(uint8_t num, NODE * Devices)
{
    int NoResp = 0;
    double Amp, Freq;
    static int Tilt[10];

    commandToLCD(LCD_CLR);
    stringToLCD("Analyze Data");
    delay(1200000);
    for(int j = 0; j < num; j++)
    {
        commandToLCD(LCD_CLR);
        if(Devices[j].status==TIMEOUT)
        {
            NoResp++;
            if(NoResp==0)   // list any devices that are timed out
            {
                stringToLCD("Device Timeout: ");
            }
            dataToLCD(j+1+0x30);
            stringToLCD(", ");
        }
        else
        {
            for(int i=0; i <LENGTH; i++)
            {
                if(Devices[j].ampl[i] > 0x2C)
                    dataToLCD(Devices[j].ampl[i]);
            }

            commandToLCD(LCD_LN2);
            for(int i=0; i <LENGTH; i++)
            {
                if(Devices[j].freq[i] > 0x2C)
                    dataToLCD(Devices[j].freq[i]);
            }

            Devices[j].Dfreq = 0;
            Devices[j].Dampl = 0;
            Freq = atof(Devices[j].freq);
            Amp = atof(Devices[j].ampl);
            Devices[j].Dampl = Amp;
            Devices[j].Dfreq = Freq;
            if(Devices[j].TiltSW == '1')
            {
                Tilt[j] = 1;
                commandToLCD(LCD_CLR);
                stringToLCD("Tilt Switch Trig");
                commandToLCD(LCD_LN2);
                stringToLCD("Device ");
                dataToLCD((j+1)+0x30);
            }
            else
                Tilt[j] = 0;
            delay(6000000);
        }
    }
}
