/* adc_init */

#include <stdint.h>

// initialize ADC
void adc_init(void);

// read from ADC
uint16_t read_adc(uint16_t port);

// convert ADC data to ascii code
void hex2asxii(uint16_t hexval, uint16_t port);

void hex2PMW(uint16_t hexval);

