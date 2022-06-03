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

void setup() {
    String txt="";
    pinMode(TP_CS, OUTPUT);
    digitalWrite(TP_CS, HIGH); // disable the touchpad
	tft.begin(TFT_CS, TFT_DC, VSPI, SPI_MOSI, SPI_MISO, SPI_SCK);
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    tft.setCursor(0, 0);
    tft.setFont(Old_English_Text_MT32x32); // uncomment font in ili9486.h
    tft.setTextColor(TFT_WHITE);
    txt= "O rose, thou art sick!\nThe invisible worm,\n";
    txt+="That flies in the nightn\nIn the howling storm.\n";
    txt+="Has found out thy bed\nOf crimson joy,\n";
    txt+="And his dark secret love\n,Does thy life destroy";
    tft.print(txt);
}
//-------------------------------------------------------------------------------------
void loop(void) {
        delay(2000);
}
