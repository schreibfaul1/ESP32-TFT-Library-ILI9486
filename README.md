# ESP32-TFT-Library-ILI9486
A library for 3.5 inch RPi LCD (A) 320x480 display from Waveshare  
![Display](https://github.com/schreibfaul1/ESP32-TFT-Library-ILI9486/blob/master/TFT_ILI9486_XPT2046/images/Waveshare%20RPi%203.5.jpg)

Create new fonts with MikroElektronika GLCD Font Creator and insert the new font in fonts.h  
You can also display bitmaps, touchpadcontroller XPT2046 is included  
Examplecodes: 
  
````
#include "Arduino.h"
#include "SPI.h"
#include "tft.h"

// defaults can be changed in tft.begin
// CS=22, DC=21, MOSI=23, MISO=19, SCK=18

TFT tft;
//-------------------------------------------------------------------------------------
void setup() {
    SPI.begin();
    tft.begin();
    tft.setRotation(1); //landscape
    tft.fillScreen(TFT_BLACK);
    tft.setFont(Garamond34x42);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(20,30);
    tft.print("Hello World!");
}
//-------------------------------------------------------------------------------------
void loop(void) {
    for(uint8_t rotation=0; rotation<4; rotation++) {
        tft.setRotation(rotation);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20,30);
        tft.print("Hello World!");
        delay(3000);
    }
}
//-------------------------------------------------------------------------------------
````
Diaplay a bitmap
````
#include "Arduino.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "tft.h"

TFT tft;

void setup() {
    SPI.begin();
    tft.begin();
    SD.begin();
    tft.setRotation(0); //portait
    tft.drawBmpFile(SD, "/wall_e.bmp", 0, 0);
}

//-------------------------------------------------------------------------------------
void loop(void) {
        delay(3000);
}
//-------------------------------------------------------------------------------------
````


