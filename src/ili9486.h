/*
 *  tft.h
 *
 *  Created on: May 28,2018
 *  Updated on: Jun 03,2022
 *      Author: Wolle (schreibfaul1)
 *
 */


#pragma once

#include "Arduino.h"
#include "FS.h"
#include "SPI.h"
#include "SD.h"

//exclude all not needed fonts (because c++ indexer will be slow)
//#include "fonts/Garamond.h"  // default, do not comment this line
//#include "fonts/Baskerville_Old_Face.h"
//#include "fonts/Courier_New.h"
//#include "fonts/Monotype_Corsiva.h"
#include "fonts/misc.h"
//#include "fonts/Old_English_Text_MT.h"
//#include "fonts/Script_MT_Bold.h"
//#include "fonts/Garamond_cyrillic.h"
//#include "fonts/Garamond_greek.h"
#include "fonts/Times_New_Roman.h"      // latin, greek, cyrillic with all extensions

extern __attribute__((weak)) void tft_info(const char*);
extern __attribute__((weak)) void tp_pressed(uint16_t x, uint16_t y);
extern __attribute__((weak)) void tp_released();

#define TFT_WIDTH       320
#define TFT_HEIGHT      480

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

// RGB565 Color definitions
#define TFT_AQUAMARINE      0x7FFA // 127, 255, 212
#define TFT_BEIGE           0xF7BB // 245, 245, 220
#define TFT_BLACK           0x0000 //   0,   0,   0
#define TFT_BLUE            0x001F //   0,   0, 255
#define TFT_BROWN           0xA145 // 165,  42,  42
#define TFT_CHOCOLATE       0xD343 // 210, 105,  30
#define TFT_CORNSILK        0xFFDB // 255, 248, 220
#define TFT_CYAN            0x07FF //   0, 255, 255
#define TFT_DARKGREEN       0x0320 //   0, 100,   0
#define TFT_DARKGREY        0xAD55 // 169, 169, 169
#define TFT_DARKCYAN        0x0451 //   0, 139, 139
#define TFT_DEEPSKYBLUE     0x05FF //   0, 191, 255
#define TFT_GRAY            0x8410 // 128, 128, 128
#define TFT_GREEN           0x0400 //   0, 128,   0
#define TFT_GREENYELLOW     0xAFE5 // 173, 255,  47
#define TFT_GOLD            0xFEA0 // 255, 215,   0
#define TFT_HOTPINK         0xFB56 // 255, 105, 180
#define TFT_LAVENDER        0xE73F // 230, 230, 250
#define TFT_LAWNGREEN       0x7FE0 // 124, 252,   0
#define TFT_LIGHTBLUE       0xAEDC // 173, 216, 230
#define TFT_LIGHTCYAN       0xE7FF // 224, 255, 255
#define TFT_LIGHTGREY       0xD69A // 211, 211, 211
#define TFT_LIGHTGREEN      0x9772 // 144, 238, 144
#define TFT_LIGHTYELLOW     0xFFFC // 255, 255, 224
#define TFT_LIME            0x07E0 //   0. 255,   0
#define TFT_MAGENTA         0xF81F // 255,   0, 255
#define TFT_MAROON          0x7800 // 128,   0,   0
#define TFT_MEDIUMVIOLETRED 0xC0B0 // 199,  21, 133
#define TFT_NAVY            0x000F //   0,   0, 128
#define TFT_OLIVE           0x7BE0 // 128, 128,   0
#define TFT_ORANGE          0xFD20 // 255, 165,   0
#define TFT_PINK            0xFE19 // 255, 192, 203
#define TFT_PURPLE          0x780F // 128,   0, 128
#define TFT_RED             0xF800 // 255,   0,   0
#define TFT_SANDYBROWN      0xF52C // 244, 164,  96
#define TFT_TURQUOISE       0x471A //  64, 224, 208
#define TFT_VIOLET          0x801F // 128,   0, 255
#define TFT_WHITE           0xFFFF // 255, 255, 255
#define TFT_YELLOW          0xFFE0 // 255, 255,   0

//-----------------------------------------------------------------------------------------------------------------------
class TFT : public Print {

    public:
        TFT();
        virtual ~TFT(){}

        void begin(uint8_t CS, uint8_t DC, uint8_t spi, uint8_t mosi, uint8_t miso, uint8_t sclk);

