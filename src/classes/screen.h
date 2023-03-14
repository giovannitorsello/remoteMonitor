#ifndef _SCREEN
#define _SCREEN
#include <Wire.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>

#define NULL 0
#define BUFFPIXEL 20
#define MARGIN 10
#define RED_PIN 46
#define GREEN_PIN 45
#define BLUE_PIN 44
 
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
 
const int XP=8,XM=A2,YP=A3,YM=9; //240x320 ID=0x9595
const int TS_LEFT=920,TS_RT=139,TS_TOP=93,TS_BOT=907;
 
MCUFRIEND_kbv tft;
TouchScreen touch = TouchScreen(XP, YP, XM, YM, 300);
uint16_t colors[] = {RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
int colorsLength = sizeof(colors) / sizeof(uint16_t);
String colorNames[] {"rosso", "giallo", "verde", "ciano", "blu", "magenta"};
int sliderWidth;
int colorWidth;
int lastValue = -1;

class Screen
{
private:
    uint8_t         spi_save;

public:
    Screen()
    {
        Serial.println("Configuring screen...");
        tft.begin(tft.readID());
        tft.setRotation(1);
        sliderWidth = tft.width() - 2 * MARGIN;
        colorWidth = sliderWidth / colorsLength;
        initScreen();
        pinMode(RED_PIN, OUTPUT);
        pinMode(GREEN_PIN, OUTPUT);
        pinMode(BLUE_PIN, OUTPUT);
        Serial.println("Screen configuration done");
    }

    void initScreen() {
        tft.fillScreen(BLACK);
        
        tft.setCursor(MARGIN, MARGIN);
        tft.setTextColor(GREEN);                
        loadLogo("LOGO.BMP",1,1);
        tft.setCursor(85, 0);
        tft.println("Remote Monitor 1.0");
        tft.setCursor(85, 10);
        tft.println("Wifinetcom 2023 (R)");
        printNetworkParams();        
    }

    void printNetworkParams() {        
        tft.setCursor(220, 0);        
        tft.println(Ethernet.localIP());
        tft.setCursor(220, 10);
        tft.println(Ethernet.subnetMask());
        tft.setCursor(220, 20);        
        tft.println(Ethernet.gatewayIP());
        tft.setCursor(220, 30);
        tft.println(Ethernet.dnsServerIP());
    }

    void printSwitchStatus() {
        char buffer[100];
        
        tft.setCursor(0, 80);        
        sprintf(buffer,"Stato relais prismi: %d",bitRead(PORTD, 22));
        tft.println(buffer);
        
        tft.setCursor(0, 90);
        sprintf(buffer,"Stato relais 220V - 1: %d",bitRead(PORTD, 24));
        tft.println(buffer);        
        
        tft.setCursor(0, 100);        
        sprintf(buffer,"Stato relais 220V - 2: %d",bitRead(PORTD, 26));
        tft.println(buffer);
                
    }

    void printAlimStatus() {
        tft.setCursor(200, 80);
        tft.println("Alim. 5V - 1: ");
        tft.setCursor(200, 90);
        tft.println("Alim. 5V - 2: ");
        tft.setCursor(200, 100);        
        tft.println("Alim 220V - 1: ");
        tft.setCursor(200, 110);        
        tft.println("Alim 220V - 2: ");
    }

    void printHeartBeat() {
        tft.setCursor(220, 180);        
        tft.println("OK");
    }

    void loadLogo(char *filename, int x, int y) {
        File 	bmpFile;
        int		bmpWidth, bmpHeight;   // W+H in pixels
        uint8_t	bmpDepth;			   // Bit depth (currently must be 24)
        uint32_t bmpImageoffset; 	   // Start of image data in file
        uint32_t rowSize;			   // Not always = bmpWidth; may have padding
        uint8_t	sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
        uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
        uint8_t	buffidx = sizeof(sdbuffer); // Current position in sdbuffer
        boolean	goodBmp = false;	   // Set to true on valid header parse
        boolean	flip	= true; 	   // BMP is stored bottom-to-top
        int		w, h, row, col;
        uint8_t	r, g, b;
        uint32_t pos = 0, startTime = millis();
        uint8_t	lcdidx = 0;
        boolean	first = true;
        
        if((x >= tft.width()) || (y >= tft.height())) return;
        
        Serial.println();
        Serial.print("Loading image '");
        Serial.print(filename);
        Serial.println('\'');
        // Open requested file on SD card
        SPCR = spi_save;
        if ((bmpFile = SD.open(filename)) == NULL) {
            Serial.println("File not found");
            return;
        }
        
        // Parse BMP header
        if(read16(bmpFile) == 0x4D42) { // BMP signature
            Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
            (void)read32(bmpFile); // Read & ignore creator bytes
            bmpImageoffset = read32(bmpFile); // Start of image data
            Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
            // Read DIB header
            Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
            bmpWidth  = read32(bmpFile);
            bmpHeight = read32(bmpFile);
            if(read16(bmpFile) == 1) { // # planes -- must be '1'
            bmpDepth = read16(bmpFile); // bits per pixel
            Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
            if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        
                goodBmp = true; // Supported BMP format -- proceed!
                Serial.print(F("Image size: "));
                Serial.print(bmpWidth);
                Serial.print('x');
                Serial.println(bmpHeight);
        
                // BMP rows are padded (if needed) to 4-byte boundary
                rowSize = (bmpWidth * 3 + 3) & ~3;
        
                // If bmpHeight is negative, image is in top-down order.
                // This is not canon but has been observed in the wild.
                if(bmpHeight < 0) {
                bmpHeight = -bmpHeight;
                flip 	 = false;
                }
        
                // Crop area to be loaded
                w = bmpWidth;
                h = bmpHeight;
                if((x+w-1) >= tft.width())  w = tft.width()  - x;
                if((y+h-1) >= tft.height()) h = tft.height() - y;
        
                // Set TFT address window to clipped image bounds
                SPCR = 0;
                tft.setAddrWindow(x, y, x+w-1, y+h-1);
        
                for (row=0; row<h; row++) { // For each scanline...
                // Seek to start of scan line.  It might seem labor-
                // intensive to be doing this on every line, but this
                // method covers a lot of gritty details like cropping
                // and scanline padding.  Also, the seek only takes
                // place if the file position actually needs to change
                // (avoids a lot of cluster math in SD library).
                if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
                    pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                else 	// Bitmap is stored top-to-bottom
                    pos = bmpImageoffset + row * rowSize;
                SPCR = spi_save;
                if(bmpFile.position() != pos) { // Need seek?
                    bmpFile.seek(pos);
                    buffidx = sizeof(sdbuffer); // Force buffer reload
                }
        
                for (col=0; col<w; col++) { // For each column...
                    // Time to read more pixel data?
                    if (buffidx >= sizeof(sdbuffer)) { // Indeed
                    // Push LCD buffer to the display first
                    if(lcdidx > 0) {
                        SPCR	= 0;
                        tft.pushColors(lcdbuffer, lcdidx, first);
                        lcdidx = 0;
                        first	= false;
                    }
                    SPCR = spi_save;
                    bmpFile.read(sdbuffer, sizeof(sdbuffer));
                    buffidx = 0; // Set index to beginning
                    }
        
                    // Convert pixel from BMP to TFT format
                    b = sdbuffer[buffidx++];
                    g = sdbuffer[buffidx++];
                    r = sdbuffer[buffidx++];
                    lcdbuffer[lcdidx++] = tft.color565(r,g,b);
                } // end pixel
                } // end scanline
                // Write any remaining data to LCD
                if(lcdidx > 0) {
                SPCR = 0;
                tft.pushColors(lcdbuffer, lcdidx, first);
                } 
                Serial.print(F("Loaded in "));
                Serial.print(millis() - startTime);
                Serial.println(" ms");
            } // end goodBmp
            }
        }
        
        bmpFile.close();
        if(!goodBmp) Serial.println("BMP format not recognized.");
        }

    void drawCursor(uint16_t color, int cursor) {
        int pos = MARGIN + cursor;
        tft.fillRect(0, 85, tft.width(), 26, BLACK);
        tft.fillRect(pos - 2, 85, 5, 15, color);
        tft.fillTriangle(pos - 9, 100, pos, 110, pos + 9, 100, color);
    }
    
    void printValue(uint16_t color, String label, String value) {
        tft.setTextColor(color);
        tft.setTextSize(3);
        tft.setCursor(MARGIN, tft.height() - MARGIN - 22);
        tft.print(label + ": ");
        tft.fillRect(MARGIN + 125, tft.height() - MARGIN - 23, tft.width(), 25, BLACK);
        tft.setCursor(MARGIN + 130, tft.height() - MARGIN - 22);
        tft.print(value);
    }
    
    bool readTouch(int &x, int &y) {
        TSPoint point = touch.getPoint();
        pinMode(YP, OUTPUT);
        pinMode(XM, OUTPUT);
        digitalWrite(YP, HIGH);
        digitalWrite(XM, HIGH);
        if (point.z > 10 && point.z < 1000) {
            x = map(point.y, TS_TOP, TS_BOT, 0, tft.width()-1);
            y = map(point.x, TS_RT, TS_LEFT, 0, tft.height()-1);
            return true;
        }
        return false;
    }
    
    uint16_t read16(File f) {
        uint16_t result;
        ((uint8_t *)&result)[0] = f.read(); // LSB
        ((uint8_t *)&result)[1] = f.read(); // MSB
        return result;
    }
 
    uint32_t read32(File f) {
        uint32_t result;
        ((uint8_t *)&result)[0] = f.read(); // LSB
        ((uint8_t *)&result)[1] = f.read();
        ((uint8_t *)&result)[2] = f.read();
        ((uint8_t *)&result)[3] = f.read(); // MSB
        return result;
    }
    void showColor(uint16_t color) {
        int r = map((color & 0xF800) >> 11, 0, 0x1F, 0, 255);
        int g = map((color & 0x7E0) >> 5, 0, 0x3F, 0, 255);
        int b = map(color & 0x1F, 0, 0x1F, 0, 255);
        analogWrite(RED_PIN, r);
        analogWrite(GREEN_PIN, g);
        analogWrite(BLUE_PIN, b);
    }

    int write(char *buf)
    {

        
        return 1;
    }
};

#endif