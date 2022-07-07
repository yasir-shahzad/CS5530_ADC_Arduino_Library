#include <string.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include "CS5530.h"
#include "Arduino.h"



void CS5530::SPI_Init(void) {
    pinMode(AFECS,OUTPUT);
    digitalWrite(AFECS, LOW);//enabled by default
    SPI.begin ();//initialisation du bus SPI
    SPI.setBitOrder(MSBFIRST);      //MSB first
    SPI.setDataMode(SPI_MODE0);      //mode 0
    SPI.setClockDivider(SPI_CLOCK_DIV8);   //divide the clock by 8 5MHz
    delay(1);
    digitalWrite(AFECS, HIGH);//disable the chip
}


/*
  Enable CS5530
*/
void CS5530::enableChip(void){
    digitalWrite(AFECS, LOW);
}


/*
  Disable CS5530
*/
void CS5530::disableChip(void){
    digitalWrite(AFECS, HIGH);  
}


 bool CS5530::CS5530_Reset(void) {
    int i;
    u32 tmp;
	
    //Initilizing SPI port
    for(i=0;i<15;i++) {
      CS5530_WriteByte(CMD_SYNC1);                                                                     
    }
	
    CS5530_WriteByte(CMD_SYNC0);
	
    // Reseting CS5530
    CS5530_Write_Reg(CMD_CONFIG_WRITE,  REG_CONFIG_RS);    	
    delay(1);  //Wait 1 milli seconds
    CS5530_Write_Reg(CMD_CONFIG_WRITE, CMD_NULL); 
	
    tmp = CS5530_Read_Reg(CMD_CONFIG_READ);	

    if(tmp & REG_CONFIG_RV) {
        return true;
    }

    return false;
 }


 void CS5530::CS5530_Write_Reg(u8 reg, u32 dat) {
 
    CS5530_WriteByte(reg);
    CS5530_Write4Bytes(dat);
 }

  void CS5530::CS5530_Set_Bit(u8 reg, u32 dat) {
    u32 tmp = 0;
    u8 cmd = 0;
    switch (reg)
    {
        case CMD_GAIN_WRITE:   cmd = CMD_GAIN_READ; break; 
        case CMD_OFFSET_WRITE: cmd = CMD_OFFSET_READ; break;		
        case CMD_CONFIG_WRITE: cmd = CMD_CONFIG_READ; break; 
    }

    tmp =  CS5530_Read_Reg(cmd);
    tmp |= dat;
    CS5530_WriteByte(reg);
    CS5530_Write4Bytes(tmp);
 }

  void CS5530::CS5530_Reset_Bit(u8 reg, u32 dat) {
     u32 tmp = 0;
     u8 cmd = 0;
    switch (reg)
    {
	    case CMD_GAIN_WRITE:   cmd = CMD_GAIN_READ; break; 
	    case CMD_OFFSET_WRITE: cmd = CMD_OFFSET_READ; break;		
	    case CMD_CONFIG_WRITE: cmd = CMD_CONFIG_READ; break; 
    }

    tmp =  CS5530_Read_Reg(cmd);
    tmp &= ~dat;
    CS5530_WriteByte(reg);
    CS5530_Write4Bytes(tmp);
 }


 void CS5530::CS5530_WriteByte(u8 dat)         {
 
    enableChip();
    SPI.transfer(dat & 0xFF); 
    disableChip();
  }

 void CS5530::CS5530_Write4Bytes(u32 dat)         	{
    int i;
    u8 tmp;

    for(i=3; i>=0; i--) {
        tmp = (u8)( (dat >> (8*i)) & 0xff);
        CS5530_WriteByte(tmp);
    }
 }
 

 u32 CS5530::CS5530_Read_Reg(u8 reg)      	{
    u32 dat;
	
    CS5530_WriteByte(reg);
    dat = CS5530_Read4Bytes();
	
    return dat;
 }


