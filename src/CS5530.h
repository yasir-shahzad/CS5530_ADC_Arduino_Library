#ifndef CS5530_H
#define CS5530_H

#include <Arduino.h>
#include <SPI.h>

// === User Configurable Constants ===
#define CS5530_DEFAULT_SPI         SPI
#define CS5530_DEFAULT_SPI_FREQUENCY 1000000
#define CS5530_SS                  10

// === CS5530 Command and Register Definitions ===
enum CS5530Command : uint8_t {
    Sync1 = 0xFF,
    Sync0 = 0xFE,
    Null = 0x00,
    SingleConversion = 0x80,
    ContinuousConversion = 0xA0,
    SystemOffsetCalib = 0xB0,
    SystemGainCalib = 0xB8,
};

enum CS5530Register : uint8_t {
    ConfigRegister = 0x03,
    OffsetRegister = 0x01,
    GainRegister = 0x02,
    OffsetRead = 0x01 | 0x08,
    GainRead = 0x02 | 0x08,
};

enum CalibrationType {
    SYSTEM_OFFSET = 0,
    SYSTEM_GAIN = 1
};

// === Bit Masks ===
#define RESET_VALID        (1UL << 6)
#define SYSTEM_RESET       0x200040
#define VOLTAGE_REF_SELECT 0x040000
#define INPUT_SHORT        0x020000
#define REG_DATA_OF        (1UL << 30)

// === Class Declaration ===
class CS5530 {
public:
    CS5530();

    void setPin(int ss);
    void setSPI(SPIClass& spi);
    void setSPIFrequency(uint32_t frequency);
    
    int begin();
    bool reset();
    
    bool setGain(uint8_t gain);
    bool setSampleRate(uint32_t rate);
    bool setConversionMode(uint8_t mode);

    int32_t getReading();
    int32_t getAverage(uint8_t samples = 5);
    bool available();

    float getWeight(bool allowNegative = false, uint8_t samples = 5);
    void calculateZeroOffset(uint8_t samples = 5);
    void calculateCalibrationFactor(float knownWeight, uint8_t samples = 5);

    void setZeroOffset(int32_t offset);
    int32_t getZeroOffset();

    void setCalibrationFactor(float factor);
    float getCalibrationFactor();

    bool setRegister(uint8_t reg, uint32_t value);
    uint32_t getRegister(uint8_t reg);

    bool setBit(uint8_t bit, uint8_t reg);
    bool clearBit(uint8_t bit, uint8_t reg);
    bool getBit(uint8_t bit, uint8_t reg);

    uint8_t calibrate(uint8_t type, int cfgReg);
    uint8_t convert(uint8_t mode, uint8_t regNo, int rate);

private:
    SPIClass* _spi;
    SPISettings _spiSettings;
    int _chipSelectPin;

    int32_t _zeroOffset = 0;
    float _calibrationFactor = 1.0f;

    void writeByte(uint8_t data);
    void write32(uint32_t data);
    uint8_t readByte();
    uint32_t read32();

    uint32_t calculateTwoComplement(uint32_t value);
};

#endif // CS5530_H
