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


//Alias of Basic Types
#define i32 long int
#define i16 int
#define i8  char
#define u32 unsigned long int
#define u16 unsigned int
#define u8  unsigned char


#define RESET_TIME 1200000
#define WASTE_TIME 90000000


#define SELF_OFFSET     1
#define SELF_GAIN       2
#define SYSTEM_OFFSET   3
#define SYSTEM_GAIN     4

#define SINGLE_CONVERSION       1
#define CONTINUED_CONVERSION    2


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


//2.3.9. Configuration Register Descriptions
#define REG_CONFIG_PSS      1UL << 31     //Power Save Select
#define REG_CONFIG_PDM      1UL << 30     //Power Down Mode
#define REG_CONFIG_RS       1UL << 29     //Reset System
#define REG_CONFIG_RV       1UL << 28     //Reset Valid
#define REG_CONFIG_IS       1UL << 27     //Input Short
#define REG_CONFIG_VRS      1UL << 25     //Voltage Reference Select
#define REG_CONFIG_A1       1UL << 24
#define REG_CONFIG_A0       1UL << 23
#define REG_CONFIG_FRS      1UL << 19     //Filter Rate Select
#define REG_CONFIG_OCD      1UL << 9

//CS5532 Gain settings
#define GAINX1              1UL  << 24
#define GAINX2              2UL  << 24
#define GAINX4              4UL  << 24
#define GAINX8              8UL  << 24
#define GAINX16             16UL << 24
#define GAINX32             32UL << 24
#define GAINX64             64UL << 24

//Word Rate when FSR=1
#define WORD_RATE_100SPS    0x0 << 11
#define WORD_RATE_50SPS     0x1 << 11
#define WORD_RATE_25SPS     0x2 << 11
#define WORD_RATE_12P5SPS   0x3 << 11
#define WORD_RATE_6P25SPS   0x4 << 11
#define WORD_RATE_3200SPS   0x8 << 11
#define WORD_RATE_1600SPS   0x9 << 11
#define WORD_RATE_800SPS    0xA << 11
#define WORD_RATE_400SPS    0xB << 11
#define WORD_RATE_200SPS    0xC << 11

//Unipolar / Bipolar
#define CS5530_UNIPOLAR     1UL << 10
#define CS5530_BIPOLAR      0UL << 10
#define REG_DATA_OF         1UL << 3

#define CMD_STOP_CONT_CONV  0xFF

#define 5530_DEFAULT_SPI           SPI
#define 5530_DEFAULT_SPI_FREQUENCY 8E6 
#define 5530_DEFAULT_SS_PIN        10



//Command Register (CS5530_COMM_REG)
#define CMD_OFFSET_READ         0x09
#define CMD_OFFSET_WRITE        0x01
#define CMD_GAIN_READ           0x0A
#define CMD_GAIN_WRITE          0x02
#define CMD_CONFIG_READ         0x0B
#define CMD_CONFIG_WRITE        0x03

#define CMD_CONVERSION_SIGLE    0x80
#define CMD_CONVERSION_CONTINU  0xC0
#define CMD_SYS_OFFSET_CALI     0x85
#define CMD_SYS_GAIN_CALI       0x86
#define CMD_SYNC1               0xFF          //Part of the serial port re-initialization sequence.
#define CMD_SYNC0               0xFE          //End of the serial port re-initialization sequence.
#define CMD_NULL                0x00          //This command is used to clear a port flag and keep the converter in the continuous conversion mode.


enum EAdStatus {
   E_AD_STATUS_BUSY,
   E_AD_STATUS_READY,
   E_AD_STATUS_OVERFLOW
};



class CS5530 {
public:
	
 CS5530();
 int begin();
 void setPin(int ss = 5530_DEFAULT_SS_PIN);
 void setSPI(SPIClass& spi);
 void setSPIFrequency(u32);
 u32 twoComplement(u32);
 u8 readByte(void);
 u32 read4Bytes(void);
 u32 readRegister(u8);
 u8 convert(u8, u8, u8, int);
 u8 calibrate(u8, int, int);
 void write4Bytes(u32);
 void writeByte(u8);
 void writeRegister(u8, u32);
 void setBit(u8, u32);
 void resetBit(u8, u32);
 bool isReady(void);
 bool reset(void);
 u32  readWeightsclae();


private:
 SPISettings _spiSettings;
 SPIClass* _spi;
 int _ss;
};

#endif




