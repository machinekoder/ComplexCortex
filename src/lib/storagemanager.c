#include "storagemanager.h"

#ifndef STORAGE_SECTOR_COUNT
#define STORAGE_SECTOR_COUNT 2048u  // 4kB * 2048 = 8MB
#endif
#ifndef STORAGE_SECTOR_SIZE
#define STORAGE_SECTOR_SIZE  4096u  // 4kB
#endif
#ifndef STORAGE_PAGE_SIZE
#define STORAGE_PAGE_SIZE 256u      // 256B
#endif

#define STORAGE_PAGES_PER_SECTOR (STORAGE_SECTOR_SIZE/STORAGE_PAGE_SIZE)
#define STORAGE_TMP_SECTOR (STORAGE_SECTOR_COUNT-1u)    // last sector as temporary sector for write operations

#define STORAGE_ERASE_DELAY 100u
#define STORAGE_WRITE_DELAY 3u

#ifndef STORAGE_HEADER_PAGE
#define STORAGE_HEADER_PAGE 0u
#endif
#ifndef STORAGE_DATA_STORAGE_PAGE_SIZE
#define STORAGE_DATA_STORAGE_PAGE_SIZE 2u
#endif
#ifndef STORAGE_DATA_PAGE_START
#define STORAGE_DATA_PAGE_START 16u
#endif
#ifndef STORAGE_MAX_COUNT
#define STORAGE_MAX_COUNT 2000u
#endif

typedef struct
{
    uint8 version;
    uint8 firstStartIndicator;
    uint16 dataCount;
    uint16 dataPages[STORAGE_MAX_COUNT];
} StorageManager_StorageHeader;

static SpiFlash storageManagerFlash;

static StorageManager_StorageHeader storageHeader;
static uint8 pageBuffer[STORAGE_PAGE_SIZE];

/** Saves the storage manager header */
static void saveHeader();
/** Returns the first free memory page */
static uint16 getFreePage();

uint16 getFreePage()
{
    uint16 i;
    uint16 currentPage;
    bool found;
    
    currentPage = (STORAGE_DATA_PAGE_START-STORAGE_DATA_STORAGE_PAGE_SIZE);
    
    do 
    {
        found = false;
        currentPage += STORAGE_DATA_STORAGE_PAGE_SIZE;
        for (i = 0u; i < storageHeader.dataCount; i++)
        {
            if (storageHeader.dataPages[i] == currentPage)
            {
                found = true;
                break;
            }
        }
    } 
    while (found == true);
    
    return currentPage;
}

void saveHeader()
{
    StorageManager_writeRawData(STORAGE_HEADER_PAGE,
                        (uint8*)&storageHeader, 
                        sizeof(StorageManager_StorageHeader),
                        StorageManager_KeepData_False
    );
}

void StorageManager_initialize(SpiFlash spiFlash)
{
    storageManagerFlash = spiFlash;
    
    StorageManager_readRawData(STORAGE_HEADER_PAGE, 
                            (uint8*)&storageHeader, 
                            sizeof(StorageManager_StorageHeader));
    
    if (storageHeader.firstStartIndicator != 42u)   // if the first start indicator != 42 it is the first start
    {
        storageHeader.version = 0u;
        storageHeader.firstStartIndicator = 42u;
        storageHeader.dataCount = 0u;
        saveHeader();
    }
}

void StorageManager_eraseSector(uint16 sector)
{
    SpiFlash_writeEnable(storageManagerFlash);
    SpiFlash_sectorErase(storageManagerFlash, STORAGE_SECTOR_SIZE * (uint32)sector);
    Timer_delayMs(STORAGE_ERASE_DELAY);
}

void StorageManager_copySector(uint16 destinationSector, uint16 sourceSector)
{
    uint8 i;
    
    StorageManager_eraseSector(destinationSector);  // delete destination
    for (i = 0u; i < STORAGE_PAGES_PER_SECTOR; i++)
    {
        SpiFlash_fastRead(storageManagerFlash, 
                          (STORAGE_SECTOR_SIZE * sourceSector)+(STORAGE_PAGE_SIZE*i),
                          pageBuffer,
                          STORAGE_PAGE_SIZE
        );
        SpiFlash_writeEnable(storageManagerFlash);
        SpiFlash_pageProgram(storageManagerFlash,
                             (STORAGE_SECTOR_SIZE * destinationSector)+(STORAGE_PAGE_SIZE*i),
                             pageBuffer,
                             STORAGE_PAGE_SIZE
        );
        Timer_delayMs(STORAGE_WRITE_DELAY);
    }
}

