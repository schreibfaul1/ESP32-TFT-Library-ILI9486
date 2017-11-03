
#include "Arduino.h"
#include "tft.h"

#define TFT_MAX_PIXELS_AT_ONCE  32

void TFT::init()
{
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
    _freq = 30000000;
    _height = TFT_HEIGHT;
    _width = TFT_WIDTH;
    _rotation=0;
}

void TFT::startWrite(void){
    SPI.beginTransaction(SPISettings(_freq, MSBFIRST, SPI_MODE2));
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

	TFT_CS=CS; TFT_DC=DC;
    TFT_MOSI=MOSI; TFT_MISO=MISO; TFT_SCK=SCK;


	_width  = TFT_WIDTH;
    _height = TFT_HEIGHT;

    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_DC, LOW);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    pinMode(16, OUTPUT); digitalWrite(16, HIGH); //GPIO TP_CS
    sprintf(sbuf, "TFT_CS:%i TFT_DC:%i TFT_BL:%i", TFT_CS, TFT_DC, TFT_BL);
    sprintf(sbuf, "%s TFT_MOSI:%i TFT_MISO:%i TFT_SCK:%i\n", sbuf, TFT_MOSI, TFT_MISO, TFT_SCK);
    if(tft_info) tft_info(sbuf);
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
//    endWrite();

//    startWrite();
        setAddrWindow(x, _height - y - h ,w ,h);
        writeCommand(ILI9486_RAMWR);
    endWrite();
}

