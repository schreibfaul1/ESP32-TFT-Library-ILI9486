
#pragma once

#include "Arduino.h"
#include "FS.h"
#include "SPI.h"
#include "SD.h"
#include "fonts.h"

extern __attribute__((weak)) void tft_info(const char*);
extern __attribute__((weak)) void tp_pressed(uint16_t x, uint16_t y);
extern __attribute__((weak)) void tp_released();

#define TFT_WIDTH       320
#define TFT_HEIGHT      480

//

#define ILI9486_INVOFF  0x20 // Display Inversion OFF
#define ILI9486_INVON   0x21 // Display Inversion ON
#define ILI9486_CASET   0x2A // Display On
#define ILI9486_PASET   0x2B // Page Address Set
#define ILI9486_RAMWR   0x2C // Memory Write
#define ILI9486_MADCTL  0x36 // Memory Data Access Control
    #define MADCTL_MY   0x80 // Bit 7 Parameter MADCTL
    #define MADCTL_MX   0x40 // Bit 6 Parameter MADCTL
    #define MADCTL_MV   0x20 // Bit 5 Parameter MADCTL
    #define MADCTL_ML   0x10 // Bit 4 Parameter MADCTL
    #define MADCTL_BGR  0x08 // Bit 3 Parameter MADCTL
    #define MADCTL_MH   0x04 // Bit 2 Parameter MADCTL
#define ILI9486_WDBVAL  0x51 // Write Display Brightness Value
#define ILI9486_CDBVAL  0x53 // Write Control Display Value

// Color definitions
#define TFT_BLACK       0x0000 /*   0,   0,   0 */
#define TFT_NAVY        0x000F /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0 /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF /*   0, 128, 128 */
#define TFT_MAROON      0x7800 /* 128,   0,   0 */
#define TFT_PURPLE      0x780F /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0 /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618 /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF /* 128, 128, 128 */
#define TFT_BLUE        0x001F /*   0,   0, 255 */
#define TFT_GREEN       0x07E0 /*   0, 255,   0 */
#define TFT_CYAN        0x07FF /*   0, 255, 255 */
#define TFT_RED         0xF800 /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0 /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF /* 255, 255, 255 */
#define TFT_ORANGE      0xFD20 /* 255, 165,   0 */
#define TFT_GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define TFT_PINK        0xF81F



class TFT : public Print {
    protected:

    public:
        TFT();
        virtual ~TFT(){}

        void      begin(uint8_t CS=22, uint8_t DC=21, uint8_t MOSI=23, uint8_t MISO=19, uint8_t SCK=18);

        void      setRotation(uint8_t r);
        bool 	  setCursor(uint16_t x, uint16_t y);
        void      invertDisplay(boolean i);
//        void      scrollTo(uint16_t y);

virtual size_t    write(uint8_t);
virtual size_t 	  write(const uint8_t *buffer, size_t size);

        // Recommended Non-Transaction
        void 	  drawLine(int16_t Xpos0, int16_t Ypos0, int16_t Xpos1, int16_t Ypos1, uint16_t color);
        void      drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void      drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void      drawRect(int16_t Xpos, int16_t Ypos, uint16_t Width, uint16_t Height,	uint16_t Color);
        void      fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void 	  drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
        void 	  fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
        void      fillScreen(uint16_t color);
        void      drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void 	  fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void 	  drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        void 	  fillCircle(int16_t Xm, int16_t Ym, uint16_t r, uint16_t color);
        void      setBrigthness(uint8_t br);
        boolean   drawBmpFile(fs::FS &fs, const char * path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0);
        uint16_t  color565(uint8_t r, uint8_t g, uint8_t b);
        size_t    writeText(const uint8_t *str, uint16_t len);

