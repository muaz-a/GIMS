#include <stdint.h>

# define SPI2_SS_HI 0x00001000;
# define SPI2_SS_LOW 0x10000000;

void spi_init(void);

uint16_t use_spi(uint16_t data);