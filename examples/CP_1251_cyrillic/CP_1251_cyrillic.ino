/*
 * Example for codepage 1251 cyrillic
 *
 *
 * uncomment #include "fonts/Garamond_cyrillic.h" in tft.h
 */



#include "Arduino.h"
#include "SPI.h"
#include "ili9486.h"

#define TFT_CS        22    // do not use GPI032 or GPIO33 here
#define TFT_DC        21    // do not use GPI032 or GPIO33 here
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define TP_CS         16




TFT tft;

void setup() {
    String txt="";
    pinMode(TP_CS, OUTPUT);
    digitalWrite(TP_CS, HIGH); // disable the touchpad
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    tft.begin(TFT_CS, TFT_DC, SPI_MOSI, SPI_MISO, SPI_SCK);
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    tft.setCursor(0, 0);
    tft.setFont(Garamond32x33cyrillic);
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