        void      setRotation(uint8_t r);
        void      setFrequency(uint32_t f);
        bool      setCursor(uint16_t x, uint16_t y);
        void      invertDisplay(boolean i);
//      void      scrollTo(uint16_t y);

virtual size_t    write(uint8_t);
virtual size_t    write(const uint8_t *buffer, size_t size);

        // Recommended Non-Transaction
        void      drawLine(int16_t Xpos0, int16_t Ypos0, int16_t Xpos1, int16_t Ypos1, uint16_t color);
        void      drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void      drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void      drawRect(int16_t Xpos, int16_t Ypos, uint16_t Width, uint16_t Height,    uint16_t Color);
        void      fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void      drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
        void      fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
        void      fillScreen(uint16_t color);
        void      drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void      fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void      drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        void      fillCircle(int16_t Xm, int16_t Ym, uint16_t r, uint16_t color);
        void      setBrigthness(uint8_t br);
        boolean   drawBmpFile(fs::FS &fs, const char * path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0);
        boolean   drawJpgFile(fs::FS &fs, const char * path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0);
        uint16_t  color565(uint8_t r, uint8_t g, uint8_t b);
        size_t    writeText(const uint8_t *str, int16_t maxHeight = -1);

        inline void setTextColor(uint16_t color){_textcolor=color;}
        inline void setFont(const uint16_t* font){_font=font;
        #ifdef TIMES_NEW_ROMAN_H_
                                                if((_font==Times_New_Roman15x14)||
                                                   (_font==Times_New_Roman21x17)||
                                                   (_font==Times_New_Roman27x21)||
                                                   (_font==Times_New_Roman34x27)||
                                                   (_font==Times_New_Roman38x31)||
                                                   (_font==Times_New_Roman43x35)||
                                                   (_font==Times_New_Roman56x46)||
                                                   (_font==Times_New_Roman66x53)){

                                                    _f_utf8=true; _f_cp1251=false; _f_cp1252=false; _f_cp1253=false;// font can handle UTF-8
                                                }
                                                else _f_utf8=false;
            #endif //TIMES_NEW_ROMAN_H_
            #ifdef GARAMOND_H_
                                                if((_font==Garamond15x18)||
                                                   (_font==Garamond17x21)||
                                                   (_font==Garamond19x24)||
                                                   (_font==Garamond27x33)||
                                                   (_font==Garamond34x42)||
                                                   (_font==Garamond44x54)||
                                                   (_font==Garamond88x108)){
                                                    _f_utf8=false; _f_cp1251=false; _f_cp1252=true; _f_cp1253=false;// font can handle CP1252
                                                 }
                                                 else _f_cp1252=false;
            #endif //GARAMOND_H_
            #ifdef GARAMOND_CYRILLIC_H_
                                                if((_font==Garamond18x18cyrillic)||
                                                   (_font==Garamond21x21cyrillic)||
                                                   (_font==Garamond23x24cyrillic)||
                                                   (_font==Garamond32x33cyrillic)||
                                                   (_font==Garamond41x42cyrillic)||
                                                   (_font==Garamond53x54cyrillic)||
                                                   (_font==Garamond107x108cyrillic)){
                                                    _f_utf8=false; _f_cp1251=true; _f_cp1252=false; _f_cp1253=false;// font can handle CP1251
                                                }
                                                else _f_cp1251=false;
            #endif //GARAMOND_CYRILLIC_H_
            #ifdef GARAMOND_GREEK_H_
                                                if((_font==Garamond15x13greek)||
                                                   (_font==Garamond17x16greek)||
                                                   (_font==Garamond19x17greek)||
                                                   (_font==Garamond27x25greek)||
                                                   (_font==Garamond35x31greek)||
                                                   (_font==Garamond44x41greek)||
                                                   (_font==Garamond85x80greek)){
                                                    _f_utf8=false; _f_cp1251=false; _f_cp1252=false; _f_cp1253=true;// font can handle CP1253
                                                }
                                                else _f_cp1253=false;
            #endif //GARAMOND_GREEK_H_

        }
        inline void setTextSize(uint8_t size){
            #ifdef TIMES_NEW_ROMAN_H_
                if(size==1) _font=Times_New_Roman15x14;
                if(size==2) _font=Times_New_Roman21x17;
                if(size==3) _font=Times_New_Roman27x21;
                if(size==4) _font=Times_New_Roman34x27;
                if(size==5) _font=Times_New_Roman38x31;
                if(size==6) _font=Times_New_Roman43x35;
                if(size==7) _font=Times_New_Roman56x46;
                if(size==8) _font=Times_New_Roman66x53;
            #else
                if(size==1) _font=Garamond15x18;
                if(size==2) _font=Garamond17x21;
                if(size==3) _font=Garamond19x24;
                if(size==4) _font=Garamond27x33;
                if(size==5) _font=Garamond34x42;
                if(size==6) _font=Garamond44x54;
                if(size==7) _font=Garamond88x108;
            #endif
        }