void StorageManager_writeRawData(uint16 page, uint8* data, uint16 dataLength, StorageManager_KeepData keepData)
{
    uint8 i;
    uint16 sector;
    uint16 startPage;
    uint16 endPage;
    
    sector = (uint16)(page/STORAGE_PAGES_PER_SECTOR);
    startPage = page - (sector*STORAGE_PAGES_PER_SECTOR);
    endPage = startPage + (uint16)(dataLength/STORAGE_PAGE_SIZE);
    
    if (keepData == StorageManager_KeepData_True)
    {
        StorageManager_copySector(STORAGE_TMP_SECTOR, sector);
    }
    
    StorageManager_eraseSector(sector);
    
    if (keepData == StorageManager_KeepData_True)
    {
        for (i = 0u; i < STORAGE_PAGES_PER_SECTOR; i++)
        {
            if ((i < startPage) || (i > endPage))
            {
                SpiFlash_fastRead(storageManagerFlash, 
                                (STORAGE_SECTOR_SIZE * STORAGE_TMP_SECTOR)+(STORAGE_PAGE_SIZE*i),
                                pageBuffer,
                                STORAGE_PAGE_SIZE
                );
                SpiFlash_writeEnable(storageManagerFlash);
                SpiFlash_pageProgram(storageManagerFlash,
                                    (STORAGE_SECTOR_SIZE * sector)+(STORAGE_PAGE_SIZE*i),
                                    pageBuffer,
                                    STORAGE_PAGE_SIZE
                );
                Timer_delayMs(STORAGE_WRITE_DELAY);
            }
        }
    }
    
    for (i = startPage; i <= endPage; i++)
    {
        uint16 missingBytes;
        missingBytes = dataLength - (i-startPage)*STORAGE_PAGE_SIZE;
        if (missingBytes > STORAGE_PAGE_SIZE)
        {
            missingBytes = STORAGE_PAGE_SIZE;
        }
        SpiFlash_writeEnable(storageManagerFlash);
        SpiFlash_pageProgram(storageManagerFlash,
                            (STORAGE_SECTOR_SIZE * sector)+(STORAGE_PAGE_SIZE*i),
                            &(data[(i-startPage)*STORAGE_PAGE_SIZE]),
                            missingBytes
        );
        Timer_delayMs(STORAGE_WRITE_DELAY);
    }
}

void StorageManager_readRawData(uint16 page, uint8 *data, uint32 dataLength)
{
    SpiFlash_fastRead(storageManagerFlash, STORAGE_PAGE_SIZE * (uint32)page, data, dataLength);
}

int8 StorageManager_addData(uint8 *data, uint16 dataLength)
{
    uint16 targetPage;
    
    if (storageHeader.dataCount >= STORAGE_MAX_COUNT)
    {
        return (int8)(-1);
    }
    
    targetPage = getFreePage();
    StorageManager_writeRawData(targetPage, data, dataLength, StorageManager_KeepData_True);
    storageHeader.dataPages[storageHeader.dataCount] = targetPage;
    storageHeader.dataCount += 1u;
    
    saveHeader();
    
    return (int8)0;
}

int8 StorageManager_removeData(uint16 pos)
{
    uint16 i;
    
    if (pos >= storageHeader.dataCount)
    {
        return (int8)(-1);
    }
    
    storageHeader.dataCount -= 1u;
    
    for (i = pos; i < storageHeader.dataCount; i++) // copy all data one position forward
    {
        storageHeader.dataPages[i] = storageHeader.dataPages[i+1u];
    }
    
    saveHeader();
    
    return (int8)0;
}

int8 StorageManager_getData(uint16 pos, uint8 *data, uint32 dataLength)
{
    if (pos >= storageHeader.dataCount)
    {
        return (int8)(-1);
    }
    
    StorageManager_readRawData(storageHeader.dataPages[pos], data, dataLength);
    
    return (int8)0;
}

int8 StorageManager_swapData(uint16 pos1, uint16 pos2)
{
    uint16 tmpPage;
    
    if ((pos1 >= storageHeader.dataCount)
        || (pos2 >= storageHeader.dataCount))
    {
        return (int8)(-1);
    }
    
    tmpPage = storageHeader.dataPages[pos2];
    storageHeader.dataPages[pos2] = storageHeader.dataPages[pos1];
    storageHeader.dataPages[pos1] = tmpPage;
    
    saveHeader();
    
    return (int8)0;
}

uint16 StorageManager_getDataCount()
{
    return storageHeader.dataCount;
}

void StorageManager_clearAllData()
{
    storageHeader.dataCount = 0u;
    saveHeader();
}
