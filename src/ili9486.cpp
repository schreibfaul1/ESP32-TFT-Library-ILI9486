/*
 *  tft.cpp
 *
 *  Created on: May 28,2018
 *  Updated on: Jan 22,2022
 *      Author: Wolle (schreibfaul1)
 *
 */



#include "Arduino.h"
#include "ili9486.h"

#define TFT_MAX_PIXELS_AT_ONCE  32

JPEGDecoder JpegDec;

void TFT::init() {
    if(tft_info) tft_info("init ILI9486\n");
    startWrite();
        //Driving ability Setting
        writeCommand(0x11); // Sleep out, also SW reset
        delay(120);

        writeCommand(0x3A); // Interface Pixel Format
        SPI.write16(0x55);

        writeCommand(0xC2); // Power Control 3 (For Normal Mode)
        SPI.write16(0x44);

        writeCommand(0xC5); // VCOM Control
        SPI.write16(0x00);
        SPI.write16(0x00);
        SPI.write16(0x00);
        SPI.write16(0x00);

        writeCommand(0xE0); // PGAMCTRL(Positive Gamma Control)
        SPI.write16(0x0F);
        SPI.write16(0x1F);
        SPI.write16(0x1C);
        SPI.write16(0x0C);
        SPI.write16(0x0F);
        SPI.write16(0x08);
        SPI.write16(0x48);
        SPI.write16(0x98);
        SPI.write16(0x37);
        SPI.write16(0x0A);
        SPI.write16(0x13);
        SPI.write16(0x04);
        SPI.write16(0x11);
        SPI.write16(0x0D);
        SPI.write16(0x00);

        writeCommand(0xE1); // NGAMCTRL (Negative Gamma Correction)
        SPI.write16(0x0F);
        SPI.write16(0x32);
        SPI.write16(0x2E);
        SPI.write16(0x0B);
        SPI.write16(0x0D);
        SPI.write16(0x05);
        SPI.write16(0x47);
        SPI.write16(0x75);
        SPI.write16(0x37);
        SPI.write16(0x06);
        SPI.write16(0x10);
        SPI.write16(0x03);
        SPI.write16(0x24);
        SPI.write16(0x20);
        SPI.write16(0x00);

        writeCommand(0x20); // Display Inversion OFF   RPi LCD (A)
//      writeCommand(0x21); // Display Inversion ON    RPi LCD (B)

        writeCommand(0x36); // Memory Access Control
        SPI.write16(0x48);

        writeCommand(0x29); // Display ON
        delay(150);
    endWrite();
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t TFT::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

TFT::TFT() {
    _freq = 20000000;
    _height = TFT_HEIGHT;
    _width = TFT_WIDTH;
    _rotation=0;
}

void TFT::setFrequency(uint32_t f){
    if(f>80000000) f=80000000;
    _freq=f;  // overwrite default
}

void TFT::startWrite(void){
    SPI.beginTransaction(SPISettings(_freq, MSBFIRST, SPI_MODE0));
    TFT_CS_LOW();
}

void TFT::endWrite(void){
    TFT_CS_HIGH();
    SPI.endTransaction();
}

void TFT::writeCommand(uint16_t cmd){
    TFT_DC_LOW();
    SPI.write16(cmd);
    TFT_DC_HIGH();
}
// Return the size of the display (per current rotation)
int16_t TFT::width(void) const {
    return _width;
}
int16_t TFT::height(void) const {
    return _height;
}
uint8_t TFT::getRotation(void) const{
    return _rotation;
}

void TFT::begin(uint8_t CS, uint8_t DC, uint8_t MOSI, uint8_t MISO, uint8_t SCK){
    String info="";
    TFT_CS=CS; TFT_DC=DC;
    TFT_MOSI=MOSI; TFT_MISO=MISO; TFT_SCK=SCK;


    _width  = TFT_WIDTH;
    _height = TFT_HEIGHT;

    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_DC, LOW);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    info ="TFT_CS:" + String(TFT_CS) + " TFT_DC:" + String(TFT_DC);
    info+=" TFT_MOSI:" + String(TFT_MOSI) + " TFT_MISO:" + String(TFT_MISO) + " TFT_SCK:" + String(TFT_SCK);
    if(tft_info) tft_info(info.c_str());
    SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI, -1);

    init();
}

void TFT::setRotation(uint8_t m) {
    _rotation = m % 4; // can't be higher than 3
       startWrite();
    writeCommand(ILI9486_MADCTL);

    switch (_rotation) {
        case 0:
            SPI.write16(MADCTL_MX |MADCTL_BGR);
            _width  = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 1:
            SPI.write16(MADCTL_MV | MADCTL_BGR);
            _width  = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
        case 2:
            SPI.write16(MADCTL_MY | MADCTL_BGR);
            _width  = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 3:
            SPI.write16(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            _width  = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
    }
    endWrite();
}


/*
 * Transaction API
 * */

void TFT::setBrigthness(uint8_t br){
    startWrite();
    writeCommand(ILI9486_CDBVAL);
    SPI.write16(0);
    writeCommand(ILI9486_WDBVAL);
    SPI.write16(128);
    endWrite();
}

void TFT::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    writeCommand(ILI9486_CASET); // Column addr set
     SPI.write16(x >> 8);
     SPI.write16(x & 0xFF);     // XSTART
     w=x+w-1;
     SPI.write16(w >> 8);
     SPI.write16(w & 0xFF);     // XEND

     writeCommand(ILI9486_PASET); // Row addr set
     SPI.write16(y >> 8);
     SPI.write16(y & 0xFF);     // YSTART
     h=y+h-1;
     SPI.write16(h >> 8);
     SPI.write16(h & 0xFF);     // YEND
}

void TFT::startBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    startWrite();
        writeCommand(ILI9486_MADCTL);
        if(_rotation==0){SPI.write16(MADCTL_MX | MADCTL_MY | MADCTL_ML | MADCTL_BGR);}
        if(_rotation==1){SPI.write16(MADCTL_MH | MADCTL_MV | MADCTL_MX | MADCTL_BGR);}
        if(_rotation==2){SPI.write16(MADCTL_MH | MADCTL_BGR);}
        if(_rotation==3){SPI.write16(MADCTL_MV | MADCTL_MY | MADCTL_BGR);}

        setAddrWindow(x, _height - y - h ,w ,h);
        writeCommand(ILI9486_RAMWR);
    endWrite();
}

void TFT::endBitmap() {
    setRotation(_rotation);
}

void TFT::startJpeg() {
    startWrite();
        writeCommand(ILI9486_MADCTL);
        if(_rotation==0){SPI.write16(MADCTL_MH | MADCTL_MX |MADCTL_BGR);}
        if(_rotation==1){SPI.write16(MADCTL_MV | MADCTL_BGR);}
        if(_rotation==2){SPI.write16(MADCTL_MY | MADCTL_BGR);}
        if(_rotation==3){SPI.write16(MADCTL_MV | MADCTL_MY | MADCTL_MX |MADCTL_BGR);}
    endWrite();
}

void TFT::endJpeg() {
        setRotation(_rotation);
}

void TFT::pushColor(uint16_t color) {
    startWrite();
        SPI.write16(color);
    endWrite();
}

void TFT::writePixel(uint16_t color){
    SPI.write16(color);
}

void TFT::writePixels(uint16_t * colors, uint32_t len){
    SPI.writePixels((uint8_t*)colors , len * 2);
}

void TFT::writeColor(uint16_t color, uint32_t len){
    static uint16_t temp[TFT_MAX_PIXELS_AT_ONCE];
    size_t blen = (len > TFT_MAX_PIXELS_AT_ONCE)?TFT_MAX_PIXELS_AT_ONCE:len;
    uint16_t tlen = 0;

    for (uint32_t t=0; t<blen; t++){
        temp[t] = color;
    }

    while(len){
        tlen = (len>blen)?blen:len;
        writePixels(temp, tlen);
        len -= tlen;
    }
}

void TFT::writePixel(int16_t x, int16_t y, uint16_t color) {
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x,y,1,1);
    writeCommand(ILI9486_RAMWR);
    writePixel(color);
}

void TFT::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
    if((x >= _width) || (y >= _height)) return;

    int16_t x2 = x + w - 1, y2 = y + h - 1;
    if((x2 < 0) || (y2 < 0)) return;

    // Clip left/top
    if(x < 0) {
        x = 0;
        w = x2 + 1;
    }
    if(y < 0) {
        y = 0;
        h = y2 + 1;
    }

    // Clip right/bottom
    if(x2 >= _width)  w = _width  - x;
    if(y2 >= _height) h = _height - y;

    int32_t len = (int32_t)w * h;
    setAddrWindow(x, y, w, h);
    writeCommand(ILI9486_RAMWR);
    writeColor(color, len);
}

void TFT::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color){
    writeFillRect(x, y, 1, h, color);
}

void TFT::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color){
    writeFillRect(x, y, w, 1, color);
}


void TFT::drawPixel(int16_t x, int16_t y, uint16_t color){
    startWrite();
    writePixel(x, y, color);
    endWrite();
}

void TFT::drawFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    startWrite();
    writeFastVLine(x, y, h, color);
    endWrite();
}

void TFT::drawFastHLine(int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    startWrite();
    writeFastHLine(x, y, w, color);
    endWrite();
}
/*******************************************************************************/
void TFT::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,  uint16_t color) {
    // Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
    // an eficient FastH/V Line draw routine for line segments of 2 pixels or more
    int16_t t;
    boolean steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        t=x0; x0=y0; y0=t;  // swap (x0, y0);
        t=x1; x1=y1; y1=t;  // swap(x1, y1);
    }
    if (x0 > x1) {
        t=x0; x0=x1; x1=t;  // swap(x0, x1);
        t=y0; y0=y1; y1=t;  // swap(y0, y1);
    }
    int16_t dx = x1 - x0, dy = abs(y1 - y0);;
    int16_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

    if (y0 < y1) ystep = 1;
    startWrite();
    // Split into steep and not steep for FastH/V separation
    if (steep) {
        for (; x0 <= x1; x0++) {
            dlen++;
            err -= dy;
            if (err < 0) {
                err += dx;
                if (dlen == 1) writePixel(y0, xs, color);
                else writeFastVLine(y0, xs, dlen, color);
                dlen = 0; y0 += ystep; xs = x0 + 1;
            }
        }
        if (dlen) writeFastVLine(y0, xs, dlen, color);
    }
    else {
        for (; x0 <= x1; x0++) {
            dlen++;
            err -= dy;
            if (err < 0) {
                err += dx;
                if (dlen == 1) writePixel(xs, y0, color);
                else writeFastHLine(xs, y0, dlen, color);
                dlen = 0; y0 += ystep; xs = x0 + 1;
            }
        }
        if (dlen) writeFastHLine(xs, y0, dlen, color);
    }
    endWrite();
}

void TFT::fillScreen(uint16_t color) {
    // Update in subclasses if desired!
    fillRect(0, 0, width(), height(), color);
}
void TFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    startWrite();
    writeFillRect(x,y,w,h,color);
    endWrite();
}
/*******************************************************************************/
void TFT::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}
/*******************************************************************************/
void TFT::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    startWrite();
    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        writeFastHLine(a, y0, b - a + 1, color);
        endWrite();
        return;
    }
    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
            dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2)
        last = y1;   // Include y1 scanline
    else
        last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
         a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b)
            _swap_int16_t(a, b);
        writeFastHLine(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
         a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b)
            _swap_int16_t(a, b);
        writeFastHLine(a, y, b - a + 1, color);
    }
    endWrite();
}
void TFT::drawRect(int16_t Xpos, int16_t Ypos, uint16_t Width, uint16_t Height,    uint16_t Color)
{
    if(Width<1 || Height<1) return;
    startWrite();
    writeFastHLine(Xpos, Ypos, Width, Color);
    writeFastHLine(Xpos, Ypos + Height-1, Width, Color);
    writeFastVLine(Xpos, Ypos, Height, Color);
    writeFastVLine(Xpos + Width-1, Ypos, Height, Color);
    endWrite();
}