        inline void setTextOrientation(uint8_t orientation=0){_textorientation=orientation;} //0 h other v
//      inline void setUTF8decoder(boolean UTF8){if(UTF8==true) _f_utf8=true; else _f_utf8=false;} // obsolete, will be set automatically
        int16_t height(void) const;
        int16_t width(void) const;
        uint8_t getRotation(void) const;


    private:
        SPIClass*        spi_TFT = nullptr;
        SPISettings      TFT_SPI;
        uint32_t         _freq;
        uint16_t         _height;
        uint16_t         _width;
        uint8_t          _rotation;
        uint16_t         _curX=0;
        int16_t          _curY=0;
        uint16_t         _textcolor = TFT_BLACK;
        uint8_t          _textorientation=0;
        boolean          _f_utf8=false;
        boolean          _f_cp1251=false;
        boolean          _f_cp1252=false;
        boolean          _f_cp1253=false;
        const uint16_t * _font = Times_New_Roman15x14;
        boolean          _f_curPos=false;
        uint8_t          TFT_DC  = 21;    /* Data or Command */
        uint8_t          TFT_CS  = 22;    /* SPI Chip select */
        //uint8_t        TFT_BL  = 17;    /* BackLight */
        uint8_t          TFT_SCK = 18;
        uint8_t          TFT_MISO= 19;
        uint8_t          TFT_MOSI= 23;
        uint8_t          buf[1024];

        inline int minimum(int a, int b){if(a < b) return a; else return b;}
    	inline void TFT_DC_HIGH() {if (TFT_DC < 32) {GPIO.out_w1ts = (1 << TFT_DC);}
    	                           else             {GPIO.out1_w1ts.data = (1 << (TFT_DC - 32));}}
    	inline void TFT_DC_LOW()  {if (TFT_DC < 32) {GPIO.out_w1tc = (1 << TFT_DC);}
    	                           else             {GPIO.out1_w1tc.data = (1 << (TFT_DC - 32));}}
    	inline void TFT_CS_HIGH() {if (TFT_CS < 32) {GPIO.out_w1ts = (1 << TFT_CS);}
    	                           else             {GPIO.out1_w1ts.data = (1 << (TFT_CS - 32));}}
    	inline void TFT_CS_LOW()  {if (TFT_CS < 32) {GPIO.out_w1tc = (1 << TFT_CS);}
    	                           else             {GPIO.out1_w1tc.data = (1 << (TFT_CS - 32));}}
        inline void _swap_int16_t(int16_t &a, int16_t &b) { int16_t t = a; a = b; b = t; }
        void        init();
        void        writeCommand(uint16_t cmd);
        const uint8_t* UTF8toCp1251(const uint8_t* str);
        const uint8_t* UTF8toCp1252(const uint8_t* str);
        const uint8_t* UTF8toCp1253(const uint8_t* str);


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

        void      fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
        void      drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
        void      startBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void      endBitmap();
        void      startJpeg();
        void      endJpeg();
        void      bmpSkipPixels(fs::File &file, uint8_t bitsPerPixel, size_t len);
        void      bmpAddPixels(fs::File &file, uint8_t bitsPerPixel, size_t len);
        void      drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);
        void      renderJPEG(int xpos, int ypos, uint16_t maxWidth, uint16_t maxHeight);
};

//-----------------------------------------------------------------------------------------------------------------------
class JPEGDecoder {

private:

    typedef enum {  // Scan types
        PJPG_GRAYSCALE,
        PJPG_YH1V1,
        PJPG_YH2V1,
        PJPG_YH1V2,
        PJPG_YH2V2
    } pjpeg_scan_type_t;

