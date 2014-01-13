#include "rfm12.h"

#define RFM12_INIT          initializeSsp1
#define RFM12_SPI_SPEED     2.5E6      

static volatile Rfm12_Mode receiverSenderMode;
static volatile Ssp ssps[RFM12_MAX_COUNT];
static volatile Gpio_Pair dataPairs[RFM12_MAX_COUNT];
static volatile bool ookStatus[RFM12_MAX_COUNT];
static volatile uint8 sspIds[RFM12_MAX_COUNT];

int8 Rfm12_initialize(Rfm12 id, Ssp ssp, Gpio_Pair selPair, Gpio_Pair dataPair)
{
    //init ssp
    Ssp_initialize( ssp,
                    RFM12_SPI_SPEED, 
                    Ssp_DataSize_16Bit, 
                    Ssp_FrameFormat_Spi, 
                    Ssp_Mode_Master, 
                    Ssp_Loopback_Disabled,
                    Ssp_SlaveOutput_Enabled,
                    Ssp_ClockOutPolarity_Low,
                    Ssp_ClockOutPhase_First
              );
    sspIds[(uint8)id] = Ssp_initializeSel(ssp, selPair.port, selPair.pin);
    
    dataPairs[id] = dataPair;
    ssps[id] = ssp;
    
    Gpio_setDirection(dataPair.port, dataPair.pin, Gpio_Direction_Input );   // nFS pin must be set
    Pin_setMode(dataPair.port, dataPair.pin, Pin_Mode_NoPullUpDown);
    
    receiverSenderMode = Rfm12_ModeNone;
    
    return (int8)(0);
}

void Rfm12_prepareOokSending(Rfm12 id, Rfm12_FrequencyBand frequencyBand, float frequency, uint32 dataRate)
{
    /*RFXX_WRT_CMD(0x80D7);//EL,EF,12.0pF
    RFXX_WRT_CMD(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC
    RFXX_WRT_CMD(0xA640);//A140=430.8MHz
    RFXX_WRT_CMD(0xC647);//4.8kbps
    RFXX_WRT_CMD(0x94A0);//VDI,FAST,134kHz,0dBm,-103dBm
    RFXX_WRT_CMD(0xC2AC);//AL,!ml,DIG,DQD4
    RFXX_WRT_CMD(0xCA81);//FIFO8,SYNC,!ff,DR
    RFXX_WRT_CMD(0xC483);//@PWR,NO RSTRIC,!st,!fi,OE,EN
    RFXX_WRT_CMD(0x9850);//!mp,9810=30kHz,MAX OUT
    RFXX_WRT_CMD(0xE000);//NOT USE
    RFXX_WRT_CMD(0xC800);//NOT USE
    RFXX_WRT_CMD(0xC400);//1.66MHz,2.2V*/
    
    Rfm12_setBaseConfig(id,             Rfm12_TxFifo_Disabled, 
                                        Rfm12_RxFifo_Disabled, 
                                        Rfm12_FrequencyBand433Mhz, 
                                        Rfm12_CrystalLoadCapacitance12_0pF
                      );
    Rfm12_setPowerManagement(id,        Rfm12_ReceiverDisabled,
                                        Rfm12_BasebandDisabled,
                                        Rfm12_TransmitterDisabled,
                                        Rfm12_SynthesizerDisabled,
                                        Rfm12_OscillatorEnabled,
                                        Rfm12_BrownoutDetectionDisabled,
                                        Rfm12_WakeupTimerDisabled,
                                        Rfm12_ClockOutputDisabled
                           );
    Rfm12_setFrequency(id,              frequencyBand,
                                        frequency
                     );
    Rfm12_setDataRate(id,               dataRate
    );
    Rfm12_setReceiverControl(id,        Rfm12_DataPinVdi,
                                        Rfm12_ValidDataIndicatorResponseFast,
                                        Rfm12_ReceiverBasebandBandwidth200kHz,
                                        Rfm12_LnaGain6dB,
                                        Rfm12_RssiDetectorThreshold97dB
                           );
    Rfm12_setDataFilter(id,             Rfm12_AutoLockDisabled,
                                        Rfm12_ManualLockSlow,
                                        Rfm12_DigitalFilter,
                                        Rfm12_DqdThresholdAlwayOn,
                                        Rfm12_OokModulationEnabled
                      );
    Rfm12_setFifoAndResetMode(id,       Rfm12_FifoInterruptLevelByte,
                                        Rfm12_SyncPatternLength2Byte,
                                        Rfm12_FifoFillConditionSynchronPattern,
                                        Rfm12_FifoFillModeClearAndLock,
                                        Rfm12_SensitiveResetDisabled
                            );
     Rfm12_setAutomaticFrequencyControl(id, Rfm12_AfcAutoModeAlways,
                                        Rfm12_AfcRangeLimitUnlimited,
                                        Rfm12_AfcStrobeEdgeDisabled,
                                        Rfm12_AfcFineModeDisabled,
                                        Rfm12_AfcOffsetEnabled,
                                        Rfm12_AfcEnabled
                                     );
    Rfm12_setClockGenerator(id,         Rfm12_ClockOutputBufferStrong,
                                        Rfm12_OsciallatorLowPowerMode1msStartup,
                                        Rfm12_PhaseDetectorDelayDisabled,
                                        Rfm12_PllDitheringDisabled,
                                        Rfm12_PllBandwith2
                          );
    Rfm12_setTxConfiguration(id,        Rfm12_ModulationPolarityNormal,
                                        Rfm12_FrequencyDeviation15kHz,
                                        Rfm12_RelativeOutputPower0dB);
    Rfm12_setWakeUpTimer(id,            0u,                          // disable wakeuptimer
                                        0u
                       );   
    Rfm12_setLowDutyCycle(id,           0u, 
                                        Rfm12_CyclicWakeupDisabled    // disable low duty cycle
                        ); 
    Rfm12_setLowBatteryDetectorAndClockDivider(id, Rfm12_Clock1MHz,
                                              225u);

    
    ookStatus[id] = false;
}