void TFT::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    startWrite();
    writeFastHLine(x + r, y, w - 2 * r, color); // Top
    writeFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
    writeFastVLine(x, y + r, h - 2 * r, color); // Left
    writeFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
    endWrite();
}
/*******************************************************************************/
void TFT::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    startWrite();
    writeFillRect(x + r, y, w - 2 * r, h, color);

    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
    endWrite();
}
/*******************************************************************************/
void TFT::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    startWrite();
    writePixel(x0, y0 + r, color);
    writePixel(x0, y0 - r, color);
    writePixel(x0 + r, y0, color);
    writePixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 - x, y0 + y, color);
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 - x, y0 - y, color);
        writePixel(x0 + y, y0 + x, color);
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 + y, y0 - x, color);
        writePixel(x0 - y, y0 - x, color);
    }
    endWrite();
}
/*******************************************************************************/
void TFT::fillCircle(int16_t Xm,       //specify x position.
                     int16_t Ym,       //specify y position.
                     uint16_t  r, //specify the radius of the circle.
                     uint16_t color)  //specify the color of the circle.
{
    signed int f = 1 - r, ddF_x = 1, ddF_y = 0 - (2 * r), x = 0, y = r;
    startWrite();
    writeFastVLine(Xm, Ym - r, 2 * r, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        writeFastVLine(Xm + x, Ym - y, 2 * y, color);
        writeFastVLine(Xm - x, Ym - y, 2 * y, color);
        writeFastVLine(Xm + y, Ym - x, 2 * x, color);
        writeFastVLine(Xm - y, Ym - x, 2 * x, color);
    }
    endWrite();
}
/*******************************************************************************/
void TFT::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            writePixel(x0 + x, y0 + y, color);
            writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            writePixel(x0 + x, y0 - y, color);
            writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            writePixel(x0 - y, y0 + x, color);
            writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            writePixel(x0 - y, y0 - x, color);
            writePixel(x0 - x, y0 - y, color);
        }
    }
}
/*******************************************************************************/
void TFT::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            writeFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            writeFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            writeFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            writeFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}
/*******************************************************************************/
bool TFT::setCursor(uint16_t x, uint16_t y) {

    if (x >= width()|| y >= height()) {
        return false;
    }
    writeCommand(0x2A);
    SPI.write(x >> 8);
    SPI.write(x & 0xFF);
    writeCommand(ILI9486_RAMWR); //Column Start
    writeCommand(0x2B);
    SPI.write(y >> 8);
    SPI.write(y & 0xFF);
    writeCommand(ILI9486_RAMWR); //Row Start
    _curX=x;
    _curY=y;
    _f_curPos=true;  //curPos is updated
    return true;
}
/*******************************************************************************/
size_t TFT::writeText(const uint8_t *str, int16_t maxHeight) {    // a pointer to string

    int16_t sHeight = height();
    int16_t sWidth =  width();
    if(maxHeight > 0){
        sHeight = maxHeight;
    }  

    uint16_t len=0;
    while(str[len]!=0)len++;  // determine length of text

    static int16_t xC=64;
    static int16_t tmp_curX=0;
    static int16_t tmp_curY=0;

    if(_f_curPos==true){tmp_curX=_curX; tmp_curY=_curY; _f_curPos=false;} //new CursorValues?

    boolean  f_wrap=false;
    uint16_t color=_textcolor;
    int16_t  Xpos=tmp_curX;
    int16_t  Ypos=tmp_curY;
    int16_t  Ypos0 = Ypos;
    int16_t  Xpos0 = Xpos;
    uint16_t font_char=0;
    int16_t  i=0;
    uint16_t j=0;
    int      a=0;
    uint16_t font_height = _font[6];
    startWrite();

    while(i != len) {  //until string ends
        int strw=0;
        //------------------------------------------------------------------  word wrap
        a=i+1 ;
        if(str[i] == 32) { // space
            strw=font_height/4; // erstes Leerzeichen
            uint16_t fi=8;
            fi=fi + (str[i] - 32) * 4;
            strw=strw + _font[fi] +1;
            while((str[a] != 32) && (a < len)) {
                fi=8;
                if((_f_utf8)&&(str[a]>=0xC2)){ //next char is UTF-8
                    uint16_t ch=str[a]; ch<<=8; ch+=str[a+1];
                        if((ch<0xD4B0)) { // char is in range, is not a armenian char or higher
                            xC=(str[a]-0xC2)*64;  a++; fi+=(str[a]+xC-32)*4; // UTF-8 decoding
                        }
                        else {
                            fi+=(str[a] - 32) * 4;
                        }
                }
                else {
                    fi+=(str[a] - 32) * 4;
                }
                strw=strw + _font[fi] +1;
                a++;
                if(str[a]=='\n') break;  // text defined word wrap recognised
            }
            if(_textorientation == 0) {
                if((Xpos + strw) >= sWidth) f_wrap = true;
            }
            else{
                if((Ypos+strw) >= sHeight) f_wrap=true;
            }
        }
        //------------------------------------------------------------------ word wrap end

        font_char = str[i];     //die ersten 32 ASCII-Zeichen sind nicht im Zeichensatz enthalten
        if((str[i]==32) && (f_wrap==true)) { font_char='\n'; f_wrap=false; }
        if(font_char>=32)       // it is a printable char
        {
            if(_f_utf8) {
                if((font_char>=0xC2)&&(font_char<0xd5)) {
                    if((font_char==0xd4)&&(str[i+1]>0xAF)) {} // do nothing, it is a armenian character or higher
                    else {
                        xC=(font_char-0xC2)*64;  i++; font_char = str[i]+xC; // UTF-8 decoding
                    }
                }
                if((font_char==0xE2)&&(str[i+1]==0x80)) { // general punctuation, three bytes
                    i+=2;
                    font_char=32; // set blank
                }
            }
            font_char-=32;
            uint16_t font_index = 8; // begins at position 8 ever
            font_index = font_index + font_char * 4;
            uint16_t char_width = _font[font_index];
            uint16_t space;
            if(font_char==0) space=font_height/4; else space=0; //correct spacewidth is 1
            if(_textorientation==0) {
                if((Xpos+char_width+space)>=sWidth){Xpos=_curX; Ypos+=font_height; Xpos0=Xpos; Ypos0=Ypos;}
                if((Ypos+font_height)>=sHeight){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
            }
            else {
                if((Ypos+char_width+space)>sHeight){Ypos=_curY; Xpos-=font_height; Xpos0=Xpos; Ypos0=Ypos;}
                if((Xpos-font_height)<0){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
            }
            uint16_t char_bytes = (char_width - 1) / 8 + 1; //number of bytes for a character
            uint32_t font_offset;
            font_offset = _font[font_index + 3]; //MSB
            font_offset <<= 8; // shift left 8 times
            font_offset += _font[font_index + 2];
            font_offset <<= 8;
            font_offset += _font[font_index + 1]; //LSB
            //ab font_offset stehen die Infos für das Zeichen
            int16_t n = 0;
            for (uint16_t k = 0; k < font_height; k++) {
                for (int16_t m = 0; m < char_bytes; m++) {
                    uint16_t chTemp = (_font[font_offset + n]);
                    n++;
                    if (_textorientation == 0) {
                        for (j = 0; j < 8; j++) {
                            if (chTemp & 0x01) {
                                writePixel(Xpos, Ypos, color);
                            }
                            chTemp >>= 1;
                            Xpos++;
                            if ((Xpos - Xpos0) == char_width) {
                                Xpos = Xpos0; Ypos++; break;
                            }
                        }
                    }
                    else {
                        for (j = 0; j < 8; j++) {
                            if (chTemp & 0x01) writePixel(Xpos, Ypos, color);
                            chTemp >>= 1;
                            Ypos++;

                            if ((Ypos - Ypos0) == char_width) {
                                Ypos = Ypos0; Xpos--; break;
                            }
                        }
                    }
                }
            }
            if (_textorientation == 0) {
                Ypos = Ypos0; Xpos0 = Xpos0 + char_width + 1 + space; Xpos = Xpos0;
            }
            else {
                Xpos = Xpos0; Ypos0 = Ypos0 + char_width + 1 + space; Ypos = Ypos0;
            }
        } // end if(font_char>=0)
        else {  // das ist ein Steuerzeichen
            //if(str[i]==10) {  //CRLF
            if(font_char==10){
                if(_textorientation==0){
                    {Xpos=_curX; Ypos+=font_height; Xpos0=Xpos; Ypos0=Ypos;}
                    if((Ypos+font_height)>sHeight){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
                }
                else{
                    {Ypos=_curY; Xpos-=font_height; Xpos0=Xpos; Ypos0=Ypos;}
                    if((Ypos+font_height)>sHeight){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
                }
            }
        }
        i++;
    } // end while
    tmp_curX=Xpos;
    tmp_curY=Ypos;

    endWrite();
    return i;
}
/*******************************************************************************/
size_t TFT::write(uint8_t character) {
    /*Code to display letter when given the ASCII code for it*/
    return 0;
}
size_t TFT::write(const uint8_t *buffer, size_t size){
    if(_f_cp1251){writeText(UTF8toCp1251(buffer)); return 0;}
    if(_f_cp1252){writeText(UTF8toCp1252(buffer)); return 0;}
    if(_f_cp1253){writeText(UTF8toCp1253(buffer)); return 0;}
    writeText(buffer); return 0;
}
/*******************************************************************************/
const uint8_t* TFT::UTF8toCp1251(const uint8_t* str){  //cyrillic
    uint16_t i=0, j=0;
    boolean k=false;
    while((str[i]!=0)&&(i<1024)){
        if(str[i]==0xD0){
            if((str[i+1]>=0x90)&&(str[i+1]<=0xBF)){
                buf[j]=str[i+1]+0x30; k=true;
            }
            if(str[i+1]==0x81){
                buf[j]=0xA8; k=true;
            }
        }
        if(str[i]==0xD1){
            if((str[i+1]>=0x80)&&(str[i+1]<=0x8F)){
                buf[j]=str[i+1]+0x70; k=true;
            }
            if(str[i+1]==0x91){
                buf[j]=0xB8; k=true;
            }
        }
        if(k==false){
            buf[j]=str[i];
            i++; j++;
        }
        else{
           k=false;
           i+=2; j++;
        }
    }
    buf[j]=0;
    return (buf);
}

const uint8_t* TFT::UTF8toCp1252(const uint8_t* str){  //WinLatin1
    uint16_t i=0, j=0;
    while((str[i]!=0)&&(i<1024)){
        if(str[i]<127){
            buf[j]=str[i];
            i++; j++;
        }
        else if(str[i]>=192 && str[i]<=195){  // 0xC0, 0xC1, 0xC2, 0xC3
            buf[j]=(str[i]-192)*64+(str[i+1]-128);
            i+=2; j++;
        }
        else{
            buf[j]=str[i];
            i++; j++;
        }
    }
    buf[j]=0;
    return (buf);
}

const uint8_t* TFT::UTF8toCp1253(const uint8_t* str){  //Greek
    uint16_t i=0, j=0;
    while((str[i]!=0)&&(i<1024)){
        if(str[i]<184){
            buf[j]=str[i];
            i++; j++;
        }
        else if(str[i]==206){  // 0xCE
            if((str[i+1]>=136)&&(str[i+1]<=191)){ //0xCE88..0xCEBF
                buf[j]=str[i+1]+48;
                i+=2; j++;
            }
            else{
                buf[j]=str[i];
                i+=2; j++;
            }
        }
        else if(str[i]==207){  // 0xCF
            if((str[i+1]>=128)&&(str[i+1]<=142)){ //0xCF88..0xCF8E
                buf[j]=str[i+1]+112;
                i+=2; j++;
            }
            else{
                buf[j]=str[i];
                i+=2; j++;
            }
        }
        else{
            buf[j]=str[i];
            i++; j++;
        }
    }
    buf[j]=0;
    return (buf);
}
/*******************************************************************************************************************
                                            B I T M A P S
*******************************************************************************************************************/

#define bmpRead32(d,o) (d[o] | (uint16_t)(d[(o)+1]) << 8 | (uint32_t)(d[(o)+2]) << 16 | (uint32_t)(d[(o)+3]) << 24)
#define bmpRead16(d,o) (d[o] | (uint16_t)(d[(o)+1]) << 8)

#define bmpColor8(c)       (((uint16_t)(((uint8_t*)(c))[0] & 0xE0) << 8) | ((uint16_t)(((uint8_t*)(c))[0] & 0x1C) << 6) | ((((uint8_t*)(c))[0] & 0x3) << 3))
#define bmpColor16(c)      ((((uint8_t*)(c))[0] | ((uint16_t)((uint8_t*)(c))[1]) << 8))
#define bmpColor24(c)      (((uint16_t)(((uint8_t*)(c))[2] & 0xF8) << 8) | ((uint16_t)(((uint8_t*)(c))[1] & 0xFC) << 3) | ((((uint8_t*)(c))[0] & 0xF8) >> 3))
#define bmpColor32(c)      (((uint16_t)(((uint8_t*)(c))[3] & 0xF8) << 8) | ((uint16_t)(((uint8_t*)(c))[2] & 0xFC) << 3) | ((((uint8_t*)(c))[1] & 0xF8) >> 3))

void TFT::bmpSkipPixels(fs::File &file, uint8_t bitsPerPixel, size_t len){
    size_t bytesToSkip = (len * bitsPerPixel) / 8;
    file.seek(bytesToSkip, SeekCur);
}

void TFT::bmpAddPixels(fs::File &file, uint8_t bitsPerPixel, size_t len){
    size_t bytesPerTransaction = bitsPerPixel * 4;
    uint8_t transBuf[bytesPerTransaction];
    uint16_t pixBuf[32];
    uint8_t * tBuf;
    uint8_t pixIndex = 0;
    size_t wIndex = 0, pixNow, bytesNow;
    while(wIndex < len){
        pixNow = len - wIndex;
        if(pixNow > 32){
            pixNow = 32;
        }
        bytesNow = (pixNow * bitsPerPixel) / 8;
        file.read(transBuf, bytesNow);
        tBuf = transBuf;

        for(pixIndex=0; pixIndex < pixNow; pixIndex++){
            if(bitsPerPixel == 32){
                pixBuf[pixIndex] = (bmpColor32(tBuf));
                tBuf+=4;
            } else if(bitsPerPixel == 24){
                pixBuf[pixIndex] = (bmpColor24(tBuf));
                tBuf+=3;
            } else if(bitsPerPixel == 16){
                pixBuf[pixIndex] = (bmpColor16(tBuf));
                tBuf+=2;
            } else if(bitsPerPixel == 8){
                pixBuf[pixIndex] = (bmpColor8(tBuf));
                tBuf+=1;
            } else if(bitsPerPixel == 4){
                uint16_t g = tBuf[0] & 0xF;
                if(pixIndex & 1){
                    tBuf+=1;
                } else {
                    g = tBuf[0] >> 4;
                }
                pixBuf[pixIndex] = ((g << 12) | (g << 7) | (g << 1));
            }
        }
        startWrite();
        writePixels(pixBuf, pixNow);
        endWrite();
        wIndex += pixNow;
    }
}

boolean TFT::drawBmpFile(fs::FS &fs, const char * path, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY){
    if((x + maxWidth) > width() || (y + maxHeight) > height()){
        log_e("Bad dimensions given");
        return false;
    }

    if(!maxWidth){
        maxWidth = width() - x;
    }
    if(!maxHeight){
        maxHeight = height() - y;
    }
    //log_e("maxWidth=%i, maxHeight=%i", maxWidth, maxHeight);
    File file = fs.open(path);
    if(!file){
        if(tft_info) tft_info("Failed to open file for reading\n");
        return false;
    }
    size_t headerLen = 0x22;
    size_t fileSize = file.size();
    uint8_t headerBuf[headerLen];
    if(fileSize < headerLen || file.read(headerBuf, headerLen) < headerLen){
        log_e("Failed to read the file's header");
        file.close();
        return false;
    }

    if(headerBuf[0] != 'B' || headerBuf[1] != 'M'){
        log_e("Wrong file format");
        file.close();
        return false;
    }

    //size_t bmpSize = bmpRead32(headerBuf, 0x02);
    uint32_t dataOffset = bmpRead32(headerBuf, 0x0A);
    int32_t bmpWidthI = bmpRead32(headerBuf, 0x12);
    int32_t bmpHeightI = bmpRead32(headerBuf, 0x16);
    uint16_t bitsPerPixel = bmpRead16(headerBuf, 0x1C);

    size_t bmpWidth = abs(bmpWidthI);
    size_t bmpHeight = abs(bmpHeightI);

    if(offX >= bmpWidth || offY >= bmpHeight){
        log_e("Offset Outside of bitmap size");
        file.close();
        return false;
    }

    size_t bmpMaxWidth = bmpWidth - offX;
    size_t bmpMaxHeight = bmpHeight - offY;
    size_t outWidth = (bmpMaxWidth > maxWidth)?maxWidth:bmpMaxWidth;
    size_t outHeight = (bmpMaxHeight > maxHeight)?maxHeight:bmpMaxHeight;
    size_t ovfWidth = bmpMaxWidth - outWidth;
    size_t ovfHeight = bmpMaxHeight - outHeight;

    file.seek(dataOffset);
    startBitmap(x, y, outWidth, outHeight);

    if(ovfHeight){
        bmpSkipPixels(file, bitsPerPixel, ovfHeight * bmpWidth);
    }
    if(!offX && !ovfWidth){
        bmpAddPixels(file, bitsPerPixel, outWidth * outHeight);
    } else {
        size_t ih;
        for(ih=0;ih<outHeight;ih++){
            if(offX){
                bmpSkipPixels(file, bitsPerPixel, offX);
            }
            bmpAddPixels(file, bitsPerPixel, outWidth);
            if(ovfWidth){
                bmpSkipPixels(file, bitsPerPixel, ovfWidth);
            }
        }
    }

    endBitmap();
    file.close();
    return true;
}

/*******************************************************************************************************************
                                            J P E G
*******************************************************************************************************************/

boolean TFT::drawJpgFile(fs::FS &fs, const char * path, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY){
    if((x + maxWidth) > width() || (y + maxHeight) > height()){
        log_e("Bad dimensions given");
        return false;
    }
    //log_e("maxWidth=%i, maxHeight=%i", maxWidth, maxHeight);
    File file = fs.open(path);
    if(!file){
        if(tft_info) tft_info("Failed to open file for reading\n");
        return false;
    }
    JpegDec.decodeSdFile(file);

//    log_i("Width: %i, Height: %i,", JpegDec.width, JpegDec.height);
//    log_i("Components: %i, Scan type %i", JpegDec.comps, JpegDec.scanType);
//    log_i("MCU / row: %i, MCU / col: %i", JpegDec.MCUSPerRow, JpegDec.MCUSPerCol);
//    log_i("MCU width: %i, MCU height: %i", JpegDec.MCUWidth, JpegDec.MCUHeight);

    renderJPEG(x, y, maxWidth, maxHeight);

    return true;
}

void TFT::renderJPEG(int xpos, int ypos, uint16_t maxWidth, uint16_t maxHeight) {
    // retrieve infomration about the image
    uint16_t *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    uint32_t max_x = JpegDec.width;
    uint32_t max_y = JpegDec.height;

    maxWidth  = (uint)(maxWidth  / 16) * 16;  // must be a multiple of 16 (MCU blocksize)
    //log_i("maxWidth %d", maxWidth );
    maxHeight = (uint)(maxHeight / 16) * 16;   // must be a multiple of 8

    if(maxWidth  > 0 && maxWidth  < max_x) max_x = maxWidth;  // overwrite
    if(maxHeight > 0 && maxHeight < max_y) max_y = maxHeight; // overwrite


    // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
    // Typically these MCUs are 16x16 pixel blocks
    // Determine the width and height of the right and bottom edge image blocks
    uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
    uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

    // save the current image block size
    uint32_t win_w = mcu_w;
    uint32_t win_h = mcu_h;

    // record the current time so we can measure how long it takes to draw an image
    uint32_t drawTime = millis();

    // save the coordinate of the right and bottom edges to assist image cropping to the screen size
    max_x += xpos;
    max_y += ypos;

    // read each MCU block until there are no more
    startJpeg();

    while( JpegDec.read()){
        // save a pointer to the image block
        pImg = JpegDec.pImage;

        // calculate where the image block should be drawn on the screen
        int16_t mcu_x = JpegDec.MCUx * mcu_w + xpos;
        int16_t mcu_y = JpegDec.MCUy * mcu_h + ypos;

        // check if the image block size needs to be changed for the right and bottom edges
        if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
        else win_w = min_w;
        if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
        else win_h = min_h;

        //log_i("mcu_x=%i, mcu_y=%i", mcu_x, mcu_y);

        // calculate how many pixels must be drawn
        uint32_t mcu_pixels = win_w * win_h;

        // draw image block if it will fit on the screen
        if ( ( mcu_x + win_w) <= width() && ( mcu_y + win_h) <= height()) {
            // open a window onto the screen to paint the pixels into
            startWrite();
                setAddrWindow(mcu_x, mcu_y, win_w , win_h);
                writeCommand(ILI9486_RAMWR);
                // push all the image block pixels to the screen
                while (mcu_pixels--) writePixel(*pImg++); // Send to TFT 16 bits at a time
            endWrite();
        }

        // stop drawing blocks if the bottom of the screen has been reached
        // the abort function will close the file
        else if ( ( mcu_y + win_h) >= height()) JpegDec.abort();
    }

    // calculate how long it took to draw the image
    drawTime = millis() - drawTime; // Calculate the time it took

    // print the results to the serial port
    // log_i("Total render time was: %ims",drawTime);
    endJpeg();
}

// JPEGDecoder thx to Bodmer https://github.com/Bodmer/JPEGDecoder
JPEGDecoder::JPEGDecoder(){ // @suppress("Class members should be properly initialized")
    mcu_x = 0 ;
    mcu_y = 0 ;
    is_available = 0;
    thisPtr = this;
}

JPEGDecoder::~JPEGDecoder(){
    if (pImage) delete pImage;
    pImage = NULL;
}

uint8_t JPEGDecoder::pjpeg_callback(uint8_t* pBuf, uint8_t buf_size, uint8_t *pBytes_actually_read, void *pCallback_data) {
    JPEGDecoder *thisPtr = JpegDec.thisPtr ;
    thisPtr->pjpeg_need_bytes_callback(pBuf, buf_size, pBytes_actually_read, pCallback_data);
    return 0;
}

uint8_t JPEGDecoder::pjpeg_need_bytes_callback(uint8_t* pBuf, uint8_t buf_size, uint8_t *pBytes_actually_read, void *pCallback_data) {
    uint n;
    n = jpg_min(g_nInFileSize - g_nInFileOfs, buf_size);
    if (jpg_source == JPEG_ARRAY) { // We are handling an array
        for (int i = 0; i < n; i++) {
            pBuf[i] = pgm_read_byte(jpg_data++);
            //Serial.println(pBuf[i],HEX);
        }
    }
    if (jpg_source == JPEG_SD_FILE) g_pInFileSd.read(pBuf,n); // else we are handling a file
    *pBytes_actually_read = (uint8_t)(n);
    g_nInFileOfs += n;
    return 0;
}

int JPEGDecoder::decode_mcu(void) {
    status = JpegDec.pjpeg_decode_mcu();
    if (status) {
        is_available = 0 ;
        if (status != JpegDec.PJPG_NO_MORE_BLOCKS) {
            log_e("pjpeg_decode_mcu() failed with status %i", status);
            return -1;
        }
    }
    return 1;
}

int JPEGDecoder::read(void){
    int y, x;
    uint16_t *pDst_row;
    if(is_available == 0 || mcu_y >= image_info.m_MCUSPerCol) {
        abort();
        return 0;
    }
    // Copy MCU's pixel blocks into the destination bitmap.
    pDst_row = pImage;
    for (y = 0; y < image_info.m_MCUHeight; y += 8) {
        const int by_limit = jpg_min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));
        for (x = 0; x < image_info.m_MCUWidth; x += 8) {
            uint16_t *pDst_block = pDst_row + x;
            // Compute source byte offset of the block in the decoder's MCU buffer.
            uint src_ofs = (x * 8U) + (y * 16U);
            const uint8_t *pSrcR = image_info.m_pMCUBufR + src_ofs;
            const uint8_t *pSrcG = image_info.m_pMCUBufG + src_ofs;
            const uint8_t *pSrcB = image_info.m_pMCUBufB + src_ofs;
            const int bx_limit = jpg_min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));
            if (image_info.m_scanType == PJPG_GRAYSCALE) {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;
                    for (bx = 0; bx < bx_limit; bx++) {
                        *pDst++ = (*pSrcR & 0xF8) << 8 | (*pSrcR & 0xFC) <<3 | *pSrcR >> 3;
                        pSrcR++;
                    }
                    pSrcR += (8 - bx_limit);
                    pDst_block += row_pitch;
                }
            }
            else {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++) {
                        *pDst++ = (*pSrcR & 0xF8) << 8 | (*pSrcG & 0xFC) <<3 | *pSrcB >> 3;
                        pSrcR++; pSrcG++; pSrcB++;
                    }
                    pSrcR += (8 - bx_limit);
                    pSrcG += (8 - bx_limit);
                    pSrcB += (8 - bx_limit);

                    pDst_block += row_pitch;
                }
            }
        }
        pDst_row += (row_pitch * 8);
    }
    MCUx = mcu_x;
    MCUy = mcu_y;

    mcu_x++;
    if (mcu_x == image_info.m_MCUSPerRow) {
        mcu_x = 0;
        mcu_y++;
    }
    if(decode_mcu()==-1) is_available = 0 ;
    return 1;
}

