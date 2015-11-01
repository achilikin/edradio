
Edison Kernel version SPI vs user space memory mapped SW SPI
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

Software version of SPI using [mraa library][] with memory mapped io. It takes ~27usec to transfer 2 bytes (16 bits).

[mraa library]:https://github.com/intel-iot-devkit/mraa