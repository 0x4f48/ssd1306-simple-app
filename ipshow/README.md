# IPSHOW README

This is a simple application detects link up/down and displays current IPv4 addrss on a SSD1306 I2C LCD.

![Running ipshow](https://github.com/0x4f48/ssd1306-simple-app/blob/master/misc/ipshow-run.gif)


## BUILD

`make`

## RUN

`ipshow [i2c device node] [interfaces to monitor]` 

`ex) ./ipshow /dev/i2c-0 eth0 eth1`

