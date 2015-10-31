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
#include "swspi.h"

mraa_spi_sw_context sw_spi_init(uint8_t pin_sck, uint8_t pin_mosi, uint8_t pin_miso)
{
	mraa_spi_sw_context pspi = (mraa_spi_sw_context)malloc(sizeof(struct mraa_sw_spi));

	if (pspi == NULL)
		return NULL;

	pspi->sck = mraa_gpio_init(pin_sck);
	pspi->sdi = mraa_gpio_init(pin_mosi);
	pspi->sdo = mraa_gpio_init(pin_miso);
	mraa_gpio_dir(pspi->sck, MRAA_GPIO_OUT);
	mraa_gpio_dir(pspi->sdi, MRAA_GPIO_OUT);
	mraa_gpio_dir(pspi->sdo, MRAA_GPIO_IN);
	mraa_gpio_use_mmaped(pspi->sck, 1);
	mraa_gpio_use_mmaped(pspi->sdi, 1);
	mraa_gpio_use_mmaped(pspi->sdo, 1);

	return pspi;
}

void sw_spi_close(mraa_spi_sw_context spi)
{
	mraa_gpio_close(spi->sck);
	mraa_gpio_close(spi->sdi);
	mraa_gpio_close(spi->sdo);
	free(spi);
}

void sw_spi_write_word(mraa_spi_sw_context spi, uint16_t data)
{
	for(uint16_t i = 0x8000; i; i >>= 1) {
		mraa_gpio_write(spi->sdi, data & i);
		mraa_gpio_write(spi->sck, 1);
		mraa_gpio_write(spi->sck, 0);
	}
}

uint16_t sw_spi_send_word(mraa_spi_sw_context spi, uint16_t data)
{
	uint16_t rx = 0;

	for(uint16_t i = 0x8000; i; i >>= 1) {
		mraa_gpio_write(spi->sdi, data & i);
		mraa_gpio_write(spi->sck, 1);
		rx = (rx << 1 ) | mraa_gpio_read(spi->sdo);
		mraa_gpio_write(spi->sck, 0);
	}

	return rx;
}

uint8_t *sw_spi_send_data(mraa_spi_sw_context spi, mraa_gpio_context ss, uint8_t *data, unsigned len)
{
	mraa_gpio_write(ss, 0);
	for(unsigned i = 0; i < len; i++) {
		uint8_t tr = data[i];
		for(uint8_t mask = 0x80; mask; mask >>= 1) {
			mraa_gpio_write(spi->sdi, tr & mask);
			tr &= ~mask;
			mraa_gpio_write(spi->sck, 1);
			if (mraa_gpio_read(spi->sdo))
				tr |= mask;
			mraa_gpio_write(spi->sck, 0);
		}
		data[i] = tr;
	}
	mraa_gpio_write(ss, 1);

	return data;
}

void sw_spi_write_data(mraa_spi_sw_context spi, mraa_gpio_context ss, uint8_t *data, unsigned len)
{
	mraa_gpio_write(ss, 0);
	for(unsigned i = 0; i < len; i++) {
		for(uint8_t mask = 0x80; mask; mask >>= 1) {
			mraa_gpio_write(spi->sdi, data[i] & mask);
			mraa_gpio_write(spi->sck, 1);
			mraa_gpio_write(spi->sck, 0);
		}
	}
	mraa_gpio_write(ss, 1);
}