int JPEGDecoder::readSwappedBytes(void) {
    int y, x;
    uint16_t *pDst_row;

    if(is_available == 0 || mcu_y >= image_info.m_MCUSPerCol) {
        abort();
        return 0;
    }
    // Copy MCU's pixel blocks into the destination bitmap.
    pDst_row = pImage;
    for (y = 0; y < image_info.m_MCUHeight; y += 8) {
        const int by_limit = jpg_min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

        for (x = 0; x < image_info.m_MCUWidth; x += 8) {
            uint16_t *pDst_block = pDst_row + x;
            // Compute source byte offset of the block in the decoder's MCU buffer.
            uint src_ofs = (x * 8U) + (y * 16U);
            const uint8_t *pSrcR = image_info.m_pMCUBufR + src_ofs;
            const uint8_t *pSrcG = image_info.m_pMCUBufG + src_ofs;
            const uint8_t *pSrcB = image_info.m_pMCUBufB + src_ofs;
            const int bx_limit = jpg_min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

            if (image_info.m_scanType == PJPG_GRAYSCALE) {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;
                    for (bx = 0; bx < bx_limit; bx++) {
                        *pDst++ = (*pSrcR & 0xF8) | (*pSrcR & 0xE0) >> 5 | (*pSrcR & 0xF8) << 5 | (*pSrcR & 0x1C) << 11;
                        pSrcR++;
                    }
                }
            }
            else {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++) {
                        *pDst++ = (*pSrcR & 0xF8) | (*pSrcG & 0xE0) >> 5 | (*pSrcB & 0xF8) << 5 | (*pSrcG & 0x1C) << 11;
                        pSrcR++; pSrcG++; pSrcB++;
                    }
                    pSrcR += (8 - bx_limit);
                    pSrcG += (8 - bx_limit);
                    pSrcB += (8 - bx_limit);
                    pDst_block += row_pitch;
                }
            }
        }
        pDst_row += (row_pitch * 8);
    }
    MCUx = mcu_x;
    MCUy = mcu_y;

    mcu_x++;
    if (mcu_x == image_info.m_MCUSPerRow) {
        mcu_x = 0;
        mcu_y++;
    }
    if(decode_mcu()==-1) is_available = 0 ;
    return 1;
}

