/* Software version of SPI for Intel Edison using MRAA library
   Copyright (c) 2015 Andrey Chilikin (https://github.com/achilikin)

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
 
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
  
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef EDISON_SW_SPI_H
#define EDISON_SW_SPI_H

#include <stdint.h>
#include "mraa.h"

#ifdef __cplusplus
extern "C" {
#if 0 // dummy bracket for VAssistX
}
#endif
#endif

struct mraa_sw_spi {
	mraa_gpio_context sdi;
	mraa_gpio_context sdo;
	mraa_gpio_context sck;
};

typedef struct mraa_sw_spi *mraa_spi_sw_context;

mraa_spi_sw_context sw_spi_init(uint8_t pin_sck, uint8_t pin_mosi, uint8_t pin_miso);
void sw_spi_close(mraa_spi_sw_context spi);

void sw_spi_write_word(mraa_spi_sw_context spi, uint16_t data);
uint16_t sw_spi_send_word(mraa_spi_sw_context spi, uint16_t data);

void sw_spi_write_data(mraa_spi_sw_context spi, mraa_gpio_context ss, uint8_t *data, unsigned len);
uint8_t *sw_spi_send_data(mraa_spi_sw_context spi, mraa_gpio_context ss, uint8_t *data, unsigned len);

#ifdef __cplusplus
}
#endif

#endif
