/**
 ****************************************************************************************************
 * @file        myfs.h
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

#ifndef __MYFS_H
#define __MYFS_H

#include "Arduino.h"
#include <FS.h>
#include <SD.h>

/* function declaration */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);         
void createDir(fs::FS &fs, const char * path);                         
void removeDir(fs::FS &fs, const char * path);                         
void readFile(fs::FS &fs, const char * path);                           
void writeFile(fs::FS &fs, const char * path, const char * message);   
void appendFile(fs::FS &fs, const char * path, const char * message);   
void renameFile(fs::FS &fs, const char * path1, const char * path2);   
void deleteFile(fs::FS &fs, const char * path);                        
void testFileIO(fs::FS &fs, const char * path);                      

#endif
