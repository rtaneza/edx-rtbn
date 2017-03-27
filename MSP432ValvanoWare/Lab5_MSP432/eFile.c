// eFile.c
// Runs on either TM4C123 or MSP432
// High-level implementation of the file system implementation.
// Daniel and Jonathan Valvano
// September 13, 2016
#include <stdint.h>
#include <string.h>
#include "eDisk.h"

uint8_t Buff[512];
uint8_t Directory[256], FAT[256];
int32_t bDirectoryLoaded =0; // 0 means disk on ROM is complete, 1 means RAM version active

// Return the larger of two integers.
int16_t max(int16_t a, int16_t b){
  if(a > b){
    return a;
  }
  return b;
}

// if directory and FAT not loaded,
// bring it into RAM from disk
void MountDirectory(void){ 
// if bDirectoryLoaded is 0, 
//    read disk sector 255 and populate Directory and FAT
//    set bDirectoryLoaded=1
// if bDirectoryLoaded is 1, simply return
  if (bDirectoryLoaded == 1) {
    return;
  }
  
  enum DRESULT result = eDisk_ReadSector(Buff, 255);
  if (result != RES_OK) {
    return;
  }
  
  memcpy(Directory, Buff, 256);
  memcpy(FAT, Buff + 256, 256);
  
  bDirectoryLoaded = 1;  
}

// Return the index of the last sector in the file
// associated with a given starting sector.
// Note: This function will loop forever without returning
// if the file has no end (i.e. the FAT is corrupted).
uint8_t lastsector(uint8_t start){
  if (start == 255) {
    return 255;
  }
  
  uint8_t sectorNumber = start;
  while (1) {
    uint8_t nextSector = FAT[sectorNumber];
    if (nextSector == 255) {
      return sectorNumber;
    }    
    sectorNumber = nextSector;
  }
}

// Return the index of the first free sector.
// Note: This function will loop forever without returning
// if a file has no end or if (Directory[255] != 255)
// (i.e. the FAT is corrupted).
uint8_t findfreesector(void){
  // First, find last sector of each file.
  // First free sector = (max last sector) + 1
  int16_t maxLast = -1;
  
  for (uint8_t i = 0; i < 255; i++) {    
    uint8_t last = lastsector(Directory[i]);
    if (last == 255) { // file is empty
      break;
    }
    maxLast = max(last, maxLast);
  }
  
  return (maxLast + 1);
}

// Append a sector index 'n' at the end of file 'num'.
// This helper function is part of OS_File_Append(), which
// should have already verified that there is free space,
// so it always returns 0 (successful).
// Note: This function will loop forever without returning
// if the file has no end (i.e. the FAT is corrupted).
uint8_t appendfat(uint8_t num, uint8_t n){
  uint8_t start = Directory[num];
  if (start == 255) {
    Directory[num] = n;
    return 0;
  }
  
  uint8_t last = lastsector(start);
  FAT[last] = n;
  return 0;
}

//********OS_File_New*************
// Returns a file number of a new file for writing
// Inputs: none
// Outputs: number of a new file
// Errors: return 255 on failure or disk full
uint8_t OS_File_New(void){
  MountDirectory();

  uint8_t i;
  for (i = 0; i < 255; i++) {
    if (Directory[i] == 255) {
      break;
    }
  }
	
  return i;
}

//********OS_File_Size*************
// Check the size of this file
// Inputs:  num, 8-bit file number, 0 to 254
// Outputs: 0 if empty, otherwise the number of sectors
// Errors:  none
uint8_t OS_File_Size(uint8_t num){
  MountDirectory();
  
  uint8_t start = Directory[num];
  if (start == 255) {
    return 0; // file not found
  }
  
  uint8_t numSectors = 0;
  uint8_t sector = start;
  do {
    numSectors++;
    sector = FAT[sector];
  } while (sector != 255);
  
  return numSectors;
}

//********OS_File_Append*************
// Save 512 bytes into the file
// Inputs:  num, 8-bit file number, 0 to 254
//          buf, pointer to 512 bytes of data
// Outputs: 0 if successful
// Errors:  255 on failure or disk full
uint8_t OS_File_Append(uint8_t num, uint8_t buf[512]){
  MountDirectory();
  
  uint8_t free = findfreesector();
  if (free == 255) {
    return 255;
  }
  
  enum DRESULT result = eDisk_WriteSector(buf, free);
  if (result != RES_OK) {
    return 255;
  }
  
  appendfat(num, free);  
  return 0;
}

//********OS_File_Read*************
// Read 512 bytes from the file
// Inputs:  num, 8-bit file number, 0 to 254
//          location, logical address, 0 to 254
//          buf, pointer to 512 empty spaces in RAM
// Outputs: 0 if successful
// Errors:  255 on failure because no data
uint8_t OS_File_Read(uint8_t num, uint8_t location,
                     uint8_t buf[512]){
  MountDirectory();
                       
  uint8_t size = OS_File_Size(num);
  if (location >= size) {
    return 255;
  }
  
  uint8_t start = Directory[num];
  if (start == 255) {
    return 255;
  }
  
  uint8_t i = 0;
  uint8_t sector = start;
  while (i != location) {
    i++;
    sector = FAT[sector];
  }  
  
  enum DRESULT result = eDisk_ReadSector(buf, sector);
  if (result != RES_OK) {
    return 255;
  }
  
  return 0;
}

//********OS_File_Flush*************
// Update working buffers onto the disk
// Power can be removed after calling flush
// Inputs:  none
// Outputs: 0 if success
// Errors:  255 on disk write failure
uint8_t OS_File_Flush(void){
  MountDirectory();
  
  memcpy(Buff, Directory, 256);
  memcpy(Buff + 256, FAT, 256);

  enum DRESULT result = eDisk_WriteSector(Buff, 255);
  if (result != RES_OK) {
    return 255;
  }
  
  return 0;
}

//********OS_File_Format*************
// Erase all files and all data
// Inputs:  none
// Outputs: 0 if success
// Errors:  255 on disk write failure
uint8_t OS_File_Format(void){
// call eDiskFormat
// clear bDirectoryLoaded to zero
  enum DRESULT result = eDisk_Format();
  if (result != RES_OK) {
    return 255;
  }
  
  bDirectoryLoaded = 0;
  return 0;
}
