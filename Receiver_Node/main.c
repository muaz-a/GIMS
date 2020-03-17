#include "main.h"

uint8_t pressedBtn;
static state cur_state = WAIT_CO_1;
state next_state;
//message MSG;

int main() {  
  
  clockInit();
  portInit();
  LCDInit();
  ADCInit();
  timerInit();
  usartInit();
    
  char dispBuf[5];
  int rc;
  
  // Comms setup
  uint8_t coAddr[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  NODE Device[2];
  RXD Received;
  XbeeSetUp(Device); // Set up Device Struct with Xbee adr, idx and status
  
  ToLCD(LCD_LN2, 0);
  ToLCD(LCD_LN1, 0);
  stringToLCD("Welcome to GIMS!");
  delay(18000000); // ~18e6 = 3 sec
  ToLCD(LCD_CLR, 0);
  ToLCD(LCD_LN1, 0);
  
  while(1) 
  {
    switch(cur_state)
    {
      case WAIT_CO_1:
        /* Come here on startup. Wait indefinitely until we receive signal
        from Main Node (AKA Coordinator/CO). If '1' received, we proceed to
        next state. If anything else received, we go into error state */
      
        next_state = WAIT_CO_1;
      
        while(next_state == WAIT_CO_1)
        {
          ToLCD(LCD_CLR, 0);
          stringToLCD("Waiting for CO");          
          // will have to wait indefinitely here until receive signal from CO
          XbeeRecieve(&Received, 0);
          
          #ifdef DEBUG
          // To test message receiving and sending          
          ToLCD(LCD_CLR, 0);
          stringToLCD("Rcvd size: ");
          sprintf(dispBuf, "%d", Received.length);
          stringToLCD(dispBuf);
          delay(12000000);
          ToLCD(LCD_LN2, 0);
          
					stringToLCD("Data: ");
					ToLCD(Received.data[0], 1);
					//xbeeSend(coAddr, sizeof(Received.data[0]), &Received.data[0]); 
                                   
          #endif
          
          if(Received.length != 1 || (char)Received.data[0] != SYNCH)
          {
            next_state = ERROR_STATE;
            
            #ifdef DEBUG
            ToLCD(LCD_CLR, 0);
            stringToLCD("ERROR STATE");
            delay(4000000);
            #endif
          }
          
          if((char)Received.data[0] == SYNCH)
          {
            #ifdef DEBUG            
            ToLCD(LCD_CLR, 0);
            stringToLCD("RCVD from CO: ");  
            ToLCD(SYNCH, 1);
            delay(4000000);
            #endif
            
            next_state = SEND_RESPONSE_2;
          }                 
        }
        break;
        
      case SEND_RESPONSE_2:
      // Send back to CO our SYNCH signal ('1')
        xbeeSend(coAddr, 1, "1"); 
        
        #ifdef DEBUG
        ToLCD(LCD_CLR, 0);
        stringToLCD("SENT 1 to CO");  
        delay(4000000);
        #endif
      
        next_state = WAIT_CO_3;
        break;
      
      case WAIT_CO_3:
        // Wait on CO to signal us to sleep
        next_state = WAIT_CO_3;
        
        #ifdef DEBUG
        ToLCD(LCD_CLR, 0);
        stringToLCD("Waitin on CO sig");
        delay(4000000);
        #endif
      
        while(next_state == WAIT_CO_3)
        {
          // wait indefinitely here
          XbeeRecieve(&Received, 0);
          
          if(Received.length != 1 || (char)Received.data[0] != SLEEP)
          {
            next_state = ERROR_STATE;
            
            #ifdef DEBUG
            ToLCD(LCD_CLR, 0);
            stringToLCD("ERROR STATE");
						
						ToLCD(LCD_LN2, 0);
						stringToLCD("Rcvd size: ");
						sprintf(dispBuf, "%d", Received.length);
						stringToLCD(dispBuf);
						delay(2400000);											
            #endif
          }
          
          if((char)Received.data[0] == SLEEP)
          {
            #ifdef DEBUG            
            ToLCD(LCD_CLR, 0);
            stringToLCD("RCVD from CO: ");  
            ToLCD(SLEEP, 1);
            delay(6000000);
            #endif
            
            next_state = SLEEP_4;
          }
        }        
        break;
        
      case SLEEP_4:
        // send back SLEEP signal, and go to sleep for 5 min
        //xbeeSend(coAddr, sizeof(SLEEP), SLEEP); 
        
				xbeeSend(coAddr, 1, "2"); 
        
				#ifdef DEBUG
        ToLCD(LCD_CLR, 0);
        stringToLCD("Sleep for 5 min");        
        #endif
      
        //sleep(15); // 5 minutes
        delay(90000000); // 15 seconds - for debug
			
        #ifdef DEBUG
        ToLCD(LCD_CLR, 0);
        stringToLCD("Awake");        
        delay(12000000);
        #endif
      
        next_state = PROCESS_SIG_5;
        break;
        
      
      case PROCESS_SIG_5:
        
				next_state = PROCESS_SIG_5;
			
        rc = 1;
        //char amp[8], freq[8];
				double amp, freq;
			
        // send RESP state
        xbeeSend(coAddr, 1, "3"); // RESP
        // receive RESP ack within 10 sec
        rc = XbeeRecieve(&Received, 60000000); // was 60e6 = 10 sec earlier
        
        if (rc == -1)
        {
          // did not receive response in time. Goto error state
          next_state = ERROR_STATE;
					
					#ifdef DEBUG
					ToLCD(LCD_CLR, 0);
					stringToLCD("ERROR STATE:");
					ToLCD(LCD_LN2, 0);	
					stringToLCD("Didn't rcv 3");
					delay(12000000);
					#endif
					
        } else if (rc == 0)
        {
          if(Received.length != 1 || (char)Received.data[0] != RESP)
          {
            next_state = ERROR_STATE;
          } else if ((char)Received.data[0] == RESP)
          {
						stringToLCD("Analyzing");
            getSignal(&amp, &freq);
            
            xbeeSend(coAddr, sizeof(amp), (char)amp); 
            xbeeSend(coAddr, sizeof(freq), (char)freq); 
            
						#ifdef DEBUG
						ToLCD(LCD_CLR, 0);
						stringToLCD("Sent to CO:");
						ToLCD(LCD_LN2, 0);	
						stringToLCD("Amp and Freq");
						delay(24000000);
						#endif
						
            next_state = WAIT_CO_3;
          }
        }
        
        break;
				
			case ERROR_STATE:
				
				next_state = ERROR_STATE;
				
				#ifdef DEBUG
				ToLCD(LCD_CLR, 0);
				stringToLCD("ERROR STATE");
				ToLCD(LCD_LN2, 0);	
				stringToLCD("Here for good");
				delay(24000000);
				#endif
				
				break;
			
//      if(detect_btn_press() != pressedBtn)
//      {
//        pressedBtn = detect_btn_press();
//      
//        if(pressedBtn == 8) // Green PB was pressed
//        {
//          calcSignal();
//        }
//      }
    }
    cur_state = next_state;
  }
  
}

uint8_t detect_btn_press(void) {
  uint32_t sw_val;
  uint8_t trunc;
  
  // Read in values from pin registers
  sw_val = ((GPIOB->IDR &(GPIO_IDR_IDR8 | GPIO_IDR_IDR9)) >> 8)
           | ((GPIOC->IDR & GPIO_IDR_IDR12) >> 10)
           | ((GPIOA->IDR & GPIO_IDR_IDR5) >> 2) ;
  
  // Mask and truncate results
  sw_val = ~sw_val & 0xF;
  trunc = sw_val;
  return trunc;
}

void getSignal(double *amp, double *freq)
{
  double amp_arr[3], freq_arr[3];
  int median_amp, median_freq;
	char vBuf[8], freqBuf[8];
  
  for (int i = 0; i < 3; i++) 
  {
    calcSignal(&amp_arr[i], &freq_arr[i]);
  }
  
  median_amp = getMedian(amp_arr);
  median_freq = getMedian(freq_arr);
  
	//sprintf(amp, "%.3f", amp_arr[median_amp]);
	//sprintf(freq, "%.3f", freq_arr[median_freq]);
  *amp = amp_arr[median_amp];
  *freq = freq_arr[median_freq];
}
 
void calcSignal(double *amplitude, double *frequency) {
  
  #ifdef DEBUG
  char vBuf[8],         // char buffer to hold voltage value to print on LCD
       freqBuf[8];      // char buffer to hold frequency value to print on LCD
  
  // double dacAmplitude;  // amplitude to send to DAC - percentage of 3.1V
  #endif
  
  unsigned long peakCtr, // peak crossing counter
                cycCtr;  // cycle counter - to measure period of wave
                
  
  unsigned read;         // raw ADC value as read in
  
  double min,           // minimum ADC reading
         max,           // maximum ADC reading
         vCutoffRise,   // 0.7 x max ADC reading
         vCutoffFall;   // 0.5 x max ADC reading      
    
  min = 2000;
  max = 0;
  peakCtr = 0;
  cycCtr = 0;
  
  for (int i = 0; i < 200000; i++)
  {
    read = readADC(); // this can take around 28 ticks

    if (read > max)
      max = read;
    
    if (read < min)
      min = read;
  }
  vCutoffRise = 0.7 * max; // peak detection for frequency
  vCutoffFall = 0.5 * max;
  
  min = 3.3*(((double)min)/4095.0); // convert to voltage level
  max = 3.3*(((double)max)/4095.0);
  *amplitude = max - min;
  
  #ifdef DEBUG
  ToLCD(LCD_CLR, 0);
  ToLCD(LCD_LN1, 0);
  stringToLCD("delta V = ");  
  sprintf(vBuf, "%.2f", *amplitude);
  stringToLCD(vBuf);
  ToLCD('V', 1);
      
  GPIOB->BSRR = 0x400; // set PB10 high. Done to measure run time of loop below
  #endif
  
  alarm = 0;
  Configure_RTC(2);
  
  while(!alarm)
  {
    for (int i = 0; i < SAMPLED_CYCLES; i++)
    {
      do { 
        read = readADC(); 
        cycCtr++;
        
        //if(alarm)
          //break;
        
      } while (read < vCutoffRise);
      
      if (read > vCutoffRise)
        peakCtr++;
      
      do {
        read = readADC();
        cycCtr++;
        
        //if(alarm)
          //break;
        
      } while (read > vCutoffFall);    
    }
  }
  
  Disable_RTC();
  
  
  
  #ifdef DEBUG
  GPIOB->BSRR = 0x4000000; // set PB10 low. 
  #endif
  
  if (peakCtr != SAMPLED_CYCLES)
  {
    // MHA: An error has occurred here
    // It may be that the amplitude of the wave may be much lower than the peak
    // and thus hasn't been counted. Frequency measurement will be inaccurate
    // May need to log and report this error.
  }
  
  *frequency = (double)peakCtr * ( 205194.0 / (double)cycCtr ); // num = 189995
  
  if (*frequency > 5000 || *frequency < 10) // hard limits on frequency
    *frequency = 0; // can also do error reporting later
  
	if(alarm)
    *frequency = 0;
	
  #ifdef DEBUG
  ToLCD(LCD_LN2, 0);
  stringToLCD("Freq = ");  
  sprintf(freqBuf, "%.2f", *frequency);
  stringToLCD(freqBuf);
  stringToLCD(" Hz");
  delay(18000000); // 3 sec
  #endif
  
  /*dacAmplitude = ( (int)((amplitude * 10) / 10) / 2.6 );  // divide by 3.1 earlier
  dacInit(dacAmplitude, (uint16_t)frequency);
  pressedBtn = 0;*/
}

int getMedian(double AL3[]) 
{  
  if (AL3[0] > AL3[1])  
    { 
        if (AL3[1] > AL3[2]) 
            return 1; 
        else if (AL3[0] > AL3[2]) 
            return 2; 
        else
            return 0; 
    } 
    else 
    { 
        if (AL3[0] > AL3[2]) 
            return 0; 
        else if (AL3[1] > AL3[2]) 
            return 2; 
        else
            return 1; 
    } 
}

void RTCAlarm_IRQHandler(void)
{
    if((EXTI->PR & EXTI_PR_PR17) == EXTI_PR_PR17)
    {
        EXTI->PR |= EXTI_PR_PR17;
        if ((RTC->CRL & RTC_CRL_ALRF)==RTC_CRL_ALRF)
        {
            RTC->CRL &= ~RTC_CRL_ALRF;
            alarm = 1;
        }
    }
    else
    {
        NVIC_DisableIRQ(RTCAlarm_IRQn);// Disable
    }
}
