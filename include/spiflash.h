/**
 * @file spiflash.h
 * @author Alexander Rössler
 * @brief SpiFlash Library for Winbond flash modules
 * @date 16-12-2013
 */
/** @defgroup SPIFLASH SpiFlash Library
 * @ingroup Device
 * @{
 */
#pragma once

#include <ssp.h>
#include <gpio.h>
#include <pincon.h>
#include <types.h>

#define SPIFLASH_MAX_COUNT 5u

typedef enum {
    SpiFlash_0 = 0u,
    SpiFlash_1 = 1u,
    SpiFlash_2 = 2u,
    SpiFlash_3 = 3u,
    SpiFlash_4 = 4u
} SpiFlash;

int8 SpiFlash_initialize(SpiFlash id, Ssp ssp, Gpio_Pair selPair);

void SpiFlash_writeEnable(SpiFlash id);
void SpiFlash_VolatileSrWriteEnable(SpiFlash id);
void SpiFlash_writeDisable(SpiFlash id);
void SpiFlash_readStatusRegister1(SpiFlash id, uint8 *statusRegister1);
void SpiFlash_readStatusRegister2(SpiFlash id, uint8 *statusRegister2);
void SpiFlash_writeStatusRegister(SpiFlash id, uint8 statusRegister1, uint8 statusRegister2);
void SpiFlash_pageProgram(SpiFlash id, uint32 address, uint8* data, uint16 dataLength);
void SpiFlash_sectorErase(SpiFlash id, uint32 address);
void SpiFlash_blockErase32(SpiFlash id, uint32 address);
void SpiFlash_blockErase64(SpiFlash id, uint32 address);
void SpiFlash_chipErase(SpiFlash id);
void SpiFlash_eraseProgramSuspend(SpiFlash id);
void SpiFlash_eraseProgramResume(SpiFlash id);
void SpiFlash_powerDown(SpiFlash id);
void SpiFlash_readData(SpiFlash id, uint32 address, uint8 *data, uint32 dataLength);
void SpiFlash_fastRead(SpiFlash id, uint32 address, uint8 *data, uint32 dataLength);
void SpiFlash_releasePowerDown(SpiFlash id);
void SpiFlash_readDeviceId(SpiFlash id, uint8 *deviceId);
void SpiFlash_readManufacturerDeviceId(SpiFlash id, uint8 *manufacturerId, uint8 *deviceId);
void SpiFlash_jedecId(SpiFlash id, uint8 *manufacturer, uint8 *memoryType, uint8 *capacity);
void SpiFlash_readUniqueId(SpiFlash id, uint64 *uniqueId);
void SpiFlash_readSfdbRgister(SpiFlash id, uint8 address, uint8 *data, uint16 dataLength);
void SpiFlash_eraseSecurityRegisters(SpiFlash id, uint32 address);
void SpiFlash_programSecurityRegisters(SpiFlash id, uint32 address, uint8 *data, uint16 dataLength);
void SpiFlash_readSecurityRegisters(SpiFlash id, uint32 address, uint8 *data, uint32 dataLength);
void SpiFlash_enableReset(SpiFlash id);
void SpiFlash_reset(SpiFlash id);
void SpiFlash_erase(SpiFlash id, uint32 address, uint16 dataLength);

/**
 * @}
 */
