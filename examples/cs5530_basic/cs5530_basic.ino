/*
Connections:
Arduino NANO     CS 5530    Description
(ATMEGA 328P)   24bit ADC

D2 (GPIO4)      0 RST         Output from ESP to reset display
D1 (GPIO5)      1 CE          Output from ESP to chip select/enable display
D6 (GPIO12)     2 DC          Output from display data/command to ESP
D7 (GPIO13)     3 Din         Output from ESP SPI MOSI to display data input
D5 (GPIO14)     4 Clk         Output from ESP SPI clock
3V3             5 Vcc         3.3V from ESP to display
D0 (GPIO16)     6 BL          3.3V to turn backlight on, or PWM
G               7 Gnd         Ground

Dependencies:
https://github.com/yasir-shahzad/CS5530

*/


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