void Rfm12_prepareOokReceiving(Rfm12 id, Rfm12_FrequencyBand frequencyBand, float frequency, uint32 dataRate)
{
  /*  RFXX_WRT_CMD(0x80D7);//EL,EF,11.5pF
    RFXX_WRT_CMD(0x82D9);//!er,!ebb,ET,ES,EX,!eb,!ew,DC
    RFXX_WRT_CMD(0xA640);//434MHz
    RFXX_WRT_CMD(0xC647);//4.8kbps
    RFXX_WRT_CMD(0x94A0);//VDI,FAST,134kHz,0dBm,-103dBm
    RFXX_WRT_CMD(0xC2AC);//AL,!ml,DIG,DQD4
    RFXX_WRT_CMD(0xCA81);//FIFO8,SYNC,!ff,DR
    RFXX_WRT_CMD(0xC483);//@PWR,NO RSTRIC,!st,!fi,OE,EN
    RFXX_WRT_CMD(0x9850);//!mp,9810=30kHz,MAX OUT
    RFXX_WRT_CMD(0xE000);//NOT USE
    RFXX_WRT_CMD(0xC800);//NOT USE
    RFXX_WRT_CMD(0xC400);//1.66MHz,2.2V*/
    
    Rfm12_setBaseConfig(id,             Rfm12_TxFifo_Disabled, 
                                        Rfm12_RxFifo_Disabled, 
                                        Rfm12_FrequencyBand868Mhz, 
                                        Rfm12_CrystalLoadCapacitance11_5pF
                      );
    Rfm12_setPowerManagement(id,        Rfm12_ReceiverEnabled,
                                        Rfm12_BasebandEnabled,
                                        Rfm12_TransmitterDisabled,
                                        Rfm12_SynthesizerDisabled,
                                        Rfm12_OscillatorDisabled,
                                        Rfm12_BrownoutDetectionDisabled,
                                        Rfm12_WakeupTimerDisabled,
                                        Rfm12_ClockOutputDisabled
                           );
    Rfm12_setFrequency(id,              frequencyBand, 
                                        frequency
                     );
    Rfm12_setDataRate(id,               dataRate
    );
    Rfm12_setReceiverControl(id,        Rfm12_DataPinVdi,
                                        Rfm12_ValidDataIndicatorResponseFast,
                                        Rfm12_ReceiverBasebandBandwidth200kHz,
                                        Rfm12_LnaGain0dB,
                                        Rfm12_RssiDetectorThreshold97dB
                           );
    Rfm12_setDataFilter(id,             Rfm12_AutoLockDisabled,
                                        Rfm12_ManualLockSlow,
                                        Rfm12_DigitalFilter,
                                        Rfm12_DqdThresholdAlwayOn,
                                        Rfm12_OokModulationEnabled
                      );
    Rfm12_setFifoAndResetMode(id,       Rfm12_FifoInterruptLevelByte,
                                        Rfm12_SyncPatternLength2Byte,
                                        Rfm12_FifoFillConditionSynchronPattern,
                                        Rfm12_FifoFillModeClearAndLock,
                                        Rfm12_SensitiveResetDisabled
                            );
    Rfm12_setAutomaticFrequencyControl(id, Rfm12_AfcAutoModeStartup,
                                        Rfm12_AfcRangeLimitSmall,
                                        Rfm12_AfcStrobeEdgeDisabled,
                                        Rfm12_AfcFineModeDisabled,
                                        Rfm12_AfcOffsetEnabled,
                                        Rfm12_AfcEnabled
                                     );
    Rfm12_setClockGenerator(id,         Rfm12_ClockOutputBufferWeak,
                                        Rfm12_OsciallatorLowPowerMode1msStartup,
                                        Rfm12_PhaseDetectorDelayDisabled,
                                        Rfm12_PllDitheringDisabled,
                                        Rfm12_PllBandwith2
                          );
    Rfm12_setWakeUpTimer(id,            0u,
                                        0u
                       );   // disable wakeuptimer
    Rfm12_setLowDutyCycle(id,           0u, 
                                        Rfm12_CyclicWakeupDisabled
                        ); // disable low duty cycle
    Rfm12_setLowBatteryDetectorAndClockDivider(id, Rfm12_Clock1MHz,
                                              225u);
}

