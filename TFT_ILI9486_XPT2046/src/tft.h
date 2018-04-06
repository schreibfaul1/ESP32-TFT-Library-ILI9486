
#pragma once

#include "Arduino.h"
#include "FS.h"
#include "SPI.h"
#include "SD.h"
#include "fonts.h"
#include "picojpeg.h"


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



class TFT : public Print {
    protected:

    public:
        TFT();
        virtual ~TFT(){}

        void      begin(uint8_t CS=22, uint8_t DC=21, uint8_t MOSI=23, uint8_t MISO=19, uint8_t SCK=18);

        void      setRotation(uint8_t r);
        bool      setCursor(uint16_t x, uint16_t y);
        void      invertDisplay(boolean i);
//      void      scrollTo(uint16_t y);

virtual size_t    write(uint8_t);
virtual size_t       write(const uint8_t *buffer, size_t size);

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
        uint8_t   TFT_DC  = 21;    /* Data or Command */
        uint8_t   TFT_CS  = 22;    /* SPI Chip select */
        uint8_t   TFT_BL  = 17;    /* BackLight */
        uint8_t   TFT_SCK = 18;
        uint8_t   TFT_MISO= 19;
        uint8_t   TFT_MOSI= 23;
        char      sbuf[256];

        inline void TFT_DC_HIGH() {GPIO.out_w1ts = (1 << TFT_DC);}
        inline void TFT_DC_LOW()  {GPIO.out_w1tc = (1 << TFT_DC);}
        inline void TFT_CS_HIGH() {GPIO.out_w1ts = (1 << TFT_CS);}
        inline void TFT_CS_LOW()  {GPIO.out_w1tc = (1 << TFT_CS);}
        inline void _swap_int16_t(int16_t a, int16_t b) { int16_t t = a; a = b; b = t; }
        void        init();
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

        void      fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
        void      drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
        void      startBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void      endBitmap();
        void      startJpeg();
        void      endJpeg();
        void      bmpSkipPixels(fs::File &file, uint8_t bitsPerPixel, size_t len);
        void      bmpAddPixels(fs::File &file, uint8_t bitsPerPixel, size_t len);
        void      drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);
        void      renderJPEG(int xpos, int ypos);
};

//------------------------------------------------------------------------------

class JPEGDecoder {


#ifndef jpg_min
    #define jpg_min(a,b)     (((a) < (b)) ? (a) : (b))
#endif

private:
    const uint8_t JPEG_ARRAY = 0;
    const uint8_t JPEG_FS_FILE=1;
    const uint8_t JPEG_SD_FILE=2;
    File g_pInFileSd;
    pjpeg_scan_type_t scan_type;
    pjpeg_image_info_t image_info;

    int16_t   is_available=0;
    int16_t   mcu_x;
    int16_t   mcu_y;
    uint16_t  g_nInFileSize=0;
    uint16_t  g_nInFileOfs=0;
    uint16_t  row_pitch=0;
    uint16_t  decoded_width=0, decoded_height=0;
    uint16_t  row_blocks_per_mcu=0, col_blocks_per_mcu=0;
    uint8_t   status=0;
    uint8_t   jpg_source = 0;
    uint8_t*  jpg_data;

    static uint8_t pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
    uint8_t pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
    int decode_mcu(void);
    int decodeCommon(void);

public:
    uint16_t *pImage=0;
    JPEGDecoder *thisPtr;

    int width=0;
    int height=0;
    int comps=0;
    int MCUSPerRow=0;
    int MCUSPerCol=0;
    pjpeg_scan_type_t scanType;
    int MCUWidth=0;
    int MCUHeight=0;
    int MCUx=0;
    int MCUy=0;

    JPEGDecoder();
    ~JPEGDecoder();

    int available(void);
    int read(void);
    int readSwappedBytes(void);
    int decodeSdFile (File g_pInFile);
    int decodeArray(const uint8_t array[], uint32_t  array_size);
    void abort(void);
};
extern JPEGDecoder JpegDec;


















        //        uint8_t   decodeJpeg(File file);
