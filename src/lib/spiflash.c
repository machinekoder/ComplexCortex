#include "spiflash.h"

#define SPIFLASH_SPI_SPEED 2.5E6

static volatile Ssp ssps[SPIFLASH_MAX_COUNT];
static volatile uint8 sspIds[SPIFLASH_MAX_COUNT];

int8 SpiFlash_initialize(SpiFlash id, Ssp ssp, Gpio_Pair selPair)
{
    Ssp_initialize( ssp,
                    SPIFLASH_SPI_SPEED,
                    Ssp_DataSize_8Bit,
                    Ssp_FrameFormat_Spi,
                    Ssp_Mode_Master,
                    Ssp_Loopback_Disabled,
                    Ssp_SlaveOutput_Enabled,
                    Ssp_ClockOutPolarity_Low,
                    Ssp_ClockOutPhase_First
    );
    
    sspIds[id] = Ssp_initializeSel(ssp, selPair.port, selPair.pin, Ssp_AutoSel_Disabled);
    ssps[id] = ssp;
    
    return (int8)(0);
}

void SpiFlash_readUniqueId(SpiFlash id, uint64* uniqueId)
{
    uint16 data;
    
    *uniqueId = 0u;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x4Bu);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 56u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 48u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 40u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 32u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 24u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 16u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 8u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *uniqueId |= ((uint64)data << 0u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_writeEnable(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x06u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_VolatileSrWriteEnable(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x50u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_writeDisable(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x04u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readStatusRegister1(SpiFlash id, uint8 *statusRegister1)
{
    uint16 data;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x05u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *statusRegister1 = (uint8)data;
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readStatusRegister2(SpiFlash id, uint8 *statusRegister2)
{
    uint16 data;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x35u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *statusRegister2 = (uint8)data;
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_writeStatusRegister(SpiFlash id, uint8 statusRegister1, uint8 statusRegister2)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x01u);
    Ssp_write(ssps[id], sspIds[id], (uint16)statusRegister1);
    Ssp_write(ssps[id], sspIds[id], (uint16)statusRegister2);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_pageProgram(SpiFlash id, uint32 address, uint8 *data, uint16 dataLength)
{
    uint16 i;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x02u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_write(ssps[id], sspIds[id], (uint16)data[i]);
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_erase(SpiFlash id, uint32 address, uint16 dataLength)
{
    uint16 i;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x02u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_write(ssps[id], sspIds[id], 0xFFu);
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_sectorErase(SpiFlash id, uint32 address)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x20u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_blockErase32(SpiFlash id, uint32 address)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x52u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_blockErase64(SpiFlash id, uint32 address)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0xD8u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_chipErase(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0xC7u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_eraseProgramSuspend(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x75u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_eraseProgramResume(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x7Au);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_powerDown(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0xB9u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readData(SpiFlash id, uint32 address, uint8* data, uint32 dataLength)
{
    uint32 i;
    uint16 tmpData;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x03u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_read(ssps[id], sspIds[id], &tmpData);
        data[i] = (uint8)tmpData;
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_fastRead(SpiFlash id, uint32 address, uint8* data, uint32 dataLength)
{
    uint32 i;
    uint16 tmpData;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x0Bu);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], 0x00u); // dummy byte
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_read(ssps[id], sspIds[id], &tmpData);
        data[i] = (uint8)tmpData;
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_releasePowerDown(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0xABu);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readDeviceId(SpiFlash id, uint8* deviceId)
{
    uint16 data;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0xABu);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *deviceId = (uint8)data;
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readManufacturerDeviceId(SpiFlash id, uint8* manufacturerId, uint8* deviceId)
{
    uint16 data;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x90u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_write(ssps[id], sspIds[id], 0x00u);
    Ssp_read(ssps[id], sspIds[id], &data);
    *manufacturerId = (uint8)data;
    Ssp_read(ssps[id], sspIds[id], &data);
    *deviceId = (uint8)data;
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_jedecId(SpiFlash id, uint8* manufacturer, uint8* memoryType, uint8* capacity)
{
    uint16 data;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x9Fu);
    Ssp_read(ssps[id], sspIds[id], &data);
    *manufacturer = (uint8)data;
    Ssp_read(ssps[id], sspIds[id], &data);
    *memoryType = (uint8)data;
    Ssp_read(ssps[id], sspIds[id], &data);
    *capacity = (uint8)data;
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readSfdbRgister(SpiFlash id, uint8 address, uint8* data, uint16 dataLength)
{
    uint32 i;
    uint16 tmpData;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x5Au);
    Ssp_write(ssps[id], sspIds[id], 0u);
    Ssp_write(ssps[id], sspIds[id], 0u);
    Ssp_write(ssps[id], sspIds[id], address);
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_read(ssps[id], sspIds[id], &tmpData);
        data[i] = (uint8)tmpData;
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_eraseSecurityRegisters(SpiFlash id, uint32 address)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x44u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_programSecurityRegisters(SpiFlash id, uint32 address, uint8* data, uint16 dataLength)
{
    uint16 i;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x42u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_write(ssps[id], sspIds[id], (uint16)data[i]);
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_readSecurityRegisters(SpiFlash id, uint32 address, uint8* data, uint32 dataLength)
{
    uint32 i;
    uint16 tmpData;
    
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x48u);
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 16u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 8u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], (uint16)((address >> 0u) & 0xFFu));
    Ssp_write(ssps[id], sspIds[id], 0x00u); // dummy byte
    for (i = 0u; i < dataLength; i++)
    {
        Ssp_read(ssps[id], sspIds[id], &tmpData);
        data[i] = (uint8)tmpData;
    }
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_enableReset(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x66u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}

void SpiFlash_reset(SpiFlash id)
{
    Ssp_enableSel(ssps[id], sspIds[id]);
    Ssp_write(ssps[id], sspIds[id], 0x99u);
    Ssp_disableSel(ssps[id], sspIds[id]);
}