int JPEGDecoder::decodeSdFile(File jpgFile) { // This is for the SD library
    g_pInFileSd = jpgFile;
    jpg_source = JPEG_SD_FILE; // Flag to indicate a SD file

    if (!g_pInFileSd) {
        return -1;
    }
    g_nInFileOfs = 0;
    g_nInFileSize = g_pInFileSd.size();

    return decodeCommon();
}

int JPEGDecoder::decodeArray(const uint8_t array[], uint32_t  array_size) {
    jpg_source = JPEG_ARRAY; // We are not processing a file, use arrays
    g_nInFileOfs = 0;
    jpg_data = (uint8_t *)array;
    g_nInFileSize = array_size;
    return decodeCommon();
}

int JPEGDecoder::decodeCommon(void) {
    status = JpegDec.pjpeg_decode_init(&image_info, pjpeg_callback, NULL, 0);
    if (status) {
        log_e("pjpeg_decode_init() failed with status %i", status);
        if (status == JpegDec.PJPG_UNSUPPORTED_MODE) {
            log_e("Progressive JPEG files are not supported.");
        }
        return -1;
    }
    decoded_width =  image_info.m_width;
    decoded_height =  image_info.m_height;
    row_pitch = image_info.m_MCUWidth;
    pImage = new uint16_t[image_info.m_MCUWidth * image_info.m_MCUHeight];
    if (!pImage) {
        log_e("Memory Allocation Failure");
        return -1;
    }
//  memset(pImage , 0 , sizeof(*pImage));

    row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
    col_blocks_per_mcu = image_info.m_MCUHeight >> 3;
    is_available = 1 ;
    width = decoded_width;
    height = decoded_height;
    comps = 1;
    MCUSPerRow = image_info.m_MCUSPerRow;
    MCUSPerCol = image_info.m_MCUSPerCol;
    scanType = image_info.m_scanType;
    MCUWidth = image_info.m_MCUWidth;
    MCUHeight = image_info.m_MCUHeight;
    return decode_mcu();
}

void JPEGDecoder::abort(void) {
    mcu_x = 0 ;
    mcu_y = 0 ;
    is_available = 0;
    if(pImage) delete pImage;
    pImage = NULL;
    if (jpg_source == JPEG_SD_FILE) if (g_pInFileSd) g_pInFileSd.close();
}
// JPEGDecoder picojpeg
int16_t JPEGDecoder::replicateSignBit16(int8_t n){
   switch (n){
      case 0:  return 0x0000;
      case 1:  return 0x8000;
      case 2:  return 0xC000;
      case 3:  return 0xE000;
      case 4:  return 0xF000;
      case 5:  return 0xF800;
      case 6:  return 0xFC00;
      case 7:  return 0xFE00;
      case 8:  return 0xFF00;
      case 9:  return 0xFF80;
      case 10: return 0xFFC0;
      case 11: return 0xFFE0;
      case 12: return 0xFFF0;
      case 13: return 0xFFF8;
      case 14: return 0xFFFC;
      case 15: return 0xFFFE;
      default: return 0xFFFF;
   }
}

//------------------------------------------------------------------------------
void JPEGDecoder::fillInBuf(void){
   unsigned char status;
   // Reserve a few bytes at the beginning of the buffer for putting back ("stuffing") chars.
   gInBufOfs = 4;
   gInBufLeft = 0;
   status = (*g_pNeedBytesCallback)(gInBuf + gInBufOfs, PJPG_MAX_IN_BUF_SIZE - gInBufOfs, &gInBufLeft, g_pCallback_data);
   if (status){
      // The user provided need bytes callback has indicated an error, so record the error and continue trying to decode.
      // The highest level pjpeg entrypoints will catch the error and return the non-zero status.
      gCallbackStatus = status;
   }
}
//------------------------------------------------------------------------------

uint16_t JPEGDecoder::getBits(uint8_t numBits, uint8_t FFCheck){
   uint8_t origBits = numBits;
   uint16_t ret = gBitBuf;
   if (numBits > 8)   {
      numBits -= 8;
      gBitBuf <<= gBitsLeft;
      gBitBuf |= getOctet(FFCheck);
      gBitBuf <<= (8 - gBitsLeft);
      ret = (ret & 0xFF00) | (gBitBuf >> 8);
   }
   if (gBitsLeft < numBits){
      gBitBuf <<= gBitsLeft;
      gBitBuf |= getOctet(FFCheck);
      gBitBuf <<= (numBits - gBitsLeft);
      gBitsLeft = 8 - (numBits - gBitsLeft);
   }
   else{
      gBitsLeft = (uint8_t)(gBitsLeft - numBits);
      gBitBuf <<= numBits;
   }
   return ret >> (16 - origBits);
}
//------------------------------------------------------------------------------

uint16_t JPEGDecoder::getExtendTest(uint8_t i){
   switch (i){
      case 0: return 0;
      case 1: return 0x0001;
      case 2: return 0x0002;
      case 3: return 0x0004;
      case 4: return 0x0008;
      case 5: return 0x0010;
      case 6: return 0x0020;
      case 7: return 0x0040;
      case 8:  return 0x0080;
      case 9:  return 0x0100;
      case 10: return 0x0200;
      case 11: return 0x0400;
      case 12: return 0x0800;
      case 13: return 0x1000;
      case 14: return 0x2000;
      case 15: return 0x4000;
      default: return 0;
   }
}
//------------------------------------------------------------------------------
int16_t JPEGDecoder::getExtendOffset(uint8_t i){

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-negative-value"

   switch (i){
      case 0: return 0;
      case 1: return ((-1)<<1) + 1;
      case 2: return ((-1)<<2) + 1;
      case 3: return ((-1)<<3) + 1;
      case 4: return ((-1)<<4) + 1;
      case 5: return ((-1)<<5) + 1;
      case 6: return ((-1)<<6) + 1;
      case 7: return ((-1)<<7) + 1;
      case 8: return ((-1)<<8) + 1;
      case 9: return ((-1)<<9) + 1;
      case 10: return ((-1)<<10) + 1;
      case 11: return ((-1)<<11) + 1;
      case 12: return ((-1)<<12) + 1;
      case 13: return ((-1)<<13) + 1;
      case 14: return ((-1)<<14) + 1;
      case 15: return ((-1)<<15) + 1;
      default: return 0;
   }

#pragma GCC diagnostic pop

}
//------------------------------------------------------------------------------