//        uint8_t   initDecodeJpeg(uint16_t);
//
//        // Jpeg Error codes
//        enum status{JPG_NO_MORE_BLOCKS = 1,       JPG_BAD_DHT_COUNTS=2,          JPG_BAD_DHT_INDEX=3,             JPG_BAD_DHT_MARKER=4,
//                    JPG_BAD_DQT_MARKER=5,           JPG_BAD_DQT_TABLE=6,           JPG_BAD_PRECISION=7,             JPG_BAD_HEIGHT=8,
//                    JPG_BAD_WIDTH=9,              JPG_TOO_MANY_COMPONENTS=10,    JPG_BAD_SOF_LENGTH=11,           JPG_BAD_VARIABLE_MARKER=12,
//                    JPG_BAD_DRI_LENGTH=13,        JPG_BAD_SOS_LENGTH=14,         JPG_BAD_SOS_COMP_ID=15,          JPG_W_EXTRA_BYTES_BEFORE_MARKER=16,
//                    JPG_NO_ARITHMITIC_SUPPORT=17, JPG_UNEXPECTED_MARKER=18,      JPG_NOT_JPEG=19,                 JPG_UNSUPPORTED_MARKER=20,
//                    JPG_BAD_DQT_LENGTH=21,        JPG_TOO_MANY_BLOCKS=22,        JPG_UNDEFINED_QUANT_TABLE=23,    JPG_UNDEFINED_HUFF_TABLE=24,
//                    JPG_NOT_SINGLE_SCAN=25,       JPG_UNSUPPORTED_COLORSPACE=26, JPG_UNSUPPORTED_SAMP_FACTORS=27, JPG_DECODE_ERROR=28,
//                    JPG_BAD_RESTART_MARKER=29,    JPG_ASSERTION_ERROR=30,        JPG_BAD_SOS_SPECTRAL=31,         JPG_BAD_SOS_SUCCESSIVE=32,
//                    JPG_STREAM_READ_ERROR=33,     JPG_NOTENOUGHMEM=34,           JPG_UNSUPPORTED_COMP_IDENT=35,   JPG_UNSUPPORTED_QUANT_TABLE=36,
//                    JPG_UNSUPPORTED_MODE=37,       // doesn't support progressive JPEG's
//        }_status;
//
//        typedef enum{
//           JPG_GRAYSCALE,
//           JPG_YH1V1,
//           JPG_YH2V1,
//           JPG_YH1V2,
//           JPG_YH2V2
//        } pjpeg_scan_type_t;
//
//        typedef struct {
//           int m_width; // Image resolution
//           int m_height;
//           int m_comps; // Number of components (1 or 3)
//           int m_MCUSPerRow;// Total number of minimum coded units (MCU's) per row/col.
//           int m_MCUSPerCol;
//           pjpeg_scan_type_t m_scanType; // Scan type
//           int m_MCUWidth;// MCU width/height in pixels (each is either 8 or 16 depending on the scan type)
//           int m_MCUHeight;
//           unsigned char *m_pMCUBufR;
//           unsigned char *m_pMCUBufG;
//           unsigned char *m_pMCUBufB;
//        } pjpeg_image_info_t;






//Kalibrierung
//x,y | Ux,Uy  0  ,0     | 1922,1930
//x,y | Ux,Uy  320,0     |  140,1930
//x,y | Ux,Uy  0  ,480   | 1922,125
//x,y | Ux,Uy  320,480   |  140,125
//daraus ergib sich für x: (1922-140)/320 = 5,5687mV pro Pixel
//              und für y: (1930-125)/480 = 3,7604mV pro Pixel


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
        const uint16_t Xmax=1922;
        const uint16_t Xmin=140;
        const uint16_t Ymax=1930;
        const uint16_t Ymin=125;

    protected:
        uint16_t TP_Send(uint8_t set_val);
        bool read_TP(uint16_t& x, uint16_t& y);
};



















