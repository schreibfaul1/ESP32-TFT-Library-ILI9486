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
    tft.setFont(Times_New_Roman34x27);
    tft.setTextColor(TFT_YELLOW);
    txt= "  Hør mig Vætter,\n  Hør mig Thurser,\n";
    txt+="  Hør mig alfer og dværge.\n";
    txt+="  Hør mig Aser,\n  Hør mig Vaner,\n";
    txt+="  Hør mig disedøtre og muspelsønner.\n";
    txt+="  På dette sted mrker vi marken,\n";
    txt+="  På dette sted ære vi guder,\n";
    txt+="  På dette sted holder vi blot.";
    tft.print(txt);
}
//-------------------------------------------------------------------------------------
void loop(void) {
        delay(2000);
}
