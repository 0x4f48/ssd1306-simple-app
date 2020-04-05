# ssd1306 based LCD sample application

- It is a simple SSD1306 LCD control application written in C.
- The base code comes from the iliapenev [1]. 
- Original implementation uses Wiriing Pi lib [2], but my implementation combine those two.
- A CH341 based I2C adapter is used and the driver is available at [3].

# Build
- Enable/Disable proper LCD size from ssd1306_i2c.h. (SSD1306_128_64, SSD1306_128_32, or SSD1306_96_16)
- Set correct I2C address from ssd1306_i2c.h.
- Run "make"

# Run
- ssd1306 /dev/YOUR-I2C-DEV-NODE


# References

[1] SSD1306 demo
https://github.com/iliapenev/ssd1306_i2c

[2] Wiriing Pi
https://github.com/WiringPi/WiringPi

[3] CH341 USB to I2C adapter driver
https://github.com/gschorcht/i2c-ch341-usb