        inline void setTextColor(uint16_t color){_textcolor=color;}
    	inline void setFont(const uint16_t* font){ _font=font;}// the name of the font
    	inline void setTextSize(uint8_t size){if(size==1) _font=Garamond15x18;
    										  if(size==2) _font=Garamond17x21;
    										  if(size==3) _font=Garamond19x24;
    										  if(size==4) _font=Garamond27x33;
    										  if(size==5) _font=Garamond34x42;
    										  if(size==6) _font=Garamond44x54;
    	                                      if(size==7) _font=Garamond88x108;}
    	inline void setTextOrientation(uint8_t orientation=0){_textorientation=orientation;} //0 h other v
    	inline void setUTF8encoder(boolean UTF8){if(UTF8==true) _f_utf8=true; else _f_utf8=false;}
    	int16_t height(void) const;
        int16_t width(void) const;
        uint8_t getRotation(void) const;

    private:
        uint32_t  _freq;
        uint16_t  _height;
        uint16_t  _width;
        uint8_t   _rotation;
        uint16_t  _curX=0;
        int16_t   _curY=0;
        uint16_t  _textcolor = TFT_BLACK;
        uint8_t   _textorientation=0;
        boolean   _f_utf8=false;
        const uint16_t * _font=Garamond17x21;
        boolean   _f_curPos=false;
        uint8_t  TFT_DC  = 21;    /* Data or Command */
    	uint8_t  TFT_CS  = 22;    /* SPI Chip select */
    	uint8_t  TFT_BL  = 17;    /* BackLight */
    	uint8_t  TFT_SCK = 18;
    	uint8_t  TFT_MISO= 19;
    	uint8_t  TFT_MOSI= 23;
    	char 	 sbuf[256];

        inline void TFT_DC_HIGH() {GPIO.out_w1ts = (1 << TFT_DC);}
    	inline void TFT_DC_LOW()  {GPIO.out_w1tc = (1 << TFT_DC);}
    	inline void TFT_CS_HIGH() {GPIO.out_w1ts = (1 << TFT_CS);}
    	inline void TFT_CS_LOW()  {GPIO.out_w1tc = (1 << TFT_CS);}
    	inline void _swap_int16_t(int16_t a, int16_t b) { int16_t t = a; a = b; b = t; }
    	void 	    init();
        void        writeCommand(uint16_t cmd);
        const uint8_t* UTF8toASCII(const uint8_t* str);

        // Transaction API not used by GFX
        void      setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void      writePixel(uint16_t color);
        void      writePixels(uint16_t * colors, uint32_t len);
        void      writeColor(uint16_t color, uint32_t len);
        void      pushColor(uint16_t color);

        // Required Non-Transaction
        void      drawPixel(int16_t x, int16_t y, uint16_t color);

        // Transaction API
        void      startWrite(void);
        void      endWrite(void);
        void      writePixel(int16_t x, int16_t y, uint16_t color);
        void      writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void      writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void      writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

        void 	  fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
        void 	  drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
        void      startBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void      endBitmap();

        void      bmpSkipPixels(fs::File &file, uint8_t bitsPerPixel, size_t len);
        void      bmpAddPixels(fs::File &file, uint8_t bitsPerPixel, size_t len);
        void      drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);
};

//Kalibrierung
//x,y | Ux,Uy  0  ,0     | 1910,1906
//x,y | Ux,Uy  320,0     |  240,1906
//x,y | Ux,Uy  0  ,480   | 1910,140
//x,y | Ux,Uy  320,480   |  240,140
//daraus ergib sich für x: (1910-240)/320 = 5,2187mV pro Pixel
//              und für y: (1906-140)/480 = 3,6791mV pro Pixel




class TP {
    public:

        TP(uint8_t TP_CS, uint8_t TP_IRQ);
        void loop();
        void setRotation(uint8_t m);
    private:
        SPISettings TP_SPI;
        uint8_t TP_CS, TP_IRQ;
        uint16_t x=0, y=0;
        uint8_t _rotation;
        boolean f_loop=false;
        //const uint8_t TP_Dummy=0x80; //nur Startbit für XPT2046
        float xFaktor;
        float yFaktor;
        const uint16_t Xmax=1910;
        const uint16_t Xmin=240;
        const uint16_t Ymax=1906;
        const uint16_t Ymin=140;

    protected:
        uint16_t TP_Send(uint8_t set_val);
        bool read_TP(uint16_t& x, uint16_t& y);
};


