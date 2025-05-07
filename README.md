# Arduino CS5530

[![Build Status](https://travis-ci.org/sandeepmistry/arduino-LoRa.svg?branch=master)](https://travis-ci.org/sandeepmistry/arduino-LoRa)

An [Arduino](https://arduino.cc/) library for sending and receiving data using [Cirrus logic CS5530](https://www.lora-alliance.org/) .

## Compatible Hardware

 * [CS5530](https://www.cirrus.com/products/cs5530/) based boards including:
   * [CS5530 Shield v1.0](https://www.aliexpress.com/item/1005002054335619.html)
   * [CS5530 Sheild v2.0](https://www.aliexpress.com/item/1005002394037116.html) 
  

### Cirrus logic CS5530 wiring

| Cirrus logic CS5530 | Arduino |
| :---------------------: | :------:|
| VCC | 5.0V |
| GND | GND |
| SCK | SCK |
| SDO | MISO |
| SDI | MOSI |
| NSS | 10 |


`NSS` pin can be changed by using `cell.setPin(ss)`. 

**NOTES**:
 * Some boards (like the Arduino Nano), cannot supply enough current for the CS5530 in TX mode. This will cause lockups when sending, be sure to use an external 3.3V supply that can provide at least 120mA's when using these boards.
 * If your Arduino board operates at 5V or 3.3V, no need to use a level converter for the wiring to the Cirrus logic CS5530 module. 

## Installation

### Using the Arduino IDE Library Manager

1. Choose `Sketch` -> `Include Library` -> `Manage Libraries...`
2. Type `CS5530` into the search box.
3. Click the row to select the library.
4. Click the `Install` button to install the library.

### Using Git

```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/yasir-shahzad/CS5530 
```

## API

See [API.md](API.md).

## Examples

See [examples](examples) folder.

## FAQ

**1) Initializing the CS5530 is failing**

Please check the wiring you are using matches what's listed in [Cirrus logic CS5530](# Cirrus logic CS5530-wiring). You can also use `cell.setPins(ss)` to change the default pins used. You can call `cell.setSPIFrequency(frequency)` to lower the SPI frequency used by the library. Both API's must be called before `cell.begin(...)`.

**2) Is CS5530 MSBFIRST or LSBFIRST?**

If we see in the data sheet we can clearly see the CS5530 is MSBFIRST.

**3) Which SPI mode does CS5530 uses?**

if we see at Cpoles and Cphases in data-sheet, we can clearly analize from graphs that it's SPI_MODE0.

**4) Does it SPI works on fixed frequency?**

No, It's SPI work on different frequency ranges.

**5) Can this library be used for CS5532 or CS5534?**

Yes, It's SPI communication works on all the cirrus logic chips. But you have to change register configurations. 

## License

This libary is [licensed](LICENSE) under the [MIT Licence](https://en.wikipedia.org/wiki/MIT_License).