void TFT::endBitmap() {
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
  int16_t t;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    t=x0; x0=y0; y0=t;  // swap (x0, y0);
	t=x1; x1=y1; y1=t; 	//  swap(x1, y1);
  }
  if (x0 > x1) {
	t=x0; x0=x1; x1=t;  // swap(x0, x1);
	t=y0; y0=y1; y1=t;  // swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }
  startWrite();
  for (; x0<=x1; x0++) {
    if (steep) {
      writePixel(y0, x0, color);
    } else {
      writePixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
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
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
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
void TFT::drawRect(int16_t Xpos, int16_t Ypos, uint16_t Width, uint16_t Height,	uint16_t Color)
{
	startWrite();
	writeFastHLine(Xpos, Ypos, Width, Color);
	writeFastHLine(Xpos, Ypos + Height, Width, Color);
	writeFastVLine(Xpos, Ypos, Height, Color);
	writeFastVLine(Xpos + Width, Ypos, Height + 1, Color);
	endWrite();
}


void TFT::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
		uint16_t color) {
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
void TFT::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
		uint16_t color) {
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
void TFT::drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, uint16_t color) {
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
void TFT::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, int16_t delta, uint16_t color) {

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
bool TFT::setCursor(uint16_t x, uint16_t y) {
    if (x >= width()|| y >= height()) return false;
  	writeCommand(0x2A); SPI.write(x >> 8);
   	SPI.write(x & 0xFF); writeCommand(ILI9486_RAMWR); //Column Start
    writeCommand(0x2B); SPI.write(y >> 8);
    SPI.write(y & 0xFF); writeCommand(ILI9486_RAMWR); //Row Start
    _curX=x; _curY=y;
    _f_curPos=true;  //curPos is updated
    return true;
}
/*******************************************************************************/
/*******************************************************************************/

size_t TFT::writeText(const uint8_t *str, uint16_t len)      // a pointer to string
{

	static int16_t tmp_curX=0;
	static int16_t tmp_curY=0;

	if(_f_curPos==true){tmp_curX=_curX; tmp_curY=_curY; _f_curPos=false;} //new CursorValues?

	uint16_t color=_textcolor;
	int16_t Xpos=tmp_curX;
	int16_t Ypos=tmp_curY;
	int16_t Ypos0 = Ypos;
	int16_t Xpos0 = Xpos;
	boolean f_wrap=false;
	int16_t font_char;
	int16_t i,  m, n;
	uint8_t font_height, char_width,  chTemp, char_bytes, j, k, space;
	uint16_t font_index, font_offset;
	int a=0; int fi; int strw=0;
	font_height = _font[6];
	i = 0; j=0;
	startWrite();
    while(i != len) //bis zum Ende der Zeichenkette
    {
        // word wrap
        a=i+1 ;
        if(str[i] == 32){
            //log_e("str %s",&str[i]);
            strw=font_height/4; // erstes Leerzeichen
            fi=8;
            fi=fi + (str[i] - 32) * 4;
            strw=strw + _font[fi] +1;
            while((str[a] != 32) && (a < len)){
                fi=8;
                fi=fi + (str[a] - 32) * 4;
                strw=strw + _font[fi] +1;
                //log_e("Char %c Len %i",str[a],fi);
                a++;
            }
            if(_textorientation==0){
                if((Xpos + strw) >= width())f_wrap=true;
            }
            else{
                if((Ypos+strw)>=height()) f_wrap=true;
            }
        }
	    font_char = str[i]; 	//die ersten 32 ASCII-Zeichen sind nicht im Zeichensatz enthalten
	    if((str[i]==32) && (f_wrap==true)){font_char='\n'; f_wrap=false;}
	    if(font_char>=32)		//dann ist es ein druckbares Zeichen
		{
            font_char-=32;


		    font_index = 8; //Zeichenindex beginnt immer ab Position 8
			font_index = font_index + font_char * 4;
			char_width = _font[font_index];
			if(font_char==0) space=font_height/4; else space=0; //correct spacewidth is 1
			if(_textorientation==0){
				if((Xpos+char_width+space)>=width()){Xpos=tmp_curX; Ypos+=font_height; Xpos0=Xpos; Ypos0=Ypos;}
				if((Ypos+font_height)>=height()){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
			}
			else {
				if((Ypos+char_width+space)>=height()){Ypos=tmp_curY; Xpos-=font_height; Xpos0=Xpos; Ypos0=Ypos;}
				if((Xpos-font_height)<0){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
			}
			char_bytes = (char_width - 1) / 8 + 1; //Anzahl der Bytes pro Zeichen
			font_offset = _font[font_index + 2]; //MSB
			font_offset = font_offset << 8;
			font_offset = font_offset + _font[font_index + 1]; //LSB
			//ab font_offset stehen die Infos für das Zeichen
			n = 0;
			for (k = 0; k < font_height; k++) {
				for (m = 0; m < char_bytes; m++) {
					chTemp = (_font[font_offset + n]);
					n++;
					if (_textorientation == 0) {

						for (j = 0; j < 8; j++) {
							if (chTemp & 0x01)
								writePixel(Xpos, Ypos, color);
							chTemp >>= 1;

							Xpos++;

							if ((Xpos - Xpos0) == char_width) {
								Xpos = Xpos0; Ypos++; break;
							}
						}
					} else {
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
			} else {
				Xpos = Xpos0; Ypos0 = Ypos0 + char_width + 1 + space; Ypos = Ypos0;
			}
		} // end if(font_char>=0)
		else  // das ist ein Steuerzeichen
		{
			//if(str[i]==10) {  //CRLF
            if(font_char==10){
				if(_textorientation==0){
					{Xpos=_curX; Ypos+=font_height; Xpos0=Xpos; Ypos0=Ypos;}
					if((Ypos+font_height)>height()){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
				}
				else{
					{Ypos=_curY; Xpos-=font_height; Xpos0=Xpos; Ypos0=Ypos;}
					if((Ypos+font_height)>height()){tmp_curX=Xpos; tmp_curY=Ypos; endWrite(); return i;}
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


size_t TFT::write(uint8_t character) {
	/*Code to display letter when given the ASCII code for it*/
	return 0;
}
size_t TFT::write(const uint8_t *buffer, size_t size){
	if(_f_utf8==true) writeText(UTF8toASCII(buffer), size);
	else writeText(buffer, size);
	return 0;
}

const uint8_t*  TFT::UTF8toASCII(const uint8_t* str){
    uint16_t i=0, j=0;
    static uint8_t sbuf[255];
    while(str[i]!=0){
        sbuf[j]=str[i];
    	if(sbuf[j]=='|') sbuf[j]='\n';
    	if(sbuf[j]==0xC3){i++;sbuf[j]=str[i]+64;}
    	if((str[i]=='%')&&(str[i+1]=='2')&&(str[i+2]=='0')){sbuf[j]=' '; i+=2;} //%20 in blank
    	i++; j++;
    }
    sbuf[j]=0;
	return (sbuf);
}

/*******************************************************************************/

/*
 * Bitmaps
 * */


/*
 * BMP
 * */

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

/*******************************************************************************/
/*******************************************************************************/

  // Code für Touchpad mit XPT2046
TP::TP(uint8_t CS, uint8_t IRQ){
    TP_CS=CS;
    TP_IRQ=IRQ;
    pinMode(TP_CS, OUTPUT);
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
  log_i("TP X=%i",x);
  log_i("TP Y=%i",y);
  return true;
}



