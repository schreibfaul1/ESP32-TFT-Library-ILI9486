#include "Arduino.h"
#include "SPI.h"
#include "tft.h"

#define TFT_CS        22
#define TFT_DC        21
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18


#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

TFT tft;


void testText(); // function prototyp

void setup() {
    Serial.begin(115200);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    tft.begin(TFT_CS, TFT_DC, SPI_MOSI, SPI_MISO, SPI_SCK);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
}

//-------------------------------------------------------------------------------------
void loop(void) {
    for(uint8_t rotation=0; rotation<4; rotation++) {
        tft.setRotation(rotation);
        testText();
        delay(3000);
    }
}
//-------------------------------------------------------------------------------------

void testText() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(TFT_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(TFT_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(6);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
}
