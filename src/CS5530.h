/*
 * Copyright (c) 2022 by Yasir Shahzad <Yasirshahzad918@gmail.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


#ifndef CS5530_H
#define CS5530_H


#define RESET_TIME 1200000
#define WASTE_TIME 90000000

#define SELF_OFFSET     1
#define SELF_GAIN       2
#define SYSTEM_OFFSET   3
#define SYSTEM_GAIN     4

typedef enum {
    SINGLE_CONVERSION = 1,
    CONTINUED_CONVERSION = 2
} Conversion_Type;

//Alias of Basic Results
#define PASS              0x0
#define FAIL              0xff
#define TRUE              1
#define FALSE             0

#define NOTHING 0
#define CS5530_READ_DATA_TIMEOUT 2000000
#define TIMEOUTERR         0xff
#define SUCCESSFUL         0x0
#define CMD_STOP_CONT_CONV 0xff
#define CAL_TIMEOUT_LIMIT  4000
#define READ_CONV_RESULT   0x00

#define DATA_VALID  0
#define ERR_AD_BUSY 1
#define ERR_AD_OVER_FLOW   2
#define ERR_FILTER_ONGOING 3


/**
 * @brief Enumeration of commands that can be sent to the Tic stepper motor controller.
 * 
 * These commands are used to set and retrieve various settings and parameters of the Tic controller.
 * 
 * @note This enumeration is based on the Tic Stepper Motor Controller User's Guide, available at
 * 
 */
enum class Command : uint8_t
{
    OffsetRead          = 0x09, ///< Read the offset value.
    OffsetWrite         = 0x01, ///< Write the offset value.
    GainRead            = 0x0A, ///< Read the gain value.
    GainWrite           = 0x02, ///< Write the gain value.
    ConfigRead          = 0x0B, ///< Read the configuration value.
    ConfigWrite         = 0x03, ///< Write the configuration value.
    SingleConversion    = 0x80, ///< Perform a single conversion.
    ContinuousConversion= 0xC0, ///< Perform continuous conversions.
    SystemOffsetCalib   = 0x85, ///< System offset calibration.
    SystemGainCalib     = 0x86, ///< System gain calibration.
    Sync1               = 0xFF, ///< Part of the serial port re-initialization sequence.
    Sync0               = 0xFE, ///< End of the serial port re-initialization sequence.
    Null                = 0x00  ///< Clear a port flag and keep the converter in continuous conversion mode.
};

typedef enum
{
  CS5530_GAIN_64 = 1 << 6, ///< Gain: 64
  CS5530_GAIN_32 = 1 << 5, ///< Gain: 32
  CS5530_GAIN_16 = 1 << 4, ///< Gain: 16
  CS5530_GAIN_8  = 1 << 3, ///< Gain: 8
  CS5530_GAIN_4  = 1 << 2, ///< Gain: 4
  CS5530_GAIN_2  = 1 << 1, ///< Gain: 2
  CS5530_GAIN_1  = 1 << 0  ///< Gain: 1
} CS5530_Gain_Values;


typedef enum
{
  CS5530_SPS_3200 = 0b1000, ///< 3200 SPS
  CS5530_SPS_1600 = 0b1001, ///< 1600 SPS
  CS5530_SPS_800  = 0b1010, ///< 800 SPS
  CS5530_SPS_400  = 0b1011, ///< 400 SPS
  CS5530_SPS_200  = 0b1100, ///< 200 SPS
  CS5530_SPS_100  = 0b0000, ///< 100 SPS
  CS5530_SPS_50   = 0b0001, ///< 50 SPS
  CS5530_SPS_25   = 0b0010, ///< 25 SPS
  CS5530_SPS_12P5 = 0b0011, ///< 12.5 SPS
  CS5530_SPS_6P25 = 0b0100, ///< 6.25 SPS
} CS5530_SPS_Values;

