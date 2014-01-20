/**
 * @file storagemanager.h
 * @author Alexander Rössler
 * @brief Flash storage manager
 * @date 17-01-2014
 */
/** @defgroup StorageManager Storage manager for external flash storage
 * @ingroup Functional
 * @{
 */
#pragma once
#include <types.h>
#include <spiflash.h>
#include <timer.h>

typedef enum 
{
    StorageManager_KeepData_True = 1u,
    StorageManager_KeepData_False = 0u
    
} StorageManager_KeepData;

/** Initializes the storage manager, note that size of data and maximum 
 *  count are defined as precompiler commands
 *  @param spiFlash The SpiFlash module to initialize the storage manager to.
 */
void StorageManager_initialize(SpiFlash spiFlash);
/** Erases one memory sector (4kB)
 *  @param sector The memory sector to erase.
 */
void StorageManager_eraseSector(uint16 sector);
/** Copies a memory sector to another.
 *  @param destinationSector Memory sector where data should be copied to.
 *  @param sourceSector Memory sector where the data should be copied from.
 */
void StorageManager_copySector(uint16 destinationSector, uint16 sourceSector);
/** Writes raw data to a memory page.
 *  @param page The memory page where data write should start.
 *  @param data Pointer to the source data.
 *  @param dataLength Length of the source data.
 *  @param keepData Specifies whether data already in the memory sector should be preserved or not.
 */
void StorageManager_writeRawData(uint16 page, uint8 *data, uint16 dataLength, StorageManager_KeepData keepData);
/** Reads raw to data from a memory page.
 *  @param page The memory page where data should be read from.
 *  @param data Pointer where data should be stored at.
 *  @param dataLength Length of the data to read.
 */
void StorageManager_readRawData(uint16 page, uint8 *data, uint32 dataLength);
/** Adds a data block to the storage manager.
 *  @param data Pointer to the source data.
 *  @param dataLength Length of the data to add.
 *  @return 0 if successful, -1 if not
 */
int8 StorageManager_addData(uint8 *data, uint16 dataLength);
/** Removes a data block from the storage manager.
 *  @param pos Position of the memory block.
 *  @return 0 if successful, -1 if not
 */
int8 StorageManager_removeData(uint16 pos);
/** Reads a data block to the storage manager.
 *  @param pos Position of the memory block.
 *  @param data Pointer where to store the data.
 *  @param dataLength Length of the data to read.
 *  @return 0 if successful, -1 if not
 */
int8 StorageManager_getData(uint16 pos, uint8 *data, uint32 dataLength);
/** Swaps two data blocks in the storage manager.
 *  @param pos1 Position of the first data block
 *  @param pos2 Position of the second data block
 *  @return 0 if successful, -1 if not
 */
int8 StorageManager_swapData(uint16 pos1, uint16 pos2);
/** Returns the cound of allocated data blocks
 *  @return Currently allocated data blocks.
 */
uint16 StorageManager_getDataCount();
/** Clears all data blocks. Warning: This does not erase data!
 */
void StorageManager_clearAllData();

/**
 * @}
 */
