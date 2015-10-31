EdRadio
=======

EdRadio is an example of using RFM12BS RF module with Intel Edison mini breakout board (or [SparkFun RPi Block](https://www.sparkfun.com/products/13044)).

To communicate with RFM12BS software version if SPI is implemented using [mraa library](https://github.com/intel-iot-devkit/mraa) memory mapped io. Kernel version of SPI is not supported at the moment because of very slow user/kernel space data transfer.    

shDAN LBS
---------

This example implements Listening Base Station (**LBS**) for Data Acquisition Network ([shDAN](https://github.com/achilikin/shDAN)) running in interactive command mode. 

**Following commands are available:**

* _help_ 
* _cls_  clear screen
* _exit|quit|q_
* _echo_ show current echo configuration
* _echo rx on|off_ turn data rx on or off
* _echo dan on|off_ turn DAN protocol parsing on or off* 
* _sync_ send sync request to active base station

On startup DAN message parsing is on.

Kernel version SPI vs user space memory mapped SW SPI
-----------------------------------------------------

Some screen shots showing kernel SPI behavior transferring 2 bytes (16bits).
Yellow - CS signal, blue - SCK signal.

![mmr70 screen](http://achilikin.com/github/sys-4mhz.png)

SPI speed 4MHz, HW CS deselects after every byte transferred, ~10usec inter-byte gap
   
![mmr70 screen](http://achilikin.com/github/sys-2mhz.png)

SPI speed 2MHz, HW CS deselects after every byte transferred, ~8.5usec inter-byte gap. This is the speed used for RFM12BS.

![mmr70 screen](http://achilikin.com/github/sys-pi-cs.png)

SPI speed 2MHz, memory mapped gpio pin is used for CS. 2 bytes transfer takes in average 340usec - after driving CS low it takes about 220 usec before kernel SPI driver starts transfer, and then it takes ~100usec to return to user space.

![mmr70 screen](http://achilikin.com/github/sw-spi.png)

Software version of SPI using [mraa library](https://github.com/intel-iot-devkit/mraa) with memory mapped io. It takes ~27usec to transfer 2 bytes (16 bits).


