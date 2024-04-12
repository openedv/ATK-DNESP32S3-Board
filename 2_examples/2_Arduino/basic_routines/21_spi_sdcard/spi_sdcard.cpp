/**
 ****************************************************************************************************
 * @file        spi_sdcard.cpp
 * @author      ALIENTEK
 * @brief       SPI_SDCARD code
 * @license     Copyright (C) 2020-2030, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK DNESP32S3 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  :
 * version      data         notes
 * V1.0         20240430     the first version
 *
 ****************************************************************************************************
 */

#include "spi_sdcard.h"
#include <SD.h>
#include <SPI.h>
#include "myfs.h"

File myFile;
SPIClass spi_sdcard;
 
/**
 * @brief       sdcard initialization
 * @param       None
 * @retval      0:succeed; other:failed
 */
uint8_t sdcard_init(void)
{
    spi_sdcard.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);    /* set the pin used by SPI of SD card */ 

    pinMode(SD_CS_PIN, OUTPUT);                 /* CS pin is set to output */

    if (!SD.begin(SD_CS_PIN, spi_sdcard))       /* initialize the SD card */
    {
        Serial.println("SD card initialization failed!");

        if (!SD.begin(SD_CS_PIN, spi_sdcard))   /* SD card fails to be initialized again */
        {
            Serial.println(" SD card initialization failed!");
            return 1;
        }
    }
    Serial.println("SD card initialization succeed!");

    uint8_t cardType = SD.cardType();   /* get the type of card */
    if (cardType == CARD_NONE)          /* the memory card is not connected */
    {
        Serial.println("the memory card is not connected");
        return 2;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)           /* mmc card */
    {
        Serial.println("MMC");
    } 
    else if (cardType == CARD_SD)       /* sd card */
    {
        Serial.println("SDSC");
    } 
    else if (cardType == CARD_SDHC)     /* sdhc card */
    {
        Serial.println("SDHC");
    } 
    else                                /* unknown memory card */
    {
        Serial.println("UNKNOWN");
    }
    return 0;
}

/**
 * @brief       sd card test code
 * @param       None
 * @retval      None
 */
void sd_test(void)
{
    myFile = SD.open("/test.txt", FILE_WRITE);    /* open create file */
    if (myFile)                                   /* the file was successfully opened */
    {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");       /* write data to a file */

        myFile.close();                           /* close file */
        Serial.println("done.");
    } 
    else                                          /* failed to open file */
    {
        Serial.println("error opening test.txt");
    }

    myFile = SD.open("/test.txt");               /* open the file from the previous operation */
    if (myFile) 
    {
        Serial.println("test.txt:");

        while (myFile.available())                  
        {
            Serial.write(myFile.read());          /* read the entire contents of the file */
        }

        myFile.close();                           /* close file  */
    } 
    else                                          /* failed to open file  */
    {
        Serial.println("error opening test.txt");
    }

    listDir(SD, "/", 0);                          /* loop over files in the current working directory */
    createDir(SD, "/mydir");                      /* create folder */
    listDir(SD, "/", 0);                          /* loop over files in the current working directory */
    removeDir(SD, "/mydir");                      /* remote folder */
    listDir(SD, "/", 2);                          /* loop over files in the current working directory */
    writeFile(SD, "/hello.txt", "Hello ");        /* write data to file */
    appendFile(SD, "/hello.txt", "World!\n");     /* Append data to a file */
    readFile(SD, "/hello.txt");                   /* Read data from a file */
    deleteFile(SD, "/foo.txt");                   /* delect file */
    renameFile(SD, "/hello.txt", "/foo.txt");     /* change file name */
    readFile(SD, "/foo.txt");                     /* read data from a file */ 
    testFileIO(SD, "/test.txt");                  /* testing file I/O performance */
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));  /* total capacity of the system to print SD card files */
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));    /* capacity used by the system to print SD card files */
}
