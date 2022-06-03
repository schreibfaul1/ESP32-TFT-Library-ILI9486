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
    tft.setFont(Times_New_Roman43x35);
    tft.setTextColor(TFT_YELLOW);
 	txt="Ο Παπάς πρώτα τα γένια του ευλογεί.";
    tft.print(txt);
    tft.setTextColor(TFT_WHITE);
    txt=" The pastor blesses his own beard first\n";
    tft.println(txt);
    tft.setTextColor(TFT_YELLOW);
    txt="Ο τρελός είδε τον μεθυσμένο και φοβήθηκε.";
    tft.print(txt);
    tft.setTextColor(TFT_WHITE);
    txt= " A drunk is more dangerous than a madman";
    tft.print(txt);
}
//-------------------------------------------------------------------------------------
void loop(void) {
        delay(2000);
}