typedef enum  {
    PWR_SAVE_SELECT     = 1UL << 31, ///< Power Save Select
    POWER_DOWN_MODE     = 1UL << 30, ///< Power Down Mode
    SYSTEM_RESET        = 1UL << 29, ///< Reset System
    RESET_VALID         = 1UL << 28, ///< Reset Valid
    INPUT_SHORT         = 1UL << 27, ///< Input Short
    VOLTAGE_REF_SELECT  = 1UL << 25, ///< Voltage Reference Select
    A1_CONFIGURATION    = 1UL << 24, ///< A1 Configuration
    A0_CONFIGURATION    = 1UL << 23, ///< A0 Configuration
    FILTER_RATE_SELECT  = 1UL << 19, ///< Filter Rate Select
    OPEN_CIRCUIT_DETECT = 1UL << 9   ///< Open Circuit Detect
} RegisterConfig;


//Unipolar / Bipolar
#define CS5530_UNIPOLAR     1UL << 10
#define CS5530_BIPOLAR      0UL << 10
#define REG_DATA_OF         1UL << 3

#define CMD_STOP_CONT_CONV  0xFF

#define CS5530_DEFAULT_SPI           SPI
#define CS5530_DEFAULT_SPI_FREQUENCY 8E6 
#define CS5530_SS        10



enum EAdStatus {
    E_AD_STATUS_BUSY,
    E_AD_STATUS_READY,
    E_AD_STATUS_OVERFLOW
};

class CS5530
{
  public:
    CS5530();
    int begin();
    void setPin(int ss = CS5530_SS);
    void setSPI(SPIClass &spi);
    void setSPIFrequency(uint32_t);
    uint32_t twoComplement(uint32_t);
    bool isConnected();                        //Returns true if device acks at the I2C address

    bool available();                          //Returns true if Cycle Ready bit is set (conversion is complete)
    int32_t getReading();                      //Returns 24-bit reading. Assumes CR Cycle Ready bit (ADC conversion complete) has been checked by .available()
    int32_t getAverage(uint8_t samplesToTake); //Return the average of a given number of readings

    void calculateZeroOffset(uint8_t averageAmount = 8); //Also called taring. Call this with nothing on the scale
    void setZeroOffset(int32_t newZeroOffset);           //Sets the internal variable. Useful for users who are loading values from NVM.
    int32_t getZeroOffset();                             //Ask library for this value. Useful for storing value into NVM.

    void calculateCalibrationFactor(float weightOnScale, uint8_t averageAmount = 8); //Call this with the value of the thing on the scale. Sets the calibration factor based on the weight on scale and zero offset.
    void setCalibrationFactor(float calFactor);                                      //Pass a known calibration factor into library. Helpful if users is loading settings from NVM.
    float getCalibrationFactor();                                                    //Ask library for this value. Useful for storing value into NVM.

    float getWeight(bool allowNegativeWeights = false, uint8_t samplesToTake = 8); //Once you've set zero offset and cal factor, you can ask the library to do the calculations for you.

    bool setGain(uint8_t gainValue); // Set the gain. x1, 2, 4, 8, 16, 32, 64, 128 are available
    bool setSampleRate(uint8_t rate); // Set the readings per second. 10, 20, 40, 80, and 320 samples per second is available

    uint8_t getRegister(uint8_t registerAddress);             //Get contents of a register
    bool setRegister(uint8_t registerAddress, uint32_t value); //Send a given value to be written to given address. Return true if successful

    uint8_t convert(uint8_t, uint8_t, uint8_t, int);
    uint8_t calibrate(uint8_t, int, int);
  
    bool setBit(uint8_t bitNumber, uint8_t registerAddress);   //Mask & set a given bit within a register
    bool clearBit(uint8_t bitNumber, uint8_t registerAddress); //Mask & clear a given bit within a register
    bool getBit(uint8_t bitNumber, uint8_t registerAddress);   //Return a given bit within a register

    bool reset(); //Resets all registers to Power Of Defaults

    private:
    SPISettings _spiSettings;
    SPIClass* _spi;
    int _ss;

    uint8_t readByte();
    int32_t read32();
    void write32(uint32_t value);
    void writeByte(uint8_t value);


};

#endif




