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

#define SELF_OFFSET 1
#define SELF_GAIN	2
#define SYSTEM_OFFSET	3
#define SYSTEM_GAIN	4

#define SINGLE_CONVERSION	    1
#define CONTINUED_CONVERSION	2


//Alias of Basic Results
#define PASS              0x0
#define FAIL              0xff
#define TRUE              1
#define FALSE             0
 
#define NOTHING 0

#define CS5532_READ_DATA_TIMEOUT 2000000

 
#define CMD_STOP_CONT_CONV 0xFF
  
#define TIMEOUTERR 0xff
 
#define SUCCESSFUL 0x0  
 
#define CAL_TIMEOUT_LIMIT 4000

#define READ_CONV_RESULT 0x00


  
#define DATA_VALID 0
 
#define ERR_AD_BUSY 1 
 
#define ERR_AD_OVER_FLOW 2
 
#define ERR_FILTER_ONGOING 3


//Configuration Register 
#define REG_CONFIG_PSS      1UL<<31
#define REG_CONFIG_PDM      1UL<<30
#define REG_CONFIG_RS       1UL<<29
#define REG_CONFIG_RV       1UL<<28
#define REG_CONFIG_IS       1UL<<27
#define REG_CONFIG_VRS      1UL<<25
#define REG_CONFIG_A1       1UL<<24
#define REG_CONFIG_A0       1UL<<23
#define REG_CONFIG_FRS      1UL<<19
#define REG_CONFIG_UNIPOLAR 1UL<<10     //U/B (Unipolar = 1 / Bipolar = 0) 
#define REG_CONFIG_OCD      1UL<<9

//CS5532 Gain settings
#define GAINX1  1UL<<24
#define GAINX2  2UL<<24
#define GAINX4  4UL<<24
#define GAINX8  8UL<<24
#define GAINX16 16UL<<24
#define GAINX32 32UL<<24
#define GAINX64 64UL<<24

//CS5532 Word rate settings 
#define WORD_RATE_100SPS  0x0<<11
#define WORD_RATE_50SPS   0x1<<11
#define WORD_RATE_25SPS   0x2<<11
#define WORD_RATE_12P5SPS 0x3<<11
#define WORD_RATE_6P25SPS 0x4<<11
#define WORD_RATE_3200SPS 0x8<<11
#define WORD_RATE_1600SPS 0x9<<11
#define WORD_RATE_800SPS  0xA<<11
#define WORD_RATE_400SPS  0xB<<11
#define WORD_RATE_200SPS  0xC<<11


#define CMD_STOP_CONT_CONV 0xFF


#define CMD_OFFSET_READ  0x09
#define CMD_OFFSET_WRITE 0x01
#define CMD_GAIN_READ    0x0A
#define CMD_GAIN_WRITE   0x02
#define CMD_CONFIG_READ  0x0B
#define CMD_CONFIG_WRITE 0x03

#define REG_DATA_OF    1UL << 3

#define CMD_CONVERSION_SIGLE   0x80
#define CMD_CONVERSION_CONTINU 0xC0
#define CMD_SYS_OFFSET_CALI    0x85
#define CMD_SYS_GAIN_CALI      0x86
#define CMD_SYNC1              0xFF
#define CMD_SYNC0              0xFE
#define CMD_NULL               0x00

#define AFECS 10    // chipselect pin for CS5530

enum EAdStatus {
    E_AD_STATUS_BUSY,
    E_AD_STATUS_READY,
    E_AD_STATUS_OVERFLOW
};



class CS5530 {
public:

 void SPI_Init(void);
 u32 TwoComplement(u32);
 u8 CS5530_ReadByte(void);
 u32 CS5530_Read4Bytes(void);   
 u32 CS5530_Read_Reg(u8);
 u8 Convert(u8, u8, u8, int);
 u8 Calibrate(u8, int, int);
 void CS5530_Write4Bytes(u32);
 void CS5530_WriteByte(u8);
 void CS5530_Write_Reg(u8, u32);
 void CS5530_Set_Bit(u8, u32);
 void CS5530_Reset_Bit(u8, u32);
 bool CS5530_IsReady(void);
 bool CS5530_Reset(void);
 u32  CS5530_Read_Weightsclae();


 private:
 void enableChip(void);  
 void disableChip(void);      
};

#endif




