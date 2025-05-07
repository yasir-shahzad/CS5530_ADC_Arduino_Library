#include "CS5530.h"

CS5530::CS5530()
    : _spiSettings(CS5530_DEFAULT_SPI_FREQUENCY, MSBFIRST, SPI_MODE0),
      _spi(&CS5530_DEFAULT_SPI),
      _chipSelectPin(CS5530_SS) {}

void CS5530::setPin(int ss) {
    _chipSelectPin = ss;
}

void CS5530::setSPI(SPIClass& spi) {
    _spi = &spi;
}

void CS5530::setSPIFrequency(uint32_t frequency) {
    _spiSettings = SPISettings(frequency, MSBFIRST, SPI_MODE0);
}

int CS5530::begin() {
    pinMode(_chipSelectPin, OUTPUT);
    digitalWrite(_chipSelectPin, HIGH);
    _spi->begin();

    if (!reset())
        return 0;

    setRegister(ConfigRegister, CS5530_UNIPOLAR);
    setRegister(OffsetRegister, calculateTwoComplement(0xFFFFFFFF));
    setConversionMode(ContinuousConversion);
    return 1;
}

bool CS5530::reset() {
    for (int i = 0; i < 15; i++) writeByte(Sync1);
    writeByte(Sync0);

    setRegister(ConfigRegister, SYSTEM_RESET);
    delay(1);
    setRegister(ConfigRegister, Null);

    return (getRegister(ConfigRegister) & RESET_VALID);
}

bool CS5530::setGain(uint8_t gain) {
    if (gain > 64) gain = 64;
    uint32_t value = getRegister(GainRegister);
    value &= ~(0b111111UL << 24);
    value |= (static_cast<uint32_t>(gain) << 24);
    return setRegister(GainRegister, value);
}

bool CS5530::setSampleRate(uint32_t rate) {
    if (rate > 0x0F) rate = 0x0F;
    uint32_t value = getRegister(ConfigRegister);
    value &= ~(0x0FUL << 11);
    value |= (rate << 11);
    return setRegister(ConfigRegister, value);
}

bool CS5530::setConversionMode(uint8_t mode) {
    writeByte(mode);
    return true;
}

bool CS5530::available() {
    return digitalRead(12) == LOW;
}

int32_t CS5530::getReading() {
    if (!available()) return -2;

    uint32_t data = getRegister(Null);
    if ((data & REG_DATA_OF) == 0)
        return static_cast<int32_t>(data & 0xFFFFFF00) >> 8;

    return -1;
}

int32_t CS5530::getAverage(uint8_t samples) {
    long total = 0;
    uint8_t count = 0;
    unsigned long start = millis();

    while (millis() - start < 1000) {
        if (available()) {
            total += getReading();
            if (++count == samples) break;
        }
        delay(1);
    }
    return (count > 0) ? total / count : 0;
}

float CS5530::getWeight(bool allowNegative, uint8_t samples) {
    int32_t reading = getAverage(samples);
    if (!allowNegative && reading < _zeroOffset)
        reading = _zeroOffset;
    return (reading - _zeroOffset) / _calibrationFactor;
}

void CS5530::calculateZeroOffset(uint8_t samples) {
    setZeroOffset(getAverage(samples));
}

void CS5530::calculateCalibrationFactor(float weight, uint8_t samples) {
    int32_t raw = getAverage(samples);
    setCalibrationFactor((raw - _zeroOffset) / weight);
}

void CS5530::setZeroOffset(int32_t offset) {
    _zeroOffset = offset;
}

int32_t CS5530::getZeroOffset() {
    return _zeroOffset;
}

void CS5530::setCalibrationFactor(float factor) {
    _calibrationFactor = factor;
}

float CS5530::getCalibrationFactor() {
    return _calibrationFactor;
}

bool CS5530::setRegister(uint8_t reg, uint32_t value) {
    writeByte(reg);
    write32(value);
    return true;
}

uint32_t CS5530::getRegister(uint8_t reg) {
    if (reg > 0x00 && reg < 0x04)
        reg |= 0x08;
    writeByte(reg);
    return read32();
}

bool CS5530::setBit(uint8_t bit, uint8_t reg) {
    uint32_t value = getRegister(reg);
    value |= (1UL << bit);
    return setRegister(reg, value);
}

bool CS5530::clearBit(uint8_t bit, uint8_t reg) {
    uint32_t value = getRegister(reg);
    value &= ~(1UL << bit);
    return setRegister(reg, value);
}

bool CS5530::getBit(uint8_t bit, uint8_t reg) {
    return (getRegister(reg) & (1UL << bit)) != 0;
}

void CS5530::writeByte(uint8_t data) {
    digitalWrite(_chipSelectPin, LOW);
    _spi->beginTransaction(_spiSettings);
    _spi->transfer(data);
    _spi->endTransaction();
    digitalWrite(_chipSelectPin, HIGH);
}

void CS5530::write32(uint32_t data) {
    for (int i = 3; i >= 0; i--)
        writeByte((data >> (8 * i)) & 0xFF);
}

uint8_t CS5530::readByte() {
    _spi->beginTransaction(_spiSettings);
    digitalWrite(_chipSelectPin, LOW);
    uint8_t data = _spi->transfer(Null);
    digitalWrite(_chipSelectPin, HIGH);
    _spi->endTransaction();
    return data;
}

uint32_t CS5530::read32() {
    uint32_t result = 0;
    for (int i = 0; i < 4; i++)
        result = (result << 8) | readByte();
    return result;
}

uint8_t CS5530::calibrate(uint8_t type, int cfgReg) {
    uint8_t cmd = (type == SYSTEM_GAIN) ? SystemGainCalib : SystemOffsetCalib;
    uint8_t readReg = (type == SYSTEM_GAIN) ? GainRead : OffsetRead;

    if (type == SYSTEM_OFFSET)
        cfgReg |= INPUT_SHORT;

    setRegister(ConfigRegister, cfgReg);
    writeByte(cmd);
    delayMicroseconds(10);
    
    uint32_t result = getRegister(readReg);
    Serial.print("Calibration result: ");
    Serial.println(result, HEX);
    return 1;
}

uint8_t CS5530::convert(uint8_t type, uint8_t regNo, int rate) {
    uint8_t cmd = (type == SINGLE_CONVERSION) ? SingleConversion : ContinuousConversion;
    setRegister(ConfigRegister, VOLTAGE_REF_SELECT);
    writeByte(cmd);
    delayMicroseconds(10);

    uint32_t result = 0;  // Placeholder: Add real reading logic here if needed

    Serial.print("Raw result: ");
    Serial.println(result, BIN);
    result = calculateTwoComplement(result);
    Serial.print("Converted result: ");
    Serial.println(result);

    result = result * 500 / 0x7FFFFF;
    Serial.print("Scaled result: ");
    Serial.println(result);
    return 1;
}

uint32_t CS5530::calculateTwoComplement(uint32_t value) {
    return (value & (1UL << 23)) ? (value | ~((1UL << 24) - 1)) : value;
}