void Rfm12_ookSet(Rfm12 id)
{
    Rfm12_setPowerManagement(id,        Rfm12_ReceiverDisabled,
                                        Rfm12_BasebandDisabled,
                                        Rfm12_TransmitterEnabled,
                                        Rfm12_SynthesizerEnabled,
                                        Rfm12_OscillatorEnabled,
                                        Rfm12_BrownoutDetectionDisabled,
                                        Rfm12_WakeupTimerDisabled,
                                        Rfm12_ClockOutputEnabled
                           );
    ookStatus[id] = true;
}

void Rfm12_ookClear(Rfm12 id)
{
    Rfm12_setPowerManagement(id,        Rfm12_ReceiverDisabled,
                                        Rfm12_BasebandDisabled,
                                        Rfm12_TransmitterDisabled,
                                        Rfm12_SynthesizerDisabled,
                                        Rfm12_OscillatorEnabled,
                                        Rfm12_BrownoutDetectionDisabled,
                                        Rfm12_WakeupTimerDisabled,
                                        Rfm12_ClockOutputDisabled
                           );
    ookStatus[id] = false;
}

void Rfm12_ookToggle(Rfm12 id)
{
    if (ookStatus[id] == true)
    {
        Rfm12_ookClear(id);
        ookStatus[id] = false;
    }
    else 
    {  
        Rfm12_ookSet(id);
        ookStatus[id] = true;
    }
}

void Rfm12_disablePower(Rfm12 id)
{
    Rfm12_setPowerManagement(id, 
                             Rfm12_ReceiverDisabled, 
                             Rfm12_BasebandDisabled, 
                             Rfm12_TransmitterDisabled, 
                             Rfm12_SynthesizerDisabled, 
                             Rfm12_OscillatorDisabled,
                             Rfm12_BrownoutDetectionDisabled,
                             Rfm12_WakeupTimerDisabled,
                             Rfm12_ClockOutputDisabled);
}