u32 CS5530::CS5530_Read4Bytes(void)         {
    int i;                                                      
    u32 dat=0; 
    u8 currntByte = 0;
   
    for(i=0; i<4; i++) {
        dat    <<=    8;   
        dat    |= CS5530_ReadByte();
    }                                                                                                                           

    return dat;                                                
}

u8 CS5530::CS5530_ReadByte(void)        {
    u8 dat=0;
	  
    enableChip();
    dat = SPI.transfer(CMD_NULL);
    disableChip();
	  
    return dat;
}



bool CS5530::CS5530_IsReady(void)
{
    if(digitalRead(12) == 0)
    {
        return true;
    }

    return false;
}




u32 CS5530::CS5530_Read_Weightsclae()
{
    u32 rec_data = 0;
    
    EAdStatus status;

   if(CS5530_IsReady()==false)
   {
       status = E_AD_STATUS_BUSY;
       return -2;
   }
   else
   {    
       rec_data = CS5530_Read_Reg(CMD_NULL);
       if((rec_data &  REG_DATA_OF) == 0)
       {
           rec_data &= 0xFFFFFF00;
           
           rec_data = rec_data >> 8;

           status =  E_AD_STATUS_READY;

           return rec_data;
       }
       else
       {
             status =  E_AD_STATUS_OVERFLOW;
             return -1;
       }       
   }
      
   // return status;
}


u8 CS5530::Calibrate(u8 calibrate_type, int cfg_reg, int setup_reg) { 
	u32 calibrate_result;
	int waste_time, i;
	cfg_reg = (int)((calibrate_type % 2 == 1) ? (cfg_reg|REG_CONFIG_IS):(cfg_reg));
	u8 cmd,read_reg;

	CS5530_Write_Reg(CMD_CONFIG_WRITE, cfg_reg);
	CS5530_WriteByte(cmd);

	for(waste_time = WASTE_TIME; waste_time > 0; waste_time--)
		;

	calibrate_result = CS5530_Read_Reg(read_reg);

	printf("The calibration result is: ");
	printf("%x ", calibrate_result);
	printf("\n");

	return 1; 
}

u32 CS5530::TwoComplement(u32 n) {
    u32 negative = (n & (1UL << 23)) != 0;
	u32 native_int;

	if (negative)
	  native_int = n | ~((1UL << 24) - 1);
	else
	  native_int = n;
	return native_int;
}



u8 CS5530::Convert(u8 convert_type, u8 setup_reg_no, u8 reg_no, int word_rate) {
	Serial.print("Prepare for conversion.\n");
	u32 final_result = 0;
	int waste_time, i;
	u32 cfg_reg =  (u32)REG_CONFIG_VRS;
	int setup_reg = ((setup_reg_no % 2 == 1) ? ((word_rate) << 16) : word_rate);
	u8 cmd;

	switch (convert_type)
	{
		case SINGLE_CONVERSION: cmd = CMD_CONVERSION_SIGLE; break;
		case CONTINUED_CONVERSION: cmd = CMD_CONVERSION_CONTINU; break; 
	}

	CS5530_Write_Reg(CMD_CONFIG_WRITE, cfg_reg);
    delay(10);
	//CS5530_WriteByte(cmd);
	Serial.print("Conversion begins...\n");

	delay(10);
	u8 test = 0;
//	test = CS5530_ReadByte();   // wastercycles
	//final_result = CS5530_Read4Bytes();


	Serial.print("The raw result is:"); Serial.println(final_result,BIN);
	Serial.print("The raw result is:"); Serial.println(final_result);

	final_result = TwoComplement(final_result);
	Serial.print("The final result is:"); Serial.println(final_result,BIN);
	Serial.print("The fianl result is:"); Serial.println(final_result);

	final_result = final_result * 500 / 0x7fffff;
	Serial.print("The fianl result is:"); Serial.println(final_result);

	return 1; 
}


