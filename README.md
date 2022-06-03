# ESP32-TFT-Library-ILI9486
A library for 3.5 inch RPi LCD (A) 320x480 display from Waveshare, for LCD (B) comment line 61 and uncomment line 62 in tft.cpp   
![Display](https://github.com/schreibfaul1/ESP32-TFT-Library-ILI9486/blob/master/images/tested%20displays.png)

Create new fonts with MikroElektronika GLCD Font Creator and insert the new font in fonts.h  
You can also display bitmaps, touchpadcontroller XPT2046 is included  
Examplecodes: 
  
```` c++
#include "Arduino.h"
#include "ili9486.h"

// GPIOs for SPI
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18

// GPIOs for TFT/TP
#define TFT_CS        22
#define TFT_DC         5
#define TP_CS         13
#define TP_IRQ        12

TFT tft;

//-------------------------------------------------------------------------------------
void setup() {

    tft.begin(TFT_CS, TFT_DC, VSPI, SPI_MOSI, SPI_MISO, SPI_SCK);
    tft.setRotation(1); //landscape
    tft.fillScreen(TFT_BLACK);
    tft.setFont(Times_New_Roman66x53);
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
Display a bitmap or jpg file,  thanks to Bodmer for his great work - JPEG Decoder Library 
https://github.com/Bodmer/JPEGDecoder
```` c++
#include "Arduino.h"
#include "SPI.h"
#include "SD_MMC.h"
#include "FS.h"
#include "ili9486.h"

// GPIOs for SPI
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18

// GPIOs for TFT/TP
#define TFT_CS        22
#define TFT_DC         5
#define TP_CS         13
#define TP_IRQ        12

TFT tft;

//-------------------------------------------------------------------------------------
void setup() {
  	pinMode(2, INPUT_PULLUP);
	if(!SD_MMC.begin("/sd", true)){
      	log_e("SD Card Mount Failed");
      	while(1){};  // endless loop
	}
	  tft.setFrequency(40000000);
    tft.begin(TFT_CS, TFT_DC, VSPI, SPI_MOSI, SPI_MISO, SPI_SCK);
}
//-------------------------------------------------------------------------------------
void loop(void) {
        tft.setRotation(0); //portait
        tft.drawBmpFile(SD_MMC, "/wall_e.bmp", 0, 0);
        delay(2000);
        tft.setRotation(3); //landscape
        tft.drawJpgFile(SD_MMC,"/wallpaper1.jpg", 0,0);
        delay(2000);
        tft.drawJpgFile(SD_MMC,"/arduino.jpg", 100,50);
        delay(2000);
}
//-------------------------------------------------------------------------------------
````
Use the touchpad
```` c++
#include "Arduino.h"
#include "SPI.h"
#include "tft.h"

#define TP_IRQ        39
#define TP_CS         16

TFT tft;
TP tp(TP_CS, TP_IRQ);

uint16_t tp_x, tp_y;

void setup() {
    SPI.begin();
    tft.begin();
//  SD.begin();
    tft.setRotation(1); // Use landscape format
    tp.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREENYELLOW);
    tft.setTextSize(2);
}

//-------------------------------------------------------------------------------------
void loop(void) {
    tp.loop();
}
//-------------------------------------------------------------------------------------

// Event from TouchPad
void tp_pressed(uint16_t x, uint16_t y){
    tp_x=x;  tp_y=y;
}
void tp_released(){
    tft.fillRect(100, 100, 80, 40, TFT_BLACK);
    tft.setCursor(100, 100);
    tft.print("PosX="); tft.println(tp_x);
    tft.print("PosY="); tft.println(tp_y);
}
````