    typedef enum
    {
       M_SOF0  = 0xC0, M_SOF1  = 0xC1, M_SOF2  = 0xC2, M_SOF3  = 0xC3, M_SOF5  = 0xC5,
       M_SOF6  = 0xC6, M_SOF7  = 0xC7, M_JPG   = 0xC8, M_SOF9  = 0xC9, M_SOF10 = 0xCA,
       M_SOF11 = 0xCB, M_SOF13 = 0xCD, M_SOF14 = 0xCE, M_SOF15 = 0xCF, M_DHT   = 0xC4,
       M_RST0  = 0xD0, M_RST1  = 0xD1, M_RST2  = 0xD2, M_RST3  = 0xD3, M_RST4  = 0xD4,
       M_RST5  = 0xD5, M_RST6  = 0xD6, M_RST7  = 0xD7, M_SOI   = 0xD8, M_EOI   = 0xD9,
       M_SOS   = 0xDA, M_DQT   = 0xDB, M_DNL   = 0xDC, M_DRI   = 0xDD, M_DHP   = 0xDE,
       M_EXP   = 0xDF, M_APP0  = 0xE0, M_APP15 = 0xEF, M_JPG0  = 0xF0, M_JPG13 = 0xFD,
       M_COM   = 0xFE, M_TEM   = 0x01, M_ERROR = 0x100,RST0    = 0xD0, M_DAC   = 0xCC
    } JPEG_MARKER;

    typedef struct{
        int     m_width;        // Image resolution
        int     m_height;
        int     m_comps;        // Number of components (1 or 3)
        int     m_MCUSPerRow;   // Total number of minimum coded units (MCU's) per row/col.
        int     m_MCUSPerCol;
        pjpeg_scan_type_t m_scanType; // Scan type
        int     m_MCUWidth;     // MCU width/height in pixels (each is either 8 or 16 depending on the scan type)
        int     m_MCUHeight;
        uint8_t *m_pMCUBufR;
        uint8_t *m_pMCUBufG;
        uint8_t *m_pMCUBufB;
    } pjpeg_image_info_t;

    typedef struct HufftableT{
        uint16_t mMinCode[16];
        uint16_t mMaxCode[16];
        uint8_t mValPtr[16];
    }HuffTable;

    typedef unsigned char (*pjpeg_need_bytes_callback_t)
        (unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);

    pjpeg_scan_type_t  scanType, gScanType;
    pjpeg_image_info_t image_info;
    pjpeg_need_bytes_callback_t g_pNeedBytesCallback;
    HuffTable gHuffTab0, gHuffTab1, gHuffTab2, gHuffTab3;

    File g_pInFileSd;

    const uint8_t  JPEG_ARRAY = 0;
    const uint8_t  JPEG_FS_FILE=1;
    const uint8_t  JPEG_SD_FILE=2;
    const uint16_t PJPG_MAX_WIDTH       = 16384;
    const uint16_t PJPG_MAX_HEIGHT      = 16384;
    const uint8_t  PJPG_MAXCOMPSINSCAN  = 3;
    const uint8_t  PJPG_DCT_SCALE_BITS  = 7;
    const uint8_t  PJPG_WINOGRAD_QUANT_SCALE_BITS = 10;

    static const uint16_t PJPG_MAX_IN_BUF_SIZE    = 256;

    const uint8_t
    PJPG_NO_MORE_BLOCKS  =  1,  PJPG_TOO_MANY_COMPONENTS      = 11,
    PJPG_BAD_DHT_COUNTS  =  2,  PJPG_BAD_VARIABLE_MARKER      = 12,
    PJPG_BAD_DHT_INDEX   =  3,  PJPG_W_EXTRA_BYTES_BEFORE_MARKER = 16,
    PJPG_BAD_DHT_MARKER  =  4,  PJPG_NO_ARITHMITIC_SUPPORT    = 17,
    PJPG_BAD_DQT_MARKER  =  5,  PJPG_UNEXPECTED_MARKER        = 18,
    PJPG_BAD_DQT_TABLE   =  6,  PJPG_UNSUPPORTED_MARKER       = 20,
    PJPG_BAD_PRECISION   =  7,  PJPG_UNDEFINED_QUANT_TABLE    = 23,
    PJPG_BAD_HEIGHT      =  8,  PJPG_UNDEFINED_HUFF_TABLE     = 24,
    PJPG_BAD_WIDTH       =  9,  PJPG_UNSUPPORTED_COLORSPACE   = 26,
    PJPG_BAD_SOF_LENGTH  = 10,  PJPG_UNSUPPORTED_SAMP_FACTORS = 27,
    PJPG_BAD_DRI_LENGTH  = 13,  PJPG_BAD_RESTART_MARKER       = 29,
    PJPG_BAD_SOS_LENGTH  = 14,  PJPG_BAD_SOS_SPECTRAL         = 31,
    PJPG_BAD_SOS_COMP_ID = 15,  PJPG_BAD_SOS_SUCCESSIVE       = 32,
    PJPG_NOT_JPEG        = 19,  PJPG_STREAM_READ_ERROR        = 33,
    PJPG_BAD_DQT_LENGTH  = 21,  PJPG_UNSUPPORTED_COMP_IDENT   = 35,
    PJPG_TOO_MANY_BLOCKS = 22,  PJPG_UNSUPPORTED_QUANT_TABLE  = 36,
    PJPG_NOT_SINGLE_SCAN = 25,
    PJPG_DECODE_ERROR    = 28,
    PJPG_ASSERTION_ERROR = 30,
    PJPG_NOTENOUGHMEM    = 34,
    PJPG_UNSUPPORTED_MODE= 37;  // picojpeg doesn't support progressive JPEG's