void JPEGDecoder::huffCreate(const uint8_t* pBits, HuffTable* pHuffTable){
   uint8_t  i = 0, j=0;
   uint16_t code = 0;

   for ( ; ; ){
       uint8_t num = pBits[i];
       if (!num){
           pHuffTable->mMinCode[i] = 0x0000;
           pHuffTable->mMaxCode[i] = 0xFFFF;
           pHuffTable->mValPtr[i] = 0;
       }
       else{
           pHuffTable->mMinCode[i] = code;
           pHuffTable->mMaxCode[i] = code + num - 1;
           pHuffTable->mValPtr[i] = j;
           j = (uint8_t)(j + num);
           code = (uint16_t)(code + num);
      }
      code <<= 1;
      i++;
      if (i > 15) break;
   }
}
//------------------------------------------------------------------------------
JPEGDecoder::HuffTable* JPEGDecoder::getHuffTable(uint8_t index){
    // 0-1 = DC
    // 2-3 = AC
    switch (index){
        case 0: return &gHuffTab0;
        case 1: return &gHuffTab1;
        case 2: return &gHuffTab2;
        case 3: return &gHuffTab3;
        default: return 0;
    }
}
//------------------------------------------------------------------------------
uint8_t* JPEGDecoder::getHuffVal(uint8_t index){
    // 0-1 = DC
    // 2-3 = AC
    switch (index){
        case 0: return gHuffVal0;
        case 1: return gHuffVal1;
        case 2: return gHuffVal2;
        case 3: return gHuffVal3;
        default: return 0;
    }
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::readDHTMarker(void){
    uint8_t bits[16];
    uint16_t left = getBits1(16);
    if (left < 2) return PJPG_BAD_DHT_MARKER;
    left -= 2;
    while(left){
        uint8_t i, tableIndex, index;
        uint8_t* pHuffVal;
        HuffTable* pHuffTable;
        uint16_t count, totalRead;
        index = (uint8_t)getBits1(8);

        if(((index & 0xF) > 1) || ((index & 0xF0) > 0x10)) return PJPG_BAD_DHT_INDEX;

        tableIndex = ((index >> 3) & 2) + (index & 1);
        pHuffTable = getHuffTable(tableIndex);
        pHuffVal = getHuffVal(tableIndex);
        gValidHuffTables |= (1 << tableIndex);
        count = 0;
        for(i = 0; i <= 15; i++){
            uint8_t n = (uint8_t)getBits1(8);
            bits[i] = n;
            count = (uint16_t)(count + n);
        }
        if(count > getMaxHuffCodes(tableIndex)) return PJPG_BAD_DHT_COUNTS;
        for(i = 0; i < count; i++) pHuffVal[i] = (uint8_t)getBits1(8);
        totalRead = 1 + 16 + count;
        if (left < totalRead) return PJPG_BAD_DHT_MARKER;
        left = (uint16_t)(left - totalRead);
        huffCreate(bits, pHuffTable);
    }
    return 0;
}
//------------------------------------------------------------------------------
//void JPEGDecoder::createWinogradQuant(int16_t* pQuant);
uint8_t JPEGDecoder::readDQTMarker(void){
    uint16_t left = getBits1(16);
    if (left < 2) return PJPG_BAD_DQT_MARKER;
    left -= 2;
    while (left){
        uint8_t i;
        uint8_t n = (uint8_t)getBits1(8);
        uint8_t prec = n >> 4;
        uint16_t totalRead;

        n &= 0x0F;
        if (n > 1) return PJPG_BAD_DQT_TABLE;
        gValidQuantTables |= (n ? 2 : 1);
        // read quantization entries, in zag order
        for (i = 0; i < 64; i++){
            uint16_t temp = getBits1(8);
            if(prec) temp = (temp << 8) + getBits1(8);
            if (n) gQuant1[i] = (int16_t)temp;
        else
            gQuant0[i] = (int16_t)temp;
        }
        createWinogradQuant(n ? gQuant1 : gQuant0);
        totalRead = 64 + 1;
        if(prec) totalRead += 64;
        if(left < totalRead) return PJPG_BAD_DQT_LENGTH;
        left = (uint16_t)(left - totalRead);
    }
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::readSOFMarker(void){
    uint8_t i;
    uint16_t left = getBits1(16);

    if (getBits1(8) != 8) return PJPG_BAD_PRECISION;
    gImageYSize = getBits1(16);
    if((!gImageYSize) || (gImageYSize > PJPG_MAX_HEIGHT)) return PJPG_BAD_HEIGHT;
    gImageXSize = getBits1(16);
    if((!gImageXSize) || (gImageXSize > PJPG_MAX_WIDTH)) return PJPG_BAD_WIDTH;
    gCompsInFrame = (uint8_t)getBits1(8);
    if(gCompsInFrame > 3) return PJPG_TOO_MANY_COMPONENTS;
    if (left != (gCompsInFrame + gCompsInFrame + gCompsInFrame + 8)) return PJPG_BAD_SOF_LENGTH;
    for (i = 0; i < gCompsInFrame; i++){
        gCompIdent[i] = (uint8_t)getBits1(8);
        gCompHSamp[i] = (uint8_t)getBits1(4);
        gCompVSamp[i] = (uint8_t)getBits1(4);
        gCompQuant[i] = (uint8_t)getBits1(8);
        if(gCompQuant[i] > 1) return PJPG_UNSUPPORTED_QUANT_TABLE;
    }
    return 0;
}
//------------------------------------------------------------------------------
// Used to skip unrecognized markers.
uint8_t JPEGDecoder::skipVariableMarker(void){
    uint16_t left = getBits1(16);
    if (left < 2) return PJPG_BAD_VARIABLE_MARKER;
    left -= 2;
    while(left){getBits1(8); left--;}
    return 0;
}
//------------------------------------------------------------------------------
// Read a define restart interval (DRI) marker.
uint8_t JPEGDecoder::readDRIMarker(void){
    if (getBits1(16) != 4) return PJPG_BAD_DRI_LENGTH;
    gRestartInterval = getBits1(16);
    return 0;
}
//------------------------------------------------------------------------------
// Read a start of scan (SOS) marker.
uint8_t JPEGDecoder::readSOSMarker(void){
    uint8_t i;
    uint16_t left = getBits1(16);
    uint8_t successive_high __attribute__((unused));
    uint8_t successive_low  __attribute__((unused));
    uint8_t spectral_end    __attribute__((unused));
    uint8_t spectral_start  __attribute__((unused));

    gCompsInScan = (uint8_t)getBits1(8);
    left -= 3;
    if((left!=(gCompsInScan+gCompsInScan+3))||(gCompsInScan<1)||(gCompsInScan>PJPG_MAXCOMPSINSCAN)) return PJPG_BAD_SOS_LENGTH;
    for (i = 0; i < gCompsInScan; i++){
        uint8_t cc = (uint8_t)getBits1(8);
        uint8_t c  = (uint8_t)getBits1(8);
        uint8_t ci;
        left -= 2;
        for(ci = 0; ci < gCompsInFrame; ci++) if(cc == gCompIdent[ci]) break;
        if(ci >= gCompsInFrame) return PJPG_BAD_SOS_COMP_ID;
        gCompList[i]   = ci;
        gCompDCTab[ci] = (c >> 4) & 15;
        gCompACTab[ci] = (c & 15);
    }
    spectral_start  = (uint8_t)getBits1(8);
    spectral_end    = (uint8_t)getBits1(8);
    successive_high = (uint8_t)getBits1(4);
    successive_low  = (uint8_t)getBits1(4);
    left -= 3;
    while(left){getBits1(8); left--;}
    return 0;

}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::nextMarker(void){
   uint8_t c;
   uint8_t bytes = 0;

   do{
       do{
           bytes++;
           c = (uint8_t)getBits1(8);
       } while (c != 0xFF);
       do{
           c = (uint8_t)getBits1(8);
       } while (c == 0xFF);
   } while (c == 0);

   // If bytes > 0 here, there where extra bytes before the marker (not good).
   return c;
}
//------------------------------------------------------------------------------
// Process markers. Returns when an SOFx, SOI, EOI, or SOS marker is
// encountered.
uint8_t JPEGDecoder::processMarkers(uint8_t* pMarker){
    for( ; ; ){
        uint8_t c = nextMarker();

        switch (c){
         case M_SOF0:
         case M_SOF1:
         case M_SOF2:
         case M_SOF3:
         case M_SOF5:
         case M_SOF6:
         case M_SOF7:
         // case M_JPG:
         case M_SOF9:
         case M_SOF10:
         case M_SOF11:
         case M_SOF13:
         case M_SOF14:
         case M_SOF15:
         case M_SOI:
         case M_EOI:
         case M_SOS:{*pMarker = c; return 0;}
         case M_DHT:{readDHTMarker(); break;}
         // Sorry, no arithmetic support at this time. Dumb patents!
         case M_DAC:{return PJPG_NO_ARITHMITIC_SUPPORT;}
         case M_DQT:{readDQTMarker(); break;}
         case M_DRI:{readDRIMarker(); break;}
         //case M_APP0:  /* no need to read the JFIF marker */
         case M_JPG:
         case M_RST0:    /* no parameters */
         case M_RST1:
         case M_RST2:
         case M_RST3:
         case M_RST4:
         case M_RST5:
         case M_RST6:
         case M_RST7:
         case M_TEM:{return PJPG_UNEXPECTED_MARKER;}
         default:    /* must be DNL, DHP, EXP, APPn, JPGn, COM, or RESn or APP0 */
         {
            skipVariableMarker();
            break;
         }
      }
   }
   return 0;
}
//------------------------------------------------------------------------------
// Finds the start of image (SOI) marker.
uint8_t JPEGDecoder::locateSOIMarker(void){
   uint16_t bytesleft;
   uint8_t lastchar = (uint8_t)getBits1(8);
   uint8_t thischar = (uint8_t)getBits1(8);
   /* ok if it's a normal JPEG file without a special header */
   if ((lastchar == 0xFF) && (thischar == M_SOI)) return 0;
   bytesleft = 4096; //512;
   for( ; ; ){
       if (--bytesleft == 0) return PJPG_NOT_JPEG;
       lastchar = thischar;
       thischar = (uint8_t)getBits1(8);
       if(lastchar == 0xFF){
           if(thischar == M_SOI) break;
           else if (thischar == M_EOI) return PJPG_NOT_JPEG; //getBits1 will keep returning M_EOI if we read past the end
       }
   }
   /* Check the next character after marker: if it's not 0xFF, it can't
   be the start of the next marker, so the file is bad */
   thischar = (uint8_t)((gBitBuf >> 8) & 0xFF);
   if (thischar != 0xFF) return PJPG_NOT_JPEG;
   return 0;
}
//------------------------------------------------------------------------------
// Find a start of frame (SOF) marker.
uint8_t JPEGDecoder::locateSOFMarker(void){
    uint8_t c;
    uint8_t status = locateSOIMarker();
    if(status) return status;
    status = processMarkers(&c);
    if(status) return status;
    switch(c){
       case M_SOF2:{
          // Progressive JPEG - not supported by JPEGDecoder (would require too
          // much memory, or too many IDCT's for embedded systems).
          return PJPG_UNSUPPORTED_MODE;
       }
       case M_SOF0:{  /* baseline DCT */
          status = readSOFMarker();
          if (status) return status;
          break;
       }
       case M_SOF9:{return PJPG_NO_ARITHMITIC_SUPPORT;}
       case M_SOF1:  /* extended sequential DCT */
       default:{
           return PJPG_UNSUPPORTED_MARKER;
       }
    }
    return 0;
}
//------------------------------------------------------------------------------
// Find a start of scan (SOS) marker.
uint8_t JPEGDecoder::locateSOSMarker(uint8_t* pFoundEOI){
    uint8_t c;
    uint8_t status;

    *pFoundEOI = 0;
    status = processMarkers(&c);
    if(status) return status;
    if(c == M_EOI){
        *pFoundEOI = 1;
        return 0;
    }
    else if (c != M_SOS) return PJPG_UNEXPECTED_MARKER;
    return readSOSMarker();
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::init(void){
    gImageXSize = 0;
    gImageYSize = 0;
    gCompsInFrame = 0;
    gRestartInterval = 0;
    gCompsInScan = 0;
    gValidHuffTables = 0;
    gValidQuantTables = 0;
    gTemFlag = 0;
    gInBufOfs = 0;
    gInBufLeft = 0;
    gBitBuf = 0;
    gBitsLeft = 8;

    getBits1(8);
    getBits1(8);

    return 0;
}
//------------------------------------------------------------------------------
// This method throws back into the stream any bytes that where read
// into the bit buffer during initial marker scanning.
void JPEGDecoder::fixInBuffer(void){
    /* In case any 0xFF's where pulled into the buffer during marker scanning */
    if(gBitsLeft > 0) stuffChar((uint8_t)gBitBuf);
    stuffChar((uint8_t)(gBitBuf >> 8));
    gBitsLeft = 8;
    getBits2(8);
    getBits2(8);
}
//------------------------------------------------------------------------------
// Restart interval processing.
uint8_t JPEGDecoder::processRestart(void){
    // Let's scan a little bit to find the marker, but not _too_ far.
    // 1536 is a "fudge factor" that determines how much to scan.
    uint16_t i;
    uint8_t c = 0;
    for(i = 1536; i > 0; i--) if (getChar() == 0xFF) break;
    if(i==0) return PJPG_BAD_RESTART_MARKER;
    for( ; i > 0; i--) if ((c = getChar()) != 0xFF) break;
    if(i==0) return PJPG_BAD_RESTART_MARKER;
    // Is it the expected marker? If not, something bad happened.
    if(c != (gNextRestartNum + M_RST0)) return PJPG_BAD_RESTART_MARKER;
    // Reset each component's DC prediction values.
    gLastDC[0] = 0;
    gLastDC[1] = 0;
    gLastDC[2] = 0;
    gRestartsLeft = gRestartInterval;
    gNextRestartNum = (gNextRestartNum + 1) & 7;
    // Get the bit buffer going again
    gBitsLeft = 8;
    getBits2(8);
    getBits2(8);
    return 0;
}
//------------------------------------------------------------------------------
// FIXME: findEOI() is not actually called at the end of the image
// (it's optional, and probably not needed on embedded devices)
uint8_t JPEGDecoder::findEOI(void){
    uint8_t c;
    uint8_t status;
    // Prime the bit buffer
    gBitsLeft = 8;
    getBits1(8);
    getBits1(8);

    // The next marker _should_ be EOI
    status = processMarkers(&c);
    if(status) return status;
    else if(gCallbackStatus) return gCallbackStatus;
    //gTotalBytesRead -= in_buf_left;
    if(c!=M_EOI) return PJPG_UNEXPECTED_MARKER;
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::checkHuffTables(void){
    uint8_t i;
    for(i = 0; i < gCompsInScan; i++){
        uint8_t compDCTab = gCompDCTab[gCompList[i]];
        uint8_t compACTab = gCompACTab[gCompList[i]] + 2;
        if(((gValidHuffTables & (1 << compDCTab)) == 0)||((gValidHuffTables & (1 << compACTab)) == 0))
            return PJPG_UNDEFINED_HUFF_TABLE;
    }
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::checkQuantTables(void){
    uint8_t i;
    for(i=0; i<gCompsInScan; i++){
      uint8_t compQuantMask = gCompQuant[gCompList[i]] ? 2 : 1;
      if((gValidQuantTables & compQuantMask)==0) return PJPG_UNDEFINED_QUANT_TABLE;
    }
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::initScan(void){
    uint8_t foundEOI;
    uint8_t status = locateSOSMarker(&foundEOI);
    if(status) return status;
    if(foundEOI) return PJPG_UNEXPECTED_MARKER;
    status=checkHuffTables();
    if(status) return status;
    status=checkQuantTables();
    if(status) return status;
    gLastDC[0] = 0;
    gLastDC[1] = 0;
    gLastDC[2] = 0;
    if(gRestartInterval){
        gRestartsLeft = gRestartInterval;
        gNextRestartNum = 0;
    }
    fixInBuffer();
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::initFrame(void){
    if (gCompsInFrame==1){
        if((gCompHSamp[0]!=1)||(gCompVSamp[0]!=1)) return PJPG_UNSUPPORTED_SAMP_FACTORS;
        gScanType=PJPG_GRAYSCALE;
        gMaxBlocksPerMCU = 1;
        gMCUOrg[0] = 0;
        gMaxMCUXSize     = 8;
        gMaxMCUYSize     = 8;
    }
    else if(gCompsInFrame==3){
        if(((gCompHSamp[1]!=1)||(gCompVSamp[1]!=1))||((gCompHSamp[2]!=1)||(gCompVSamp[2]!=1)))
            return PJPG_UNSUPPORTED_SAMP_FACTORS;
        if((gCompHSamp[0]==1)&&(gCompVSamp[0]==1)){
            gScanType = PJPG_YH1V1;
            gMaxBlocksPerMCU = 3;
            gMCUOrg[0] = 0;
            gMCUOrg[1] = 1;
            gMCUOrg[2] = 2;
            gMaxMCUXSize = 8;
            gMaxMCUYSize = 8;
        }
        else if((gCompHSamp[0]==1)&&(gCompVSamp[0]==2)){
            gScanType = PJPG_YH1V2;
            gMaxBlocksPerMCU = 4;
            gMCUOrg[0] = 0;
            gMCUOrg[1] = 0;
            gMCUOrg[2] = 1;
            gMCUOrg[3] = 2;
            gMaxMCUXSize = 8;
            gMaxMCUYSize = 16;
        }
        else if((gCompHSamp[0]==2)&&(gCompVSamp[0]==1)){
            gScanType = PJPG_YH2V1;
            gMaxBlocksPerMCU = 4;
            gMCUOrg[0] = 0;
            gMCUOrg[1] = 0;
            gMCUOrg[2] = 1;
            gMCUOrg[3] = 2;
            gMaxMCUXSize = 16;
            gMaxMCUYSize = 8;
        }
        else if((gCompHSamp[0]==2)&&(gCompVSamp[0]==2)){
            gScanType = PJPG_YH2V2;
            gMaxBlocksPerMCU = 6;
            gMCUOrg[0] = 0;
            gMCUOrg[1] = 0;
            gMCUOrg[2] = 0;
            gMCUOrg[3] = 0;
            gMCUOrg[4] = 1;
            gMCUOrg[5] = 2;
            gMaxMCUXSize = 16;
            gMaxMCUYSize = 16;
        }
        else return PJPG_UNSUPPORTED_SAMP_FACTORS;
    }
    else return PJPG_UNSUPPORTED_COLORSPACE;
    gMaxMCUSPerRow = (gImageXSize + (gMaxMCUXSize - 1)) >> ((gMaxMCUXSize == 8) ? 3 : 4);
    gMaxMCUSPerCol = (gImageYSize + (gMaxMCUYSize - 1)) >> ((gMaxMCUYSize == 8) ? 3 : 4);
    gNumMCUSRemaining = gMaxMCUSPerRow * gMaxMCUSPerCol;
    return 0;
}
//----------------------------------------------------------------------------
// Winograd IDCT: 5 multiplies per row/col, up to 80 muls for the 2D IDCT
#define PJPG_DCT_SCALE (1U << PJPG_DCT_SCALE_BITS)
//#define PJPG_DESCALE(x) PJPG_ARITH_SHIFT_RIGHT_N_16(((x) + (1U << (PJPG_DCT_SCALE_BITS - 1))), PJPG_DCT_SCALE_BITS)
#define PJPG_WFIX(x) ((x) * PJPG_DCT_SCALE + 0.5f)

// Multiply quantization matrix by the Winograd IDCT scale factors
void JPEGDecoder::createWinogradQuant(int16_t* pQuant){
    uint8_t i;
    for(i=0; i<64; i++){
        long x = pQuant[i];
        x *= gWinogradQuant[i];
        pQuant[i]=(int16_t)((x+(1<<(PJPG_WINOGRAD_QUANT_SCALE_BITS-PJPG_DCT_SCALE_BITS-1)))>>(PJPG_WINOGRAD_QUANT_SCALE_BITS-PJPG_DCT_SCALE_BITS));
    }
}

void JPEGDecoder::idctRows(void){
    uint8_t i;
    int16_t* pSrc = gCoeffBuf;
    for(i=0; i<8; i++){
        if((pSrc[1] | pSrc[2] | pSrc[3] | pSrc[4] | pSrc[5] | pSrc[6] | pSrc[7]) == 0){
            // Short circuit the 1D IDCT if only the DC component is non-zero
            int16_t src0 = *pSrc;
            *(pSrc+1) = src0;
            *(pSrc+2) = src0;
            *(pSrc+3) = src0;
            *(pSrc+4) = src0;
            *(pSrc+5) = src0;
            *(pSrc+6) = src0;
            *(pSrc+7) = src0;
        }
        else{
            int16_t src4 = *(pSrc+5);
            int16_t src7 = *(pSrc+3);
            int16_t x4  = src4 - src7;
            int16_t x7  = src4 + src7;
            int16_t src5 = *(pSrc+1);
            int16_t src6 = *(pSrc+7);
            int16_t x5  = src5 + src6;
            int16_t x6  = src5 - src6;
            int16_t tmp1 = imul_b5(x4 - x6);
            int16_t stg26 = imul_b4(x6) - tmp1;
            int16_t x24 = tmp1 - imul_b2(x4);
            int16_t x15 = x5 - x7;
            int16_t x17 = x5 + x7;
            int16_t tmp2 = stg26 - x17;
            int16_t tmp3 = imul_b1_b3(x15) - tmp2;
            int16_t x44 = tmp3 + x24;
            int16_t src0 = *(pSrc+0);
            int16_t src1 = *(pSrc+4);
            int16_t x30 = src0 + src1;
            int16_t x31 = src0 - src1;
            int16_t src2 = *(pSrc+2);
            int16_t src3 = *(pSrc+6);
            int16_t x12 = src2 - src3;
            int16_t x13 = src2 + src3;
            int16_t x32 = imul_b1_b3(x12) - x13;
            int16_t x40 = x30 + x13;
            int16_t x43 = x30 - x13;
            int16_t x41 = x31 + x32;
            int16_t x42 = x31 - x32;

            *(pSrc+0) = x40 + x17;
            *(pSrc+1) = x41 + tmp2;
            *(pSrc+2) = x42 + tmp3;
            *(pSrc+3) = x43 - x44;
            *(pSrc+4) = x43 + x44;
            *(pSrc+5) = x42 - tmp3;
            *(pSrc+6) = x41 - tmp2;
            *(pSrc+7) = x40 - x17;
        }
        pSrc += 8;
    }
}

void JPEGDecoder::idctCols(void){
    uint8_t i;
    int16_t* pSrc = gCoeffBuf;
    for(i=0; i<8; i++){
        if((pSrc[1*8] | pSrc[2*8] | pSrc[3*8] | pSrc[4*8] | pSrc[5*8] | pSrc[6*8] | pSrc[7*8]) == 0){
            // Short circuit the 1D IDCT if only the DC component is non-zero
            uint8_t c = clamp(PJPG_DESCALE(*pSrc) + 128);
            *(pSrc+0*8) = c;
            *(pSrc+1*8) = c;
            *(pSrc+2*8) = c;
            *(pSrc+3*8) = c;
            *(pSrc+4*8) = c;
            *(pSrc+5*8) = c;
            *(pSrc+6*8) = c;
            *(pSrc+7*8) = c;
        }
        else{
            int16_t src4 = *(pSrc+5*8);
            int16_t src7 = *(pSrc+3*8);
            int16_t x4  = src4 - src7;
            int16_t x7  = src4 + src7;
            int16_t src5 = *(pSrc+1*8);
            int16_t src6 = *(pSrc+7*8);
            int16_t x5  = src5 + src6;
            int16_t x6  = src5 - src6;
            int16_t tmp1 = imul_b5(x4 - x6);
            int16_t stg26 = imul_b4(x6) - tmp1;
            int16_t x24 = tmp1 - imul_b2(x4);
            int16_t x15 = x5 - x7;
            int16_t x17 = x5 + x7;
            int16_t tmp2 = stg26 - x17;
            int16_t tmp3 = imul_b1_b3(x15) - tmp2;
            int16_t x44 = tmp3 + x24;
            int16_t src0 = *(pSrc+0*8);
            int16_t src1 = *(pSrc+4*8);
            int16_t x30 = src0 + src1;
            int16_t x31 = src0 - src1;
            int16_t src2 = *(pSrc+2*8);
            int16_t src3 = *(pSrc+6*8);
            int16_t x12 = src2 - src3;
            int16_t x13 = src2 + src3;
            int16_t x32 = imul_b1_b3(x12) - x13;
            int16_t x40 = x30 + x13;
            int16_t x43 = x30 - x13;
            int16_t x41 = x31 + x32;
            int16_t x42 = x31 - x32;
            // descale, convert to unsigned and clamp to 8-bit
            *(pSrc+0*8) = clamp(PJPG_DESCALE(x40 + x17)  + 128);
            *(pSrc+1*8) = clamp(PJPG_DESCALE(x41 + tmp2) + 128);
            *(pSrc+2*8) = clamp(PJPG_DESCALE(x42 + tmp3) + 128);
            *(pSrc+3*8) = clamp(PJPG_DESCALE(x43 - x44)  + 128);
            *(pSrc+4*8) = clamp(PJPG_DESCALE(x43 + x44)  + 128);
            *(pSrc+5*8) = clamp(PJPG_DESCALE(x42 - tmp3) + 128);
            *(pSrc+6*8) = clamp(PJPG_DESCALE(x41 - tmp2) + 128);
            *(pSrc+7*8) = clamp(PJPG_DESCALE(x40 - x17)  + 128);
        }
        pSrc++;
    }
}
/*----------------------------------------------------------------------------*/
// Cb upsample and accumulate, 4x4 to 8x8
void JPEGDecoder::upsampleCb(uint8_t srcOfs, uint8_t dstOfs){
    // Cb - affects G and B
    uint8_t x, y;
    int16_t* pSrc = gCoeffBuf + srcOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    uint8_t* pDstB = gMCUBufB + dstOfs;
    for(y=0; y<4; y++){
        for(x=0; x<4; x++){
            uint8_t cb = (uint8_t)*pSrc++;
            int16_t cbG, cbB;
            cbG = ((cb * 88U) >> 8U) - 44U;
            pDstG[0] = subAndClamp(pDstG[0], cbG);
            pDstG[1] = subAndClamp(pDstG[1], cbG);
            pDstG[8] = subAndClamp(pDstG[8], cbG);
            pDstG[9] = subAndClamp(pDstG[9], cbG);
            cbB = (cb + ((cb * 198U) >> 8U)) - 227U;
            pDstB[0] = addAndClamp(pDstB[0], cbB);
            pDstB[1] = addAndClamp(pDstB[1], cbB);
            pDstB[8] = addAndClamp(pDstB[8], cbB);
            pDstB[9] = addAndClamp(pDstB[9], cbB);
            pDstG += 2;
            pDstB += 2;
        }
        pSrc = pSrc - 4 + 8;
        pDstG = pDstG - 8 + 16;
        pDstB = pDstB - 8 + 16;
    }
}
/*----------------------------------------------------------------------------*/
// Cb upsample and accumulate, 4x8 to 8x8
void JPEGDecoder::upsampleCbH(uint8_t srcOfs, uint8_t dstOfs){
    // Cb - affects G and B
    uint8_t x, y;
    int16_t* pSrc = gCoeffBuf + srcOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    uint8_t* pDstB = gMCUBufB + dstOfs;
    for(y=0; y<8; y++){
        for(x=0; x<4; x++){
            uint8_t cb = (uint8_t)*pSrc++;
            int16_t cbG, cbB;
            cbG = ((cb * 88U) >> 8U) - 44U;
            pDstG[0] = subAndClamp(pDstG[0], cbG);
            pDstG[1] = subAndClamp(pDstG[1], cbG);
            cbB = (cb + ((cb * 198U) >> 8U)) - 227U;
            pDstB[0] = addAndClamp(pDstB[0], cbB);
            pDstB[1] = addAndClamp(pDstB[1], cbB);
            pDstG += 2;
            pDstB += 2;
        }
        pSrc = pSrc - 4 + 8;
    }
}
/*----------------------------------------------------------------------------*/
// Cb upsample and accumulate, 8x4 to 8x8
void JPEGDecoder::upsampleCbV(uint8_t srcOfs, uint8_t dstOfs){
    // Cb - affects G and B
    uint8_t x, y;
    int16_t* pSrc = gCoeffBuf + srcOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    uint8_t* pDstB = gMCUBufB + dstOfs;
    for(y=0; y<4; y++){
        for(x=0; x<8; x++){
            uint8_t cb = (uint8_t)*pSrc++;
            int16_t cbG, cbB;
            cbG = ((cb * 88U) >> 8U) - 44U;
            pDstG[0] = subAndClamp(pDstG[0], cbG);
            pDstG[8] = subAndClamp(pDstG[8], cbG);
            cbB = (cb + ((cb * 198U) >> 8U)) - 227U;
            pDstB[0] = addAndClamp(pDstB[0], cbB);
            pDstB[8] = addAndClamp(pDstB[8], cbB);
            ++pDstG;
            ++pDstB;
        }
        pDstG = pDstG - 8 + 16;
        pDstB = pDstB - 8 + 16;
    }
}
/*----------------------------------------------------------------------------*/
// Cr upsample and accumulate, 4x4 to 8x8
void JPEGDecoder::upsampleCr(uint8_t srcOfs, uint8_t dstOfs){
    // Cr - affects R and G
    uint8_t x, y;
    int16_t* pSrc = gCoeffBuf + srcOfs;
    uint8_t* pDstR = gMCUBufR + dstOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    for(y=0; y<4; y++){
        for(x=0; x<4; x++){
            uint8_t cr = (uint8_t)*pSrc++;
            int16_t crR, crG;
            crR = (cr + ((cr * 103U) >> 8U)) - 179;
            pDstR[0] = addAndClamp(pDstR[0], crR);
            pDstR[1] = addAndClamp(pDstR[1], crR);
            pDstR[8] = addAndClamp(pDstR[8], crR);
            pDstR[9] = addAndClamp(pDstR[9], crR);
            crG = ((cr * 183U) >> 8U) - 91;
            pDstG[0] = subAndClamp(pDstG[0], crG);
            pDstG[1] = subAndClamp(pDstG[1], crG);
            pDstG[8] = subAndClamp(pDstG[8], crG);
            pDstG[9] = subAndClamp(pDstG[9], crG);
            pDstR += 2;
            pDstG += 2;
        }
        pSrc = pSrc - 4 + 8;
        pDstR = pDstR - 8 + 16;
        pDstG = pDstG - 8 + 16;
    }
}
/*----------------------------------------------------------------------------*/
// Cr upsample and accumulate, 4x8 to 8x8
void JPEGDecoder::upsampleCrH(uint8_t srcOfs, uint8_t dstOfs){
    // Cr - affects R and G
    uint8_t x, y;
    int16_t* pSrc = gCoeffBuf + srcOfs;
    uint8_t* pDstR = gMCUBufR + dstOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    for(y=0; y<8; y++){
        for(x=0; x<4; x++){
            uint8_t cr = (uint8_t)*pSrc++;
            int16_t crR, crG;
            crR = (cr + ((cr * 103U) >> 8U)) - 179;
            pDstR[0] = addAndClamp(pDstR[0], crR);
            pDstR[1] = addAndClamp(pDstR[1], crR);
            crG = ((cr * 183U) >> 8U) - 91;
            pDstG[0] = subAndClamp(pDstG[0], crG);
            pDstG[1] = subAndClamp(pDstG[1], crG);
            pDstR += 2;
            pDstG += 2;
        }
        pSrc = pSrc - 4 + 8;
    }
}
/*----------------------------------------------------------------------------*/
// Cr upsample and accumulate, 8x4 to 8x8
void JPEGDecoder::upsampleCrV(uint8_t srcOfs, uint8_t dstOfs){
    // Cr - affects R and G
    uint8_t x, y;
    int16_t* pSrc = gCoeffBuf + srcOfs;
    uint8_t* pDstR = gMCUBufR + dstOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    for(y=0; y<4; y++){
        for(x=0; x<8; x++){
            uint8_t cr = (uint8_t)*pSrc++;
            int16_t crR, crG;
            crR = (cr + ((cr * 103U) >> 8U)) - 179;
            pDstR[0] = addAndClamp(pDstR[0], crR);
            pDstR[8] = addAndClamp(pDstR[8], crR);
            crG = ((cr * 183U) >> 8U) - 91;
            pDstG[0] = subAndClamp(pDstG[0], crG);
            pDstG[8] = subAndClamp(pDstG[8], crG);
            ++pDstR;
            ++pDstG;
        }
        pDstR = pDstR - 8 + 16;
        pDstG = pDstG - 8 + 16;
    }
}
/*----------------------------------------------------------------------------*/
// Convert Y to RGB
void JPEGDecoder::copyY(uint8_t dstOfs){
    uint8_t i;
    uint8_t* pRDst = gMCUBufR + dstOfs;
    uint8_t* pGDst = gMCUBufG + dstOfs;
    uint8_t* pBDst = gMCUBufB + dstOfs;
    int16_t* pSrc = gCoeffBuf;
    for(i=64; i>0; i--){
        uint8_t c = (uint8_t)*pSrc++;
        *pRDst++ = c;
        *pGDst++ = c;
        *pBDst++ = c;
    }
}
/*----------------------------------------------------------------------------*/
// Cb convert to RGB and accumulate
void JPEGDecoder::convertCb(uint8_t dstOfs){
    uint8_t i;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    uint8_t* pDstB = gMCUBufB + dstOfs;
    int16_t* pSrc = gCoeffBuf;
    for(i=64; i>0; i--){
        uint8_t cb = (uint8_t)*pSrc++;
        int16_t cbG, cbB;
        cbG = ((cb * 88U) >> 8U) - 44U;
        pDstG[0] = subAndClamp(pDstG[0], cbG);
        cbB = (cb + ((cb * 198U) >> 8U)) - 227U;
        pDstB[0] = addAndClamp(pDstB[0], cbB);
        ++pDstG;
        ++pDstB;
    }
}
/*----------------------------------------------------------------------------*/
// Cr convert to RGB and accumulate
void JPEGDecoder::convertCr(uint8_t dstOfs){
    uint8_t i;
    uint8_t* pDstR = gMCUBufR + dstOfs;
    uint8_t* pDstG = gMCUBufG + dstOfs;
    int16_t* pSrc = gCoeffBuf;
    for(i=64; i>0; i--){
        uint8_t cr = (uint8_t)*pSrc++;
        int16_t crR, crG;
        crR = (cr + ((cr * 103U) >> 8U)) - 179;
        pDstR[0] = addAndClamp(pDstR[0], crR);
        crG = ((cr * 183U) >> 8U) - 91;
        pDstG[0] = subAndClamp(pDstG[0], crG);
        ++pDstR;
        ++pDstG;
    }
}
/*----------------------------------------------------------------------------*/
void JPEGDecoder::transformBlock(uint8_t mcuBlock){
    idctRows();
    idctCols();
    switch (gScanType){
        case PJPG_GRAYSCALE:{copyY(0); break;}  // MCU size: 1, 1 block per MCU
        case PJPG_YH1V1:{                       // MCU size: 8x8, 3 blocks per MCU
                switch(mcuBlock){
                    case 0:{copyY(0); break;}
                    case 1:{convertCb(0); break;}
                    case 2:{convertCr(0); break;}
                } break;
        }
        case PJPG_YH1V2:{                       // MCU size: 8x16, 4 blocks per MCU
                switch(mcuBlock){
                    case 0:{copyY(0);   break;}
                    case 1:{copyY(128); break;}
                    case 2:{upsampleCbV(0, 0); upsampleCbV(4*8, 128); break;}
                    case 3:{upsampleCrV(0, 0); upsampleCrV(4*8, 128); break;}
                } break;
        }
        case PJPG_YH2V1:{                       // MCU size: 16x8, 4 blocks per MCU
                switch(mcuBlock){
                    case 0:{copyY(0);   break;}
                    case 1:{copyY(64);  break;}
                    case 2:{upsampleCbH(0, 0); upsampleCbH(4, 64); break;}
                    case 3:{upsampleCrH(0, 0); upsampleCrH(4, 64); break;}
                } break;
        }
        case PJPG_YH2V2:{                       // MCU size: 16x16, 6 blocks per MCU
                switch (mcuBlock){
                    case 0:{copyY(0);   break;}
                    case 1:{copyY(64);  break;}
                    case 2:{copyY(128); break;}
                    case 3:{copyY(192); break;}
                    case 4:{upsampleCb(0, 0); upsampleCb(4, 64); upsampleCb(4*8, 128); upsampleCb(4+4*8, 192); break;}
                    case 5:{upsampleCr(0, 0); upsampleCr(4, 64); upsampleCr(4*8, 128); upsampleCr(4+4*8, 192); break;}
                } break;
        }
    }
}
//------------------------------------------------------------------------------
void JPEGDecoder::transformBlockReduce(uint8_t mcuBlock){
    uint8_t c = clamp(PJPG_DESCALE(gCoeffBuf[0]) + 128);
    int16_t cbG, cbB, crR, crG;
    switch(gScanType){
        case PJPG_GRAYSCALE:{gMCUBufR[0] = c; break;}   // MCU size: 1, 1 block per MCU
        case PJPG_YH1V1:{                               // MCU size: 8x8, 3 blocks per MCU
            switch(mcuBlock){
                case 0:{gMCUBufR[0] = c; gMCUBufG[0] = c; gMCUBufB[0] = c; break;}
                case 1:{cbG = ((c * 88U) >> 8U) - 44U;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], cbG);
                        cbB = (c + ((c * 198U) >> 8U)) - 227U;
                        gMCUBufB[0] = addAndClamp(gMCUBufB[0], cbB);
                        break;}
                case 2:{crR = (c + ((c * 103U) >> 8U)) - 179;
                        gMCUBufR[0] = addAndClamp(gMCUBufR[0], crR);
                        crG = ((c * 183U) >> 8U) - 91;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], crG);
                        break;}
            } break;
        }
        case PJPG_YH1V2:{                               // MCU size: 8x16, 4 blocks per MCU
            switch(mcuBlock){
                case 0:{gMCUBufR[0] = c; gMCUBufG[0] = c; gMCUBufB[0] = c; break;}
                case 1:{gMCUBufR[128] = c; gMCUBufG[128] = c; gMCUBufB[128] = c; break;}
                case 2:{cbG = ((c * 88U) >> 8U) - 44U;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], cbG);
                        gMCUBufG[128] = subAndClamp(gMCUBufG[128], cbG);
                        cbB = (c + ((c * 198U) >> 8U)) - 227U;
                        gMCUBufB[0] = addAndClamp(gMCUBufB[0], cbB);
                        gMCUBufB[128] = addAndClamp(gMCUBufB[128], cbB);
                        break;}
                case 3:{crR = (c + ((c * 103U) >> 8U)) - 179;
                        gMCUBufR[0] = addAndClamp(gMCUBufR[0], crR);
                        gMCUBufR[128] = addAndClamp(gMCUBufR[128], crR);
                        crG = ((c * 183U) >> 8U) - 91;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], crG);
                        gMCUBufG[128] = subAndClamp(gMCUBufG[128], crG);
                        break;}
            } break;
        }
        case PJPG_YH2V1:{                               // MCU size: 16x8, 4 blocks per MCU
            switch(mcuBlock){
                case 0:{gMCUBufR[0] = c; gMCUBufG[0] = c; gMCUBufB[0] = c; break;}
                case 1:{gMCUBufR[64] = c; gMCUBufG[64] = c; gMCUBufB[64] = c; break;}
                case 2:{cbG = ((c * 88U) >> 8U) - 44U;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], cbG);
                        gMCUBufG[64] = subAndClamp(gMCUBufG[64], cbG);
                        cbB = (c + ((c * 198U) >> 8U)) - 227U;
                        gMCUBufB[0] = addAndClamp(gMCUBufB[0], cbB);
                        gMCUBufB[64] = addAndClamp(gMCUBufB[64], cbB);
                        break;}
                case 3:{crR = (c + ((c * 103U) >> 8U)) - 179;
                        gMCUBufR[0] = addAndClamp(gMCUBufR[0], crR);
                        gMCUBufR[64] = addAndClamp(gMCUBufR[64], crR);
                        crG = ((c * 183U) >> 8U) - 91;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], crG);
                        gMCUBufG[64] = subAndClamp(gMCUBufG[64], crG);
                        break;}
            } break;
        }
        case PJPG_YH2V2:{                               // MCU size: 16x16, 6 blocks per MCU
            switch(mcuBlock){
                case 0:{gMCUBufR[0] = c; gMCUBufG[0] = c; gMCUBufB[0] = c; break;}
                case 1:{gMCUBufR[64] = c; gMCUBufG[64] = c; gMCUBufB[64] = c; break;}
                case 2:{gMCUBufR[128] = c; gMCUBufG[128] = c; gMCUBufB[128] = c; break;}
                case 3:{gMCUBufR[192] = c; gMCUBufG[192] = c; gMCUBufB[192] = c; break;}
                case 4:{cbG = ((c * 88U) >> 8U) - 44U;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], cbG);
                        gMCUBufG[64] = subAndClamp(gMCUBufG[64], cbG);
                        gMCUBufG[128] = subAndClamp(gMCUBufG[128], cbG);
                        gMCUBufG[192] = subAndClamp(gMCUBufG[192], cbG);
                        cbB = (c + ((c * 198U) >> 8U)) - 227U;
                        gMCUBufB[0] = addAndClamp(gMCUBufB[0], cbB);
                        gMCUBufB[64] = addAndClamp(gMCUBufB[64], cbB);
                        gMCUBufB[128] = addAndClamp(gMCUBufB[128], cbB);
                        gMCUBufB[192] = addAndClamp(gMCUBufB[192], cbB);
                        break;}
                case 5:{crR = (c + ((c * 103U) >> 8U)) - 179;
                        gMCUBufR[0] = addAndClamp(gMCUBufR[0], crR);
                        gMCUBufR[64] = addAndClamp(gMCUBufR[64], crR);
                        gMCUBufR[128] = addAndClamp(gMCUBufR[128], crR);
                        gMCUBufR[192] = addAndClamp(gMCUBufR[192], crR);
                        crG = ((c * 183U) >> 8U) - 91;
                        gMCUBufG[0] = subAndClamp(gMCUBufG[0], crG);
                        gMCUBufG[64] = subAndClamp(gMCUBufG[64], crG);
                        gMCUBufG[128] = subAndClamp(gMCUBufG[128], crG);
                        gMCUBufG[192] = subAndClamp(gMCUBufG[192], crG);
                        break;}
            } break;
        }
    }
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::huffDecode(const HuffTable* pHuffTable, const uint8_t* pHuffVal){
     uint8_t i = 0; uint8_t j; uint16_t code = getBit();

     // This func only reads a bit at a time, which on modern CPU's is not terribly efficient.
     // But on microcontrollers without strong integer shifting support this seems like a
     // more reasonable approach.
     for ( ; ; ){
         uint16_t maxCode;
         if (i == 16) return 0;

         maxCode = pHuffTable->mMaxCode[i];
         if ((code <= maxCode) && (maxCode != 0xFFFF)) break;

         i++;
         code <<= 1;
         code |= getBit();
     }

     j = pHuffTable->mValPtr[i];
     j = (uint8_t)(j + (code - pHuffTable->mMinCode[i]));
     return pHuffVal[j];
 }
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::decodeNextMCU(void){
    uint8_t status;
    uint8_t mcuBlock;
    if(gRestartInterval){
        if(gRestartsLeft == 0){
            status = processRestart();
            if(status) return status;
        }
        gRestartsLeft--;
    }
    for(mcuBlock = 0; mcuBlock < gMaxBlocksPerMCU; mcuBlock++){
        uint8_t componentID = gMCUOrg[mcuBlock];
        uint8_t compQuant = gCompQuant[componentID];
        uint8_t compDCTab = gCompDCTab[componentID];
        uint8_t numExtraBits, compACTab, k;
        const int16_t* pQ = compQuant ? gQuant1 : gQuant0;
        uint16_t r, dc;
        uint8_t s = huffDecode(compDCTab ? &gHuffTab1 : &gHuffTab0, compDCTab ? gHuffVal1 : gHuffVal0);
        r = 0;
        numExtraBits = s & 0xF;
        if(numExtraBits) r = getBits2(numExtraBits);
        dc = huffExtend(r, s);
        dc = dc + gLastDC[componentID];
        gLastDC[componentID] = dc;
        gCoeffBuf[0] = dc * pQ[0];
        compACTab = gCompACTab[componentID];
        if(gReduce){    // Decode, but throw out the AC coefficients in reduce mode.
            for(k=1; k<64; k++){
                s=huffDecode(compACTab ? &gHuffTab3 : &gHuffTab2, compACTab ? gHuffVal3 : gHuffVal2);
                numExtraBits = s & 0xF;
                if(numExtraBits) getBits2(numExtraBits);
                r = s >> 4;
                s &= 15;
                if(s){
                    if(r){
                        if((k+r)>63) return PJPG_DECODE_ERROR;
                        k=(uint8_t)(k + r);
                    }
                }
                else{
                    if(r==15){
                        if((k+16)>64) return PJPG_DECODE_ERROR;
                        k+=(16-1); // - 1 because the loop counter is k
                    }
                    else break;
                }
            }
            transformBlockReduce(mcuBlock);
        }
        else{   // Decode and dequantize AC coefficients
            for(k=1; k<64; k++){
                uint16_t extraBits;
                s=huffDecode(compACTab ? &gHuffTab3 : &gHuffTab2, compACTab ? gHuffVal3 : gHuffVal2);
                extraBits = 0;
                numExtraBits = s & 0xF;
                if(numExtraBits) extraBits=getBits2(numExtraBits);
                r=s>>4;
                s&=15;
                if(s){
                    int16_t ac;
                    if(r){
                        if((k+r)>63) return PJPG_DECODE_ERROR;
                        while(r){
                            gCoeffBuf[ZAG[k++]] = 0;
                            r--;
                        }
                    }
                    ac=huffExtend(extraBits, s);
                    gCoeffBuf[ZAG[k]] = ac * pQ[k];
                }
                else{
                    if(r==15){
                        if((k+16)>64) return PJPG_DECODE_ERROR;
                        for(r=16; r>0; r--) gCoeffBuf[ZAG[k++]] = 0;
                        k--; // - 1 because the loop counter is k
                    }
                    else break;
                }
            }
            while(k<64) gCoeffBuf[ZAG[k++]] = 0;
            transformBlock(mcuBlock);
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::pjpeg_decode_mcu(){
    uint8_t status;
    if(gCallbackStatus) return gCallbackStatus;
    if(!gNumMCUSRemaining) return PJPG_NO_MORE_BLOCKS;
    status=decodeNextMCU();
    if((status)||(gCallbackStatus)) return gCallbackStatus ? gCallbackStatus : status;
    gNumMCUSRemaining--;
    return 0;
}
//------------------------------------------------------------------------------
uint8_t JPEGDecoder::pjpeg_decode_init(pjpeg_image_info_t* pInfo, pjpeg_need_bytes_callback_t pNeed_bytes_callback, void *pCallback_data, unsigned char reduce){
    uint8_t status;
    pInfo->m_width = 0; pInfo->m_height = 0; pInfo->m_comps = 0;
    pInfo->m_MCUSPerRow = 0; pInfo->m_MCUSPerCol = 0;
    pInfo->m_scanType = PJPG_GRAYSCALE;
    pInfo->m_MCUWidth = 0; pInfo->m_MCUHeight = 0;
    pInfo->m_pMCUBufR = (unsigned char*)0; pInfo->m_pMCUBufG = (unsigned char*)0; pInfo->m_pMCUBufB = (unsigned char*)0;
    g_pNeedBytesCallback = pNeed_bytes_callback;
    g_pCallback_data = pCallback_data;
    gCallbackStatus = 0;
    gReduce = reduce;
    status = init();
    if((status)||(gCallbackStatus)) return gCallbackStatus ? gCallbackStatus : status;
    status = locateSOFMarker();
    if((status)||(gCallbackStatus)) return gCallbackStatus ? gCallbackStatus : status;
    status = initFrame();
    if((status)||(gCallbackStatus)) return gCallbackStatus ? gCallbackStatus : status;
    status = initScan();
    if((status)||(gCallbackStatus)) return gCallbackStatus ? gCallbackStatus : status;
    pInfo->m_width = gImageXSize; pInfo->m_height = gImageYSize; pInfo->m_comps = gCompsInFrame;
    pInfo->m_scanType = gScanType;
    pInfo->m_MCUSPerRow = gMaxMCUSPerRow; pInfo->m_MCUSPerCol = gMaxMCUSPerCol;
    pInfo->m_MCUWidth = gMaxMCUXSize; pInfo->m_MCUHeight = gMaxMCUYSize;
    pInfo->m_pMCUBufR = gMCUBufR; pInfo->m_pMCUBufG = gMCUBufG; pInfo->m_pMCUBufB = gMCUBufB;
    return 0;
}

/*******************************************************************************/

  // Code für Touchpad mit XPT2046
TP::TP(uint8_t CS, uint8_t IRQ){
    TP_CS=CS;
    TP_IRQ=IRQ;
    pinMode(TP_CS, OUTPUT);
    digitalWrite(TP_CS, HIGH); // prevent blockage of the SPI bus
    pinMode(TP_IRQ, INPUT);
    TP_SPI=SPISettings(100000, MSBFIRST, SPI_MODE0); //slower speed
    xFaktor=float(Xmax-Xmin)/TFT_WIDTH;
    yFaktor=float(Ymax-Ymin)/TFT_HEIGHT;
    _rotation=0;
}

uint16_t TP::TP_Send(uint8_t set_val){
    uint16_t get_val;
    SPI.beginTransaction(TP_SPI);       // Prevent other SPI users
      digitalWrite(TP_CS, 0);
      SPI.write(set_val);
      get_val=SPI.transfer16(0);
      digitalWrite(TP_CS, 1);
    SPI.endTransaction();               // Allow other SPI users
    return get_val >> 4;
}

void TP::loop(){
    if(!digitalRead(TP_IRQ)){
        read_TP(x,y); //erste Messung auslassen
        if(f_loop){
            f_loop=false;
            if(read_TP(x, y)) {if(tp_pressed) tp_pressed(x, y);}  //zweite Messung lesen
        }
    } else {
        if(f_loop==false){if(tp_released) tp_released();}
        f_loop=true;
    }
}
void TP::setRotation(uint8_t m){
    _rotation=m;
}

bool TP::read_TP(uint16_t& x, uint16_t& y){
  uint16_t _y[3];
  uint16_t _x[3];
  uint16_t tmpxy;

  uint8_t i;
  if (digitalRead(TP_IRQ)) return false;
  for(i=0; i<3; i++){
      x = TP_Send(0xD0);  //x

      if((x<Xmin) || (x>Xmax)) return false;  //außerhalb des Displays
       x=Xmax-x;
      _x[i]=x/xFaktor;

      y=  TP_Send(0x90); //y
      if((y<Ymin) || (y>Ymax)) return false;  //außerhalb des Displays
      y=Ymax-y;
     _y[i]=y/yFaktor;

  }
  x=(_x[0]+_x[1]+_x[2])/3; // Mittelwert bilden
  y=(_y[0]+_y[1]+_y[2])/3;

  //log_i("TP X org=%i",x);
  //log_i("TP Y org=%i",y);

  if(_rotation==1){tmpxy=x; x=y;   y=TFT_WIDTH-tmpxy;  if(x>TFT_HEIGHT-1) x=0; if(y>TFT_WIDTH-1)y=0;}
  if(_rotation==2){x=TFT_WIDTH-x; y=TFT_HEIGHT-y; if(x>TFT_WIDTH-1) x=0; if(y>TFT_HEIGHT-1) y=0;}
  if(_rotation==3){tmpxy=y; y=x; x=TFT_HEIGHT-tmpxy; if(x>TFT_HEIGHT-1) x=0; if(y>TFT_WIDTH-1) y=0;}
  //log_i("TP X=%i",x);
  //log_i("TP Y=%i",y);
  return true;
}

