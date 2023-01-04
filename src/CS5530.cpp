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

int CS5530::begin() {
    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, LOW);  // set SS high
    digitalWrite(_ss, HIGH);  // start SPI
    _spi->begin();

    return 1;
}

void CS5530::setPin(int ss) {
    _ss = ss;
}

void CS5530::setSPI(SPIClass &spi) {
    _spi = &spi;
}

void CS5530::setSPIFrequency(u32 frequency) {
    _spiSettings = SPISettings(frequency, MSBFIRST, SPI_MODE0);
}

bool CS5530::reset(void) {
    int i;
    u32 tmp;

    // Initilizing SPI port
    for (i = 0; i < 15; i++) {
        write8(CMD_SYNC1);
    }

    write8(CMD_SYNC0);

    // Reseting CS5530
    writeRegister(CMD_CONFIG_WRITE, REG_CONFIG_RS);
    delay(1); // Wait 1 milli seconds
    writeRegister(CMD_CONFIG_WRITE, CMD_NULL);

    tmp = readRegister(CMD_CONFIG_READ);

    if (tmp & REG_CONFIG_RV) {
        return true;
    }

    return false;
}

void CS5530::writeRegister(u8 reg, u32 dat) {

    write8(reg);
    write32(dat);
}

void CS5530::setBit(u8 reg, u32 dat) {
    u32 tmp = 0;
    u8 cmd = 0;
    switch (reg) {
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

    tmp = readRegister(cmd);
    tmp |= dat;
    write8(reg);
    write32(tmp);
}

void CS5530::resetBit(u8 reg, u32 dat) {
    u32 tmp = 0;
    u8 cmd = 0;
    switch (reg) {
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

    tmp = readRegister(cmd);
    tmp &= ~dat;
    write8(reg);
    write32(tmp);
}

void CS5530::write8(u8 dat) {

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
    SPI.transfer(dat & 0xFF);
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
}

void CS5530::write32(u32 dat) {
    int i;
    u8 tmp;

    for (i = 3; i >= 0; i--) {
        tmp = (u8)((dat >> (8 * i)) & 0xff);
        write8(tmp);
    }
}

u32 CS5530::readRegister(u8 reg) {
    u32 dat;
    write8(reg);
    dat = read32();
    return dat;
}

u32 CS5530::read32(void) {
    int i;
    u32 dat = 0;
    u8 currntByte = 0;

    for (i = 0; i < 4; i++) {
        dat <<= 8;
        dat |= read8();
    }

    return dat;
}

u8 CS5530::read8(void) {
    u8 dat = 0;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
    dat = SPI.transfer(CMD_NULL);
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);

    return dat;
}

bool CS5530::isReady(void) {
    if (digitalRead(12) == 0) {
        return true;
    }

    return false;
}

u32 CS5530::readWeightsclae() {
    u32 rec_data = 0;
    EAdStatus status;

    if (isReady() == false) {
        status = E_AD_STATUS_BUSY;
        return -2;
    }
    else {
        rec_data = readRegister(CMD_NULL);
        if ((rec_data & REG_DATA_OF) == 0) {
            rec_data &= 0xFFFFFF00;
            rec_data = rec_data >> 8;
            status = E_AD_STATUS_READY;
            return rec_data;
        }
        else {
            status = E_AD_STATUS_OVERFLOW;
            return -1;
        }
    }

    // return status;
}

u8 CS5530::calibrate(u8 calibrate_type, int cfg_reg, int setup_reg) {
    u32 calibrate_result;
    int waste_time, i;
    cfg_reg = (int)((calibrate_type % 2 == 1) ? (cfg_reg | REG_CONFIG_IS) : (cfg_reg));
    u8 cmd, read_reg;

    writeRegister(CMD_CONFIG_WRITE, cfg_reg);
    write8(cmd);

    for (waste_time = WASTE_TIME; waste_time > 0; waste_time--)
        ;

    calibrate_result = readRegister(read_reg);

    printf("The calibration result is: ");
    printf("%x ", calibrate_result);
    printf("\n");

    return 1;
}

u32 CS5530::twoComplement(u32 n) {
    u32 negative = (n & (1UL << 23)) != 0;
    u32 native_int;

    if (negative)
        native_int = n | ~((1UL << 24) - 1);
    else
        native_int = n;
    return native_int;
}

u8 CS5530::convert(u8 convert_type, u8 setup_reg_no, u8 reg_no, int word_rate) {
    Serial.print("Prepare for conversion.\n");
    u32 final_result = 0;
    int waste_time, i;
    u32 cfg_reg = (u32)REG_CONFIG_VRS;
    int setup_reg = ((setup_reg_no % 2 == 1) ? ((word_rate) << 16) : word_rate);
    u8 cmd;

    switch (convert_type) {
        case SINGLE_CONVERSION:
            cmd = CMD_CONVERSION_SIGLE;
            break;
        case CONTINUED_CONVERSION:
            cmd = CMD_CONVERSION_CONTINU;
            break;
    }

    writeRegister(CMD_CONFIG_WRITE, cfg_reg);
    delay(10);
    // write8(cmd);
    Serial.print("Conversion begins...\n");

    delay(10);
    u8 test = 0;
    //	test = read8();   // wastercycles
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
