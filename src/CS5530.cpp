#include <string.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include "CS5530.h"
#include "Arduino.h"
#include <stdio.h>


CS5530::CS5530():
_spiSettings(CS5530_DEFAULT_SPI_FREQUENCY, MSBFIRST, SPI_MODE0),
_spi(&CS5530_DEFAULT_SPI), _ss(CS5530_DEFAULT_SS_PIN)
{
    _spi->begin();
}

int CS5530::begin()
{
    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, HIGH);
    _spi->begin();

    return 1;
}

void CS5530::setPin(int ss)
{
    _ss = ss;
}

void CS5530::setSPI(SPIClass &spi)
{
    _spi = &spi;
}

void CS5530::setSPIFrequency(uint32_t frequency)
{
    _spiSettings = SPISettings(frequency, MSBFIRST, SPI_MODE0);
}

bool CS5530::reset(void)
{
    int i;
    uint32_t tmp;

    // Initilizing SPI port
    for (i = 0; i < 15; i++) {
        writeByte(CMD_SYNC1);
    }
    writeByte(CMD_SYNC0);
    // Reseting CS5530
    setRegister(CMD_CONFIG_WRITE, REG_CONFIG_RS);
    delay(1); // Wait 1 milli seconds
    setRegister(CMD_CONFIG_WRITE, CMD_NULL);

    tmp = getRegister(CMD_CONFIG_READ);

    if (tmp & REG_CONFIG_RV) {
        return true;
    }

    return false;
}

//Set the gain
//x1, 2, 4, 8, 16, 32, 64 are avaialable
bool CS5530::setGain(uint8_t gainValue)
{


  uint8_t value = getRegister(NAU7802_CTRL1);
  value &= 0b11111000; //Clear gain bits
  value |= gainValue;  //Mask in new bits

  return (setRegister(NAU7802_CTRL1, value));




   if (gainValue > 1 << 6)
    gainValue = 1 << 6; //Error check

    // Clear gain bits (bits 12, 13, 14, and 15)
    registerValue &= ~(0b1111 << 12);

    // Set new gain bits
    registerValue |= (static_cast<uint32_t>(gainValue) << 12);

    // Return true to indicate success
    return true;











}



bool CS5530::setRegister(uint8_t reg, uint32_t dat) {
    writeByte(reg);
    write32(dat);
    return true;
}

void CS5530::setBit(uint8_t reg, uint32_t dat) {
    uint32_t tmp = 0;
    uint8_t cmd = 0;
    switch (reg)
    {
        case CMD_GAIN_WRITE:   cmd = CMD_GAIN_READ; break; 
        case CMD_OFFSET_WRITE: cmd = CMD_OFFSET_READ; break;		
        case CMD_CONFIG_WRITE: cmd = CMD_CONFIG_READ; break; 
    }

    tmp =  getRegister(cmd);
    tmp |= dat;
    writeByte(reg);
    write32(tmp);
}

void CS5530::resetBit(uint8_t reg, uint32_t dat)
{
    uint32_t tmp = 0;
    uint8_t cmd = 0;
    switch (reg)
    {
    case CMD_GAIN_WRITE:
        cmd = CMD_GAIN_READ;
        break;
    case CMD_OFFSET_WRITE:
        cmd = CMD_OFFSET_READ;
        break;
    case CMD_CONFIG_WRITE:
        cmd = CMD_CONFIG_READ;
        break;
    }

    tmp = getRegister(cmd);
    tmp &= ~dat;
    writeByte(reg);
    write32(tmp);
}

void CS5530::writeByte(uint8_t dat) {
    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
    SPI.transfer(dat & 0xFF);
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
}

void CS5530::write32(uint32_t dat) {
    int i;
    uint8_t tmp;

    for(i=3; i>=0; i--) {
        tmp = (uint8_t)( (dat >> (8*i)) & 0xff);
        writeByte(tmp);
    }
}

uint32_t CS5530::getRegister(uint8_t reg) {
    uint32_t dat;
    writeByte(reg);
    dat = read32();

    return dat;
}

uint32_t CS5530::read32(void) {
    uint32_t dat = 0;

    for (int i = 0; i < 4; i++) {
        dat = (dat << 8) | readByte();
    }

    return dat;
}

uint8_t CS5530::readByte(void) {
    uint8_t dat = 0;
    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
    dat = SPI.transfer(CMD_NULL);
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);

    return dat;
}


bool CS5530::available(void) {
    if(digitalRead(12) == 0)
    {
        return true;
    }

    return false;
}

uint32_t CS5530::getReading()
{
    if (!available())
    {
        // Return -2 for busy status
        return -2;
    }

    uint32_t rec_data = getRegister(CMD_NULL);
    if ((rec_data & REG_DATA_OF) == 0)
    {
        // Perform sign extension and return the result
        return static_cast<int32_t>(rec_data << 24) >> 24;
    }
    else
    {
        // Return -1 for overflow status
        return -1;
    }
}


uint8_t CS5530::calibrate(uint8_t calibrate_type, int cfg_reg, int setup_reg)
{
    uint32_t calibrate_result;
    int waste_time, i;
    cfg_reg = (int)((calibrate_type % 2 == 1) ? (cfg_reg | REG_CONFIG_IS) : (cfg_reg));
    uint8_t cmd, read_reg;

    setRegister(CMD_CONFIG_WRITE, cfg_reg);
    writeByte(cmd);

    for (waste_time = WASTE_TIME; waste_time > 0; waste_time--)
        ;

    calibrate_result = getRegister(read_reg);

    printf("The calibration result is: ");
    printf("%x ", calibrate_result);
    printf("\n");

    return 1;
}

uint32_t CS5530::twoComplement(uint32_t n)
{
    uint32_t negative = (n & (1UL << 23)) != 0;
    uint32_t native_int;

    if (negative)
        native_int = n | ~((1UL << 24) - 1);
    else
        native_int = n;
    return native_int;
}

uint8_t CS5530::convert(uint8_t convert_type, uint8_t setup_reg_no, uint8_t reg_no, int word_rate)
{
    Serial.print("Prepare for conversion.\n");
    uint32_t final_result = 0;
    int waste_time, i;
    uint32_t cfg_reg = (uint32_t)REG_CONFIG_VRS;
    int setup_reg = ((setup_reg_no % 2 == 1) ? ((word_rate) << 16) : word_rate);
    uint8_t cmd;

    switch (convert_type)
    {
    case SINGLE_CONVERSION:
        cmd = CMD_CONVERSION_SIGLE;
        break;
    case CONTINUED_CONVERSION:
        cmd = CMD_CONVERSION_CONTINU;
        break;
    }

    setRegister(CMD_CONFIG_WRITE, cfg_reg);
    delay(10);
    // writeByte(cmd);
    Serial.print("Conversion begins...\n");

    delay(10);
    uint8_t test = 0;
    //	test = readByte();   // wastercycles
    // final_result = read32();

    Serial.print("The raw result is:");
    Serial.println(final_result, BIN);
    Serial.print("The raw result is:");
    Serial.println(final_result);

    final_result = twoComplement(final_result);
    Serial.print("The final result is:");
    Serial.println(final_result, BIN);
    Serial.print("The fianl result is:");
    Serial.println(final_result);

    final_result = final_result * 500 / 0x7fffff;
    Serial.print("The fianl result is:");
    Serial.println(final_result);

    return 1;
}
