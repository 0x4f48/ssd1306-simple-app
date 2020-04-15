/******
Demo for ssd1306 i2c driver for  Raspberry Pi 
******/
#include <stdio.h>
#include "ssd1306_i2c.h"

int main(int argc, char** argv) 
{
	fprintf(stdout, "device: %s\n", argv[1]);

	ssd1306_begin(SSD1306_SWITCHCAPVCC, argv[1], SSD1306_I2C_ADDRESS);

#if 0
	ssd1306_clearDisplay();
	ssd1306_display(); //Adafruit logo is visible
	delay(5000);
#endif

#if 1
	//char* text = "This is demo for SSD1306 i2c driver for Raspberry Pi";
	char* text = "\nWi-Fi:\n\n255.255.255.255";
	ssd1306_setTextSize(1);
	ssd1306_clearDisplay();
	ssd1306_drawString(text);
	ssd1306_display();
	delay(5000);
#endif

#if 1
	ssd1306_dim(1);
	ssd1306_startscrollleft(00,0xFF);
	delay(5000);
#endif

#if 0
	ssd1306_clearDisplay();
	ssd1306_fillRect(0, 0, SSD1306_LCDWIDTH/2, SSD1306_LCDHEIGHT/2, WHITE);
	ssd1306_fillRect(SSD1306_LCDWIDTH/2, SSD1306_LCDHEIGHT/2, SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, WHITE);
	ssd1306_display();
	ssd1306_startscrollright(00,0xFF);
#endif
	return 0;
}