void Rfm12_setBaseConfig(Rfm12 id,
                        Rfm12_TxFifo txFifoEnable, 
                        Rfm12_RxFifo rxFifoEnable, 
                        Rfm12_FrequencyBand frequencyBand, 
                        Rfm12_CrystalLoadCapacitance loadCapacitance)
{
    uint16 data;
    data = 0x8000u | (txFifoEnable << 7) 
                  | (rxFifoEnable << 6) 
                  | (frequencyBand << 4) 
                  | (loadCapacitance << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setPowerManagement(Rfm12 id,
                             Rfm12_Receiver receiverEnable,
                             Rfm12_Baseband basebandEnable,
                             Rfm12_Transmitter transmitterEnable,
                             Rfm12_Synthesizer synthesizerEnable,
                             Rfm12_Oscillator xtalEnable,
                             Rfm12_BrownoutDetection brownoutEnable,
                             Rfm12_WakeupTimer wakeupEnable,
                             Rfm12_ClockOutput clockEnable)
{
    uint16 data;
    data = 0x8200u | (receiverEnable << 7) 
                  | (basebandEnable << 6) 
                  | (transmitterEnable << 5)
                  | (synthesizerEnable << 4)
                  | (xtalEnable << 3)
                  | (brownoutEnable << 2)
                  | (wakeupEnable << 1)
                  | (clockEnable << 0);
    Ssp_write(ssps[id], sspIds[id], data);
    
    if (receiverEnable == Rfm12_ReceiverEnabled)
    {
        receiverSenderMode = Rfm12_ModeReceiver;
    }
    else if (transmitterEnable == Rfm12_TransmitterEnabled)
    {
        receiverSenderMode = Rfm12_ModeSender;
    }
    else
    {
        receiverSenderMode = Rfm12_ModeNone;
    }
}

void Rfm12_setClockGenerator(Rfm12 id,
                            Rfm12_ClockOutputBuffer outputBuffer,
                            Rfm12_OscillatorLowPowerMode lowPowerXtal,
                            Rfm12_PhaseDetectorDelay phaseDelay,
                            Rfm12_PllDithering ditheringEnable,
                            Rfm12_PllBandwith bandwith)
{
    uint16 data;
    data = 0xCC00u | (outputBuffer << 5)
                  | (lowPowerXtal << 4)
                  | (phaseDelay << 3)
                  | (ditheringEnable << 2)
                  | (bandwith << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setLowBatteryDetectorAndClockDivider(Rfm12 id,
                                               Rfm12_Clock clock,
                                               uint16 thresholdVoltage)
{
    uint16 data;
    uint8 voltageData;
    
    voltageData = (uint8)((thresholdVoltage-225u)/10u);
    
    data = 0xC000u | (clock << 5)
                | (voltageData << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setFrequency(Rfm12 id, Rfm12_FrequencyBand frequencyBand, float frequency)
{
    uint16 data;
    uint16 frequencyData;
    
    if (frequencyBand == Rfm12_FrequencyBand315Mhz)
    {
        frequencyData = (frequency - 310.0) * 400.0;
    }
    else if (frequencyBand == Rfm12_FrequencyBand433Mhz)
    {
        frequencyData = (frequency - 430.0) * 400.0;
    }
    else if (frequencyBand == Rfm12_FrequencyBand868Mhz)
    {
        frequencyData = (frequency - 860.0) * 200.0;
    }
    else if (frequencyBand == Rfm12_FrequencyBand915Mhz)
    {
        frequencyData = (frequency - 900.0) * 133.3;
    }
    
    data = 0xA000u | (frequencyData << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setDataRate(Rfm12 id, uint32 baudrate)
{
    uint16 data;
    uint8  clockSelect;
    uint16 rate;
    
    if (baudrate < 2694u)
    {
        clockSelect = 1u;
        rate = (uint16)((10E6/(29u*8u))/(baudrate));
    }
    else
    {
        clockSelect = 0u;
        rate = (uint16)((10E6/29u)/(baudrate));
    }
    
    data = 0xC600u | (clockSelect << 7)
                  | (rate << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setReceiverControl(Rfm12 id,
                             Rfm12_DataPin pinIntVdi,
                             Rfm12_ValidDataIndicatorResponse validDataIndicatorResponse,
                             Rfm12_ReceiverBasebandBandwidth receiverBasebandBandwidth,
                             Rfm12_LnaGain lnaGainSelect,
                             Rfm12_RssiDetectorThreshold rssiDetectorThreshold)
{
    uint16 data;
    data = 0x9000u | (pinIntVdi << 10)
                  | (validDataIndicatorResponse << 8)
                  | (receiverBasebandBandwidth << 5)
                  | (lnaGainSelect << 3)
                  | (rssiDetectorThreshold << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setSynchronPattern(Rfm12 id, uint8 pattern)
{
    uint16 data;
    data = 0xCE00u | pattern;
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setDataFilter(Rfm12 id,
                        Rfm12_AutoLock autoLock,
                        Rfm12_ManualLock manualLock,
                        Rfm12_Filter filter,
                        Rfm12_DqdThreshold dqdThreshold,
                        Rfm12_OokModulation ookModulationEnable)
{
    uint16 data;
    data = 0xC200u | (autoLock << 7)
                  | (manualLock << 6)
                  | (filter << 4)
                  | (dqdThreshold << 0)
                  | (1 << 5)
                  | (ookModulationEnable << 3);   // this bit is somehow necessary for the OOK demodulation
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setFifoAndResetMode(Rfm12 id,
                              Rfm12_FifoInterruptLevel fifoInterruptLevel,
                              Rfm12_SyncPatternLength syncPatternLength,
                              Rfm12_FifoFillCondition alwayFill,
                              Rfm12_FifoFillMode fifoFill,
                              Rfm12_SensitiveReset sensResetEnable)
{
    uint16 data;
    data = 0xCA00u | (fifoInterruptLevel << 4)
                  | (syncPatternLength << 3)
                  | (alwayFill << 2)
                  | (fifoFill << 1)
                  | (sensResetEnable << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setAutomaticFrequencyControl(Rfm12 id,
                                       Rfm12_AfcAutoMode autoMode,
                                       Rfm12_AfcRangeLimit rangeLimit,
                                       Rfm12_AfcStrobeEdge strobeEdgeEnable,
                                       Rfm12_AfcFineMode fineModeEnable,
                                       Rfm12_AfcOffset afcOffsetEnable,
                                       Rfm12_Afc afcEnable)
{
    uint16 data;
    data = 0xC400u | (autoMode << 6)
                  | (rangeLimit << 4)
                  | (strobeEdgeEnable << 3)
                  | (fineModeEnable << 2)
                  | (afcOffsetEnable << 1)
                  | (afcEnable << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setTxConfiguration(Rfm12 id,
                             Rfm12_ModulationPolarity modulationPolarity,
                             Rfm12_FrequencyDeviation frequencyDeviation,
                             Rfm12_RelativeOutputPower relativeOutputPower)
{
    uint16 data;
    data = 0x9800u | (modulationPolarity << 8)
                  | (frequencyDeviation << 4)
                  | (relativeOutputPower << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setWakeUpTimer(Rfm12 id, uint8 radix, uint8 mantissa)
{
    uint16 data;
    data = 0xE000u | (radix << 8) | (mantissa << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_setLowDutyCycle(Rfm12 id,
                          uint8 duration,
                          Rfm12_CyclicWakeup cyclicWakeupEnable)
{
    uint16 data;
    data = 0xC800u | (duration << 1)
                  | (cyclicWakeupEnable << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}

void Rfm12_softwareReset(Rfm12 id)
{
    uint16 data = 0xFE00u;
    Ssp_write(ssps[id], sspIds[id], data);
}

uint16 Rfm12_readStatus(Rfm12 id)
{
    uint16 writeData = 0x000u;
    uint16 readData;
    
    Ssp_readWrite(ssps[id], sspIds[id], writeData, &readData);
    
    return readData;
}

char Rfm12_read(Rfm12 id)
{
    uint16 writeData = 0xB000u;
    uint16 readData;
    
    Ssp_readWrite(ssps[id], sspIds[id], writeData, &readData);
    
    return (char)(readData & (0x0Fu));
}

void Rfm12_write(Rfm12 id, char byte)
{
    uint16 data;
    data = 0xB800u | (byte << 0);
    Ssp_write(ssps[id], sspIds[id], data);
}
