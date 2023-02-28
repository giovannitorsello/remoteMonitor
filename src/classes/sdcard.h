#ifndef _SDCARD
#define _SDCARD
#include <Arduino.h>
#include <SD.h>
Sd2Card card;
SdVolume volume;
SdFile root;

class SdCard
{
private:
    bool bSdCardPresent = false;
    int delayTime = 1000;
    const int sdPinSelect = 4;
    const int ethPinSelect = 10;

    int initSDCard()
    {
        /*pinMode(sdPinSelect, OUTPUT);
        pinMode(ethPinSelect, OUTPUT);
        digitalWrite(sdPinSelect, LOW);
        digitalWrite(ethPinSelect, HIGH);*/

        if (!card.init(SPI_HALF_SPEED, sdPinSelect))
        {
            Serial.println("Initialization failed. Things to check:");
            Serial.println("* is a card inserted?");
            Serial.println("* is your wiring correct?");
            Serial.println("* did you change the chipSelect pin to match your shield or module?");
            return 0;
        }
        else
        {
            Serial.println("Card is present.");
        }

        // print the type of card
        Serial.print("\nCard type: ");
        switch (card.type())
        {
        case SD_CARD_TYPE_SD1:
            Serial.println("SD1");
            break;
        case SD_CARD_TYPE_SD2:
            Serial.println("SD2");
            break;
        case SD_CARD_TYPE_SDHC:
            Serial.println("SDHC");
            break;
        default:
            Serial.println("Unknown");
        }

        // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
        if (!volume.init(card))
        {
            Serial.println("No FAT16/FAT32 partition");
            return 0;
        }
        else
        {
            Serial.println("FAT partition present");
            Serial.println(volume.fatType(), DEC);
            uint32_t volumesize;
            volumesize = volume.blocksPerCluster(); // clusters are collections of blocks

            volumesize *= volume.clusterCount(); // we'll have a lot of clusters
            volumesize /= 2;                     // SD card blocks are always 512 bytes (2 blocks are 1KB)
            Serial.print("Volume size (Kb):  ");
            Serial.println(volumesize);
            Serial.print("Volume size (Mb):  ");
            volumesize /= 1024;
            Serial.println(volumesize);
            Serial.print("Volume size (Gb):  ");
            Serial.println((float)volumesize / 1024.0);

            root.openRoot(volume);
            root.ls(LS_R | LS_DATE | LS_SIZE);
            root.close();

            SD.begin(sdPinSelect);
            if (!SD.exists("index.htm"))
            {
                Serial.println("ERROR - Can't find index.htm file!");
                return -1;
            }
            else
                Serial.println("SUCCESS - Found index.htm file.");

            // File root = SD.open("/");
            // printDirectory(root, 0);
        }

        return 0;
    }

    void printDirectory(File dir, int numTabs)
    {
        while (true)
        {

            File entry = dir.openNextFile();
            if (!entry)
            {
                // no more files
                break;
            }
            for (uint8_t i = 0; i < numTabs; i++)
            {
                Serial.print('\t');
            }
            Serial.print(entry.name());
            if (entry.isDirectory())
            {
                Serial.println("/");
                printDirectory(entry, numTabs + 1);
            }
            else
            {
                // files have sizes, directories do not
                Serial.print("\t\t");
                Serial.println(entry.size(), DEC);
            }
            entry.close();
        }
    }

public:
    SdCard()
    {
        initSDCard();
    }

    int printCardType()
    {
        return 0;
    }

    int isCardPresent()
    {
        return bSdCardPresent;
    }
};

#endif