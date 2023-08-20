/*
Connections:
Arduino NANO/Uno    vCS5530    Description
(ATMEGA 328P)      24bit ADC

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

#include <CS5530.h>
#include "SPI.h"
#include <avr/pgmspace.h>

CS5530 cell;

uint32_t startTime;
int32_t value;

void setup()
{
  Serial.begin(115200);

  if (cell.begin()) {
      Serial.println("CS5530 Initialized Successfully");
  }
 else {
      Serial.println("CS5530 Initialization Failed");
      while (1)
          ;
  }
}

void loop()
{
    int32_t recData = cell.getReading();

 // Serial.println(recData);
    if (recData >= 0)
    {
        value = 0.97 * value + 0.03 * recData; // running average
        delay(5);
    }

    if (millis() > startTime)
    {
       
          Serial.println (String(value) + " grms");
        startTime = millis() + 200;
    }
}
