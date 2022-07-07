
#include "Arduino.h"

#include <avr/pgmspace.h>
#include "SPI.h"
#include "CS5530.h"



CS5530 cell;

u32 startTime;
i32 value;

//VRS = 0;

void setup() {
    Serial.begin(115200);
 
    cell.SPI_Init();   //SPI settings for CS5530
    if (cell.CS5530_Reset())
    Serial.println("CS5530 Initilized");
    else
    Serial.println("CS5530 not working");

    //  cell.CS5530_Write_Reg(CMD_GAIN_WRITE, 0x3);

    u32 tmp = cell.CS5530_Read_Reg(CMD_CONFIG_READ);
    Serial.print("CONFIG Register:");
    Serial.println(tmp, BIN);

    //u32 tmpdata = REG_CONFIG_UNIPOLAR | REG

	cell.CS5530_Write_Reg(CMD_CONFIG_WRITE, REG_CONFIG_UNIPOLAR);

  
    //cell.Convert(CONTINUED_CONVERSION, 1, 1, (int)WORD_RATE_3200SPS );

	u32 cmpl = cell.TwoComplement(0xFFFFFFFF);

	
	 cell.CS5530_WriteByte(CMD_CONVERSION_CONTINU);
     cell.CS5530_Write_Reg(CMD_OFFSET_WRITE, cmpl);
	

}


void loop() {    
		i32 recData = cell.CS5530_Read_Weightsclae();
		
		if(recData > 0) {
			
			value = 0.97 * value + 0.03 * recData;	// running average		
		    delay(5); 
	    }
		
        if(millis() > startTime){
			//Serial.print(recData); Serial.println(" data");
        //  Serial.print((value - 34250)/105);

			 Serial.print((value - 111683  )/18 );
       // Serial.print((value - 111900)/73 );
		  Serial.println(" grms");
		  startTime = millis()+200;
		}
		
    }



