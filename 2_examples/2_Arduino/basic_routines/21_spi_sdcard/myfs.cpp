/**
 ****************************************************************************************************
 * @file        myfs.cpp
 * @author      ALIENTEK
 * @brief       MYFS code
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

#include "myfs.h"
#include <FS.h>
#include <SD.h>

/**
 * @brief       loop over files in the current working directory
 * @param       fs : file object
 * @param       dirname : directory path
 * @param       levels  : traversal depth
 * @retval      None
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root)
    {
        Serial.println("Failed to open directory");
        return;
    }

    if(!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file)
    {
        if(file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels)
            {
                listDir(fs, file.path(), levels -1);
            }
        } 
        else 
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/**
 * @brief       create folder
 * @param       fs   : file object
 * @param       path : name of the folder you want to create. It needs to contain paths separated by '/', such as '/ mydir '.
 * @retval      None
 */
void createDir(fs::FS &fs, const char * path)
{
    Serial.printf("Creating Dir: %s\n", path);

    if(fs.mkdir(path))
    {
        Serial.println("Dir created");
    } 
    else 
    {
        Serial.println("mkdir failed");
    }
}

/**
 * @brief       remove folder 
 * @param       fs   : file object
 * @param       path : name of the folder you want to remove. It needs to contain paths separated by '/', such as '/ mydir '.
 * @retval      None
 */
void removeDir(fs::FS &fs, const char * path)
{
    Serial.printf("Removing Dir: %s\n", path);

    if(fs.rmdir(path))
    {
        Serial.println("Dir removed");
    } 
    else 
    {
        Serial.println("rmdir failed");
    }
}

/**
 * @brief       read data from a file 
 * @param       fs   : file object
 * @param       path : files to be read. It needs to contain paths separated by '/', such as '/ test.txt '.
 * @retval      None
 */
void readFile(fs::FS &fs, const char * path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

/**
 * @brief       write data to a file
 * @param       fs   : file object
 * @param       path : file to be written to. It needs to contain paths separated by '/', such as '/ test.txt '.
 * @retval      None
 */
void writeFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    if(file.print(message))
    {
        Serial.println("File written");
    } 
    else 
    {
        Serial.println("Write failed");
    }
    file.close();
}

/**
 * @brief       append data to a file
 * @param       fs      : file object
 * @param       path    : file to be written to. It needs to contain paths separated by '/', such as '/ test.txt '.
 * @param       message : data that needs to be appended to a file
 * @retval      None
 */
void appendFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }

    if(file.print(message))
    {
        Serial.println("Message appended");
    } 
    else 
    {
        Serial.println("Append failed");
    }
    file.close();
}

/**
 * @brief       change the file/directory name (if the directory is different, you can also move the file!)
 * @param       fs    : file object
 * @param       path1 : previous name
 * @param       path2 : new name
 * @retval      None
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2)
{
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) 
    {
        Serial.println("File renamed");
    } 
    else 
    {
        Serial.println("Rename failed");
    }
}

/**
 * @brief       delete files/directories
 * @param       fs   : file object
 * @param       path : file/directory path + name
 * @retval      None
 */
void deleteFile(fs::FS &fs, const char * path)
{
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path))
    {
        Serial.println("File deleted");
    } 
    else 
    {
        Serial.println("Delete failed");
    }
}

/**
 * @brief       testing file I/O performance
 * @param       fs   : file object
 * @param       path : file/directory path + name
 * @retval      None
 */
void testFileIO(fs::FS &fs, const char * path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;

    if(file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len)
        {
            size_t toRead = len;
            if(toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } 
    else 
    {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
