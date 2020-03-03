#include "main.h"

uint8_t pressedBtn;
static state cur_state = WAIT_MAIN_1;

int main() {	
	
	clockInit();
	portInit();
	LCDInit();
	ADCInit();
	timerInit();
	ToLCD(LCD_LN2, 0);
	ToLCD(LCD_LN1, 0);
	stringToLCD("Welcome!");
	delay(4000000); // ~2.5 sec (not)
	ToLCD(LCD_CLR, 0);
	ToLCD(LCD_LN1, 0);
	while(1) 
	{
		switch(cur_state)
		{
			case WAIT_MAIN_1:
				while(cur_state == WAIT_MAIN_1)
				{
					// can do some rudimentary checking or signalling here 
				}
				break;
				
			case SEND_RESPONSE_2:
				
		if(detect_btn_press() != pressedBtn)
		{
			pressedBtn = detect_btn_press();
			
			if(pressedBtn == 8) // Green PB was pressed
			{
				calcSignal();
			}
		}
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
 
void calcSignal(void) {
	char vBuf[8],  				// char buffer to hold voltage value to print on LCD
			 freqBuf[8];			// char buffer to hold frequency value to print on LCD
	
	unsigned long i,			// iterator var
								peakCtr,// peak crossing counter
								cycCtr; // cycle counter - to measure period of wave
								
	
	unsigned read;   			// raw ADC value as read in
	
	double min, 					// minimum ADC reading
				 max, 					// maximum ADC reading
				 vCutoffRise, 	// 0.7 x max ADC reading
				 vCutoffFall, 	// 0.5 x max ADC reading
				 deltaV, 				// stores max-min voltage
				 frequency,     // calculated frequency
				 dacAmplitude;  // amplitude to send to DAC - percentage of 3.1V
	
	min = 2000;
	max = 0;
	peakCtr = 0;
	cycCtr = 0;
	
	for (i = 0; i < 200000; i++)
	{
		read = readADC(); // this can take around 28 ticks

		if (read > max)
			max = read;
		
		if (read < min)
			min = read;
	}
	vCutoffRise = 0.7 * max;
	vCutoffFall = 0.5 * max;
	
	min = 3.3*(((double)min)/4095.0);
	max = 3.3*(((double)max)/4095.0);
	deltaV = max - min;
	
	ToLCD(LCD_CLR, 0);
	ToLCD(LCD_LN1, 0);
	stringToLCD("delta V = ");	
	sprintf(vBuf, "%.2f", deltaV);
	stringToLCD(vBuf);
	ToLCD('V', 1);
			
	#ifdef DEBUG
	GPIOB->BSRR = 0x400; // set PB10 high. Done to measure run time of loop below
	#endif
	
	for (i = 0; i < SAMPLED_CYCLES; i++)
	{
		do { 
			read = readADC(); 
			cycCtr++;
		} while (read < vCutoffRise);
		
		if (read > vCutoffRise)
			peakCtr++;
		
		do {
			read = readADC();
			cycCtr++;
		} while (read > vCutoffFall);		
	}
	
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
	
	frequency = (double)peakCtr * ( 205194.0 / (double)cycCtr ); // num = 189995
	
	if (frequency > 5000 || frequency < 10) // hard limits on frequency
		frequency = 0; // can also do error reporting later
		
	ToLCD(LCD_LN2, 0);
	stringToLCD("Freq = ");	
	sprintf(freqBuf, "%.2f", frequency);
	stringToLCD(freqBuf);
	stringToLCD(" Hz");
	
	dacAmplitude = ( (int)((deltaV * 10) / 10) / 2.6 );	// divide by 3.1 earlier
	dacInit(dacAmplitude, (uint16_t)frequency);
	
	pressedBtn = 0;
}