    const uint8_t ZAG[64] = {
         0,   1,   8,  16,   9,   2,   3,  10,  17,  24,  32,  25,  18,  11,   4,   5,
        12,  19,  26,  33,  40,  48,  41,  34,  27,  20,  13,   6,   7,  14,  21,  28,
        35,  42,  49,  56,  57,  50,  43,  36,  29,  22,  15,  23,  30,  37,  44,  51,
        58,  59,  52,  45,  38,  31,  39,  46,  53,  60,  61,  54,  47,  55, 62,  63};

    const uint8_t gWinogradQuant[64] = {
       128, 178, 178, 167, 246, 167, 151, 232, 232, 151, 128, 209, 219, 209, 128, 101,
       178, 197, 197, 178, 101,  69, 139, 167, 177, 167, 139,  69,  35,  96, 131, 151,
       151, 131,  96,  35,  49,  91, 118, 128, 118,  91,  49,  46,  81, 101, 101,  81,
        46,  42,  69,  79,  69,  42,  35,  54,  54,  35,  28,  37,  28,  19,  19,  10};

    uint8_t   status=0;
    uint8_t   jpg_source=0;
    uint8_t   gHuffVal0[16],  gHuffVal1[16];
    uint8_t   gHuffVal2[256], gHuffVal3[256];
    uint8_t   gCompsInScan;
    uint8_t   gValidHuffTables;
    uint8_t   gValidQuantTables;
    uint8_t   gTemFlag;
    uint8_t   gInBuf[PJPG_MAX_IN_BUF_SIZE];
    uint8_t   gInBufOfs;
    uint8_t   gInBufLeft;
    uint8_t   gBitsLeft;
    uint8_t   gCompsInFrame;
    uint8_t   gMaxBlocksPerMCU;
    uint8_t   gMaxMCUXSize;
    uint8_t   gMaxMCUYSize;
    uint8_t   gCompList [3];
    uint8_t   gCompDCTab[3]; // 0,1
    uint8_t   gCompACTab[3]; // 0,1
    uint8_t   gCompIdent[3];
    uint8_t   gCompHSamp[3];
    uint8_t   gCompVSamp[3];
    uint8_t   gCompQuant[3];
    uint8_t   gCallbackStatus;
    uint8_t   gReduce;
    uint8_t   gMCUOrg[6];
    uint8_t   gMCUBufR[256];
    uint8_t   gMCUBufG[256];
    uint8_t   gMCUBufB[256];
    int16_t   is_available=0;
    int16_t   mcu_x;
    int16_t   mcu_y;
    int16_t   gCoeffBuf[8*8];
    int16_t   gQuant0[8*8];
    int16_t   gQuant1[8*8];
    int16_t   gLastDC[3];
    uint16_t  g_nInFileSize=0;
    uint16_t  g_nInFileOfs=0;
    uint16_t  row_pitch=0;
    uint16_t  decoded_width=0, decoded_height=0;
    uint16_t  row_blocks_per_mcu=0, col_blocks_per_mcu=0;
    uint16_t  gBitBuf;
    uint16_t  gImageXSize, gImageYSize;
    uint16_t  gRestartInterval;
    uint16_t  gNextRestartNum;
    uint16_t  gRestartsLeft;
    uint8_t*  jpg_data;
    uint16_t  gMaxMCUSPerRow;
    uint16_t  gMaxMCUSPerCol;
    uint16_t  gNumMCUSRemaining;

