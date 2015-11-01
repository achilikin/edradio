EdRadio
=======

EdRadio is an example of using RFM12BS RF module with Intel Edison mini breakout board (or [SparkFun RPi Block](https://www.sparkfun.com/products/13044)).

To communicate with RFM12BS software version if SPI is implemented using [mraa library][] memory mapped io. Kernel version of SPI is not supported at the moment because of very slow user/kernel space data transfer. [More details on SPI here][espi].

RFM12BS
-------    

[RFM12B wireless FSK transceiver module](http://www.hoperf.com/rf/fsk_module/RFM12B.htm)
[Si4420/21 RF transmitters](http://www.silabs.com/products/wireless/EZRadio/Pages/Si442021.aspx)
[RFM12BS ARSSI hack](http://blog.strobotics.com.au/2008/06/17/rfm12-tutorial-part2)

Only 7 pins are used + 1 pin for ARSII signal and antenna:

![RFM12BS pins](http://achilikin.com/github/rfm12-pins.png)

Mounted on 8 pin DIP socket:

![RFM12BS pins](http://achilikin.com/github/rfm12-8pin.png)

Code examples
-----------------------
### rdtsc
```cpp
#include "edtsc.h"

int wait_for_low(mraa_gpio_context pin, uint32_t timeout)
{
	uint32_t dt = 0, ts;
	tstamp = rdtsc32(); // get current rdtsc

	while(mraa_gpio_read(pin)) {
		// millis() returns number of milliseconds between tstamp and now
		dt = millis(tstamp); 
		if (dt > timeout)
			return -1;
	}

	return dt;
}
```

### Software SPI
``` cpp
#include "swspi.h"
#include "mraa_pin.h"

	// init mraa and create SW SPI context
	mraa_init();
	mraa_spi_sw_context spi = sw_spi_init(MRAA_SCK, MRAA_MOSI, MRAA_MISO);
	
	// init SS pin
	mraa_gpio_context ss = mraa_gpio_init(MRAA_GP047);
	mraa_gpio_dir(ss, MRAA_GPIO_OUT);
	mraa_gpio_use_mmaped(ss, 1);
	mraa_gpio_write(ss, 1);

	// Select SPI and write 16 bits, MSB first 
	mraa_gpio_write(ss, 0);
	sw_spi_write_word(spi, cmd);
	mraa_gpio_write(ss, 1);
```
### RFM12BS
```cpp
#include "dnode.h"
#include "swspi.h"
#include "rfm12bs.h"
#include "mraa_pin.h"

	// initialize RFM12BS
	rfm12_t rfm;
	memset(&rfm, 0,  sizeof(rfm));
	rfm12_init_spi(&rfm, spi, MRAA_CS1, MRAA_GP047);
	rfm12_init(&rfm, 0xD4, RFM12_BAND_868, 868.0, RFM12_BPS_9600);

	// rfm12_init() sets iddle mode, so swith to receive
	rfm12_set_mode(&rfm, RFM_MODE_RX);

	// rfm12_receive() expects data in shDAN frame format
	if (rfm12_receive_data(&rfm, &node, 4, RFM_RX_DEBUG) == 4) {
		rfm12_set_mode(&rfm, RFM_MODE_RX);
		// process data here
	}

	// send time sync request to the active base station
	dnode_t node;
	memset(&node, 0, sizeof(node));
	node.nid = NODE_TSYNC | NODE_LBS;

	rfm12_send(&rfm, &node, sizeof(node));
	rfm12_set_mode(cfg->rfm, RFM_MODE_RX);
```
[shDAN frame format][shDAN frame]

shDAN LBS
---------

This example implements Listening Base Station (**LBS**) for Data Acquisition Network ([shDAN][]) running in interactive command mode. 

```
root@edisean:/media/sdcard/edradio# ./edradio
MRAA paltform Intel Edison (Miniboard), version v0.8.0-25-g6aaf489
started up in 7 msec
> sync
>  21:35:28 | TSYNC: 21:35:28
 21:35:59 | 11 98 18 14 | NID 1 SID 1 S1 L0 A0 E1 V 3.10 T+24.20
 21:36:19 | 15 96 13 00 | NID 5 SID 1 S1 L0 A0 E1 V 2.90 T+19.00
  
```

**Following commands are available:**

* _help_ 
* _cls_  clear screen
* _exit|quit|q_
* _echo_ show current echo configuration
* _echo rx on|off_ turn data rx on or off
* _echo dan on|off_ turn DAN protocol parsing on or off* 
* _sync_ send sync request to active base station

On startup DAN message parsing is on.

[shDAN]:(https://github.com/achilikin/shDAN)
[mraa library]:https://github.com/intel-iot-devkit/mraa
[shDAN frame]:https://github.com/achilikin/shDAN/blob/master/hDAN_protocol.svg
[espi]:./docs/spi.md