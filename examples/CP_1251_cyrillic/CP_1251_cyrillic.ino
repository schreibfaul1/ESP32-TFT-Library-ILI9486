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
    tft.setFont(Times_New_Roman38x31);
    tft.setTextColor(TFT_WHITE);
    txt="Парус – a poem by M.Lermontow\n";
    tft.println(txt);
    tft.setTextColor(TFT_YELLOW);
    txt= "Белеет парус одинокий\n";
    txt+="В тумане моря голубом!..\n";
    txt+="Что ищет он в стране далекой?,\n";
    txt+="Что кинул он в краю родном?..\n";
    tft.println(txt);
}
//-------------------------------------------------------------------------------------
void loop(void) {
        delay(2000);
}