    void *g_pCallback_data;

    JPEGDecoder *thisPtr;
    int comps=0;
    int MCUSPerRow=0;
    int MCUSPerCol=0;

public:
    uint16_t *pImage=0;
    int width=0;
    int height=0;
    int MCUWidth=0;
    int MCUHeight=0;
    int MCUx=0;
    int MCUy=0;

    JPEGDecoder();
    ~JPEGDecoder();
    int read(void);
    int decodeSdFile (File g_pInFile);
    void abort(void);
        // Initializes the decompressor. Returns 0 on success, or one of the above error codes on failure. pNeed_bytes_callback will be called
        // to fill the decompressor's internal input buffer. If reduce is 1, only the first pixel of each block will be decoded. This mode is
        // much faster because it skips the AC dequantization, IDCT and chroma upsampling of every image pixel.Not thread safe.
    uint8_t pjpeg_decode_init(pjpeg_image_info_t *pInfo, pjpeg_need_bytes_callback_t pNeed_bytes_callback, void *pCallback_data, unsigned char reduce);
        // Decompresses the file's next MCU. Returns 0 on success, PJPG_NO_MORE_BLOCKS if no more blocks are available, or an error code.
        // Must be called a total of m_MCUSPerRow*m_MCUSPerCol times to completely decompress the image. Not thread safe.
    uint8_t pjpeg_decode_mcu();

private:
    int available(void);
    int readSwappedBytes(void);
    int decodeArray(const uint8_t array[], uint32_t  array_size);
    int decode_mcu(void);
    int decodeCommon(void);
    static uint8_t pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
    uint8_t pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);

    int16_t replicateSignBit16(int8_t n);
    uint16_t getBits(uint8_t numBits, uint8_t FFCheck);
    uint16_t getExtendTest(uint8_t i);
    int16_t getExtendOffset(uint8_t i);
    HuffTable* getHuffTable(uint8_t index);
    uint8_t* getHuffVal(uint8_t index);
    uint8_t readDHTMarker(void);
    uint8_t readDQTMarker(void);
    uint8_t locateSOIMarker(void);
    uint8_t locateSOFMarker(void);
    uint8_t locateSOSMarker(uint8_t* pFoundEOI);
    uint8_t init(void);
    uint8_t processRestart(void);
    uint8_t findEOI(void);
    uint8_t checkHuffTables(void);
    uint8_t checkQuantTables(void);
    uint8_t initScan(void);
    uint8_t initFrame(void);
    uint8_t readSOFMarker(void);
    uint8_t skipVariableMarker(void);
    uint8_t readDRIMarker(void);
    uint8_t readSOSMarker(void);
    uint8_t nextMarker(void);
    uint8_t processMarkers(uint8_t* pMarker);
    uint8_t huffDecode(const HuffTable* pHuffTable, const uint8_t* pHuffVal);
    uint8_t decodeNextMCU(void);
    void fillInBuf(void);
    void fixInBuffer(void);
    void idctRows(void);
    void idctCols(void);
    void createWinogradQuant(int16_t* pQuant);
    void huffCreate(const uint8_t* pBits, HuffTable* pHuffTable);
    void upsampleCb(uint8_t srcOfs, uint8_t dstOfs);
    void upsampleCbH(uint8_t srcOfs, uint8_t dstOfs);
    void upsampleCbV(uint8_t srcOfs, uint8_t dstOfs);
    void upsampleCr(uint8_t srcOfs, uint8_t dstOfs);
    void upsampleCrH(uint8_t srcOfs, uint8_t dstOfs);
    void upsampleCrV(uint8_t srcOfs, uint8_t dstOfs);
    void copyY(uint8_t dstOfs);
    void convertCb(uint8_t dstOfs);
    void convertCr(uint8_t dstOfs);
    void transformBlock(uint8_t mcuBlock);
    void transformBlockReduce(uint8_t mcuBlock);

    inline int jpg_min(int a, int b){
        if(a < b) return a; else return b;
    }
    inline int16_t arithmeticRightShiftN16(int16_t x, int8_t n){
        int16_t r = (uint16_t)x >> (uint8_t)n;
        if (x < 0) r |= replicateSignBit16(n);
        return r;
    }
     inline int32_t arithmeticRightShift8L(long x){
        int32_t r = (unsigned long)x >> 8U;
        if (x < 0) r |= ~(~(unsigned long)0U >> 8U);
        return r;
     }
     inline uint8_t getOctet(uint8_t FFCheck){
        uint8_t c = getChar();
        if ((FFCheck) && (c == 0xFF)){ uint8_t n = getChar();
           if (n){ stuffChar(n); stuffChar(0xFF);}
        } return c;
     }
     inline uint16_t getBits1(uint8_t numBits){
         return getBits(numBits, 0);
     }
     inline uint16_t getBits2(uint8_t numBits){
        return getBits(numBits, 1);
     }
     inline uint8_t getChar(void){
        if (!gInBufLeft){ fillInBuf(); if (!gInBufLeft){
              gTemFlag = ~gTemFlag; return gTemFlag ? 0xFF : 0xD9;}}
        gInBufLeft--; return gInBuf[gInBufOfs++];
     }
     inline void stuffChar(uint8_t i){
        gInBufOfs--;  gInBuf[gInBufOfs] = i; gInBufLeft++;
     }
     inline uint8_t getBit(void){
        uint8_t ret = 0;
        if (gBitBuf & 0x8000) ret = 1;
        if (!gBitsLeft){gBitBuf |= getOctet(1); gBitsLeft += 8;}
        gBitsLeft--; gBitBuf <<= 1;
        return ret;
     }
     inline int16_t huffExtend(uint16_t x, uint8_t s){
        return ((x < getExtendTest(s)) ? ((int16_t)x + getExtendOffset(s)) : (int16_t)x);
     }
     inline uint16_t getMaxHuffCodes(uint8_t index){
         return (index < 2) ? 12 : 255;
     }
     inline int16_t imul_b1_b3(int16_t w){       // 1/cos(4*pi/16)[362, 256+106]
        long x = (w * 362L); x += 128L; return (int16_t)(arithmeticRightShift8L(x));
     }
     inline int16_t imul_b2(int16_t w){          // 1/cos(6*pi/16)[669, 256+256+157]
        long x = (w * 669L); x += 128L; return (int16_t)(arithmeticRightShift8L(x));
     }
     inline int16_t imul_b4(int16_t w){          // 1/cos(2*pi/16)[277, 256+21]
        long x = (w * 277L); x += 128L; return (int16_t)(arithmeticRightShift8L(x));
     }
     inline int16_t imul_b5(int16_t w){          // 1/(cos(2*pi/16) + cos(6*pi/16))[196, 196]
        long x = (w * 196L); x += 128L; return (int16_t)(arithmeticRightShift8L(x));
     }
     inline uint8_t clamp(int16_t s){
        if ((uint16_t)s > 255U){ if (s < 0) return 0; else if (s > 255) return 255;}
        return (uint8_t)s;
     }
     inline uint8_t addAndClamp(uint8_t a, int16_t b){
        b = a + b; if ((uint16_t)b > 255U){if (b < 0)  return 0;else if (b > 255) return 255;}
        return (uint8_t)b;
     }
     inline uint8_t subAndClamp(uint8_t a, int16_t b){
        b = a - b; if ((uint16_t)b > 255U) {if (b < 0) return 0; else if (b > 255) return 255;}
        return (uint8_t)b;
     }
     inline int16_t PJPG_DESCALE(int16_t x){
         return arithmeticRightShiftN16(x + (1U << (PJPG_DCT_SCALE_BITS - 1)), PJPG_DCT_SCALE_BITS);
     }
};

//-----------------------------------------------------------------------------------------------------------------------
class TP {

//Kalibrierung
//x,y | Ux,Uy  0  ,0     | 1922,1930
//x,y | Ux,Uy  320,0     |  140,1930
//x,y | Ux,Uy  0  ,480   | 1922,125
//x,y | Ux,Uy  320,480   |  140,125
//daraus ergib sich für x: (1922-140)/320 = 5,5687mV pro Pixel
//              und für y: (1930-125)/480 = 3,7604mV pro Pixel

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
        const uint16_t Xmax=1922;
        const uint16_t Xmin=140;
        const uint16_t Ymax=1930;
        const uint16_t Ymin=125;

    protected:
        uint16_t TP_Send(uint8_t set_val);
        bool read_TP(uint16_t& x, uint16_t& y);
};



















