/*
 * CS5530 ADC Example: Offset Calibration and Filtered Reading
 * 
 * Connections:
 * Arduino (Uno/Nano)   →   CS5530 (24-bit ADC)      | Description
 * --------------------------------------------------|-----------------------------
 * D2 (GPIO4)           →   RST (Pin 0)              | Reset
 * D1 (GPIO5)           →   CE  (Pin 1)              | Chip Select (Active Low)
 * D6 (GPIO12)          →   DC  (Pin 2)              | Data/Command Select
 * D7 (GPIO13)          →   Din (Pin 3)              | SPI MOSI
 * D5 (GPIO14)          →   Clk (Pin 4)              | SPI Clock
 * 3.3V                 →   Vcc (Pin 5)              | Power
 * D0 (GPIO16)          →   BL  (Pin 6)              | Backlight (can use PWM)
 * GND                  →   Gnd (Pin 7)              | Ground
 * 
 * Dependencies:
 * https://github.com/yasir-shahzad/CS5530
 */

 #include <Arduino.h>
 #include <SPI.h>
 #include <CS5530.h>
 
 CS5530 cell;
 
 int32_t filteredValue = 0;
 unsigned long nextPrintTime = 0;
 
 void setup() {
   Serial.begin(115200);
   delay(100);
 
   Serial.println(F("Initializing CS5530..."));
 
   if (cell.reset()) {
     Serial.println(F("CS5530 Initialized Successfully"));
   } else {
     Serial.println(F("CS5530 Initialization Failed"));
     while (true) delay(1000); // Halt
   }
 
   // Read and print current CONFIG register
   uint32_t config = cell.getRegister(ConfigRead);
   Serial.print(F("CONFIG Register: "));
   Serial.println(config, BIN);
 
   // Set unipolar mode (for load cell / weight applications)
   cell.setRegister(ConfigWrite, CS5530_UNIPOLAR);
 
   // Optional: calculate and apply two's complement offset
   uint32_t offset = cell.calculateTwoComplement(0xFFFFFFFF);
   cell.setRegister(CMD_OFFSET_WRITE, offset);
 
   // Start continuous conversion
   cell.writeByte(CMD_CONVERSION_CONTINU);
 
   // Initialize filter with first reading
   filteredValue = cell.getReading();
 }
 
 void loop() {
   int32_t rawReading = cell.getReading();
 
   if (rawReading > 0) {
     // Apply exponential moving average
     filteredValue = 0.97 * filteredValue + 0.03 * rawReading;
   }
 
   // Print value at regular interval
   if (millis() >= nextPrintTime) {
     // Convert raw value to grams (adjust offset and scale for your calibration)
     int32_t grams = (filteredValue - 111683) / 18;
 
     Serial.print(F("Weight: "));
     Serial.print(grams);
     Serial.println(F(" grms"));
 
     nextPrintTime = millis() + 200;
   }
 
   delay(5);
 }
 