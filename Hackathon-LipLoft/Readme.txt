LipLoft

This code is a basic implementation for controlling a remote controlled drone with a LipSync Wireless.

The following components are required:

- LipSync Wireless with custom Firmware
- Drone
- Modified Drone Remote Control
 - Drone Remote Control Interface




Installing Custom Libraries
- Download .zip file from Github
- In Arduino IDE: Sketch > Include Library > Add .zip Library

MCP4261 Arduino Drivers
https://github.com/dreamcat4/Mcp4261
(Not sure why this particular library was chosen over the other ones available through Arduino Library Manager)


SPI Library by Cam Thompson
http://www.arduino.cc/playground/Code/Spi
(Still referenced in the MCP4261 Library documentation, but doesn't appear to be necessary)



LipLoft_RX_V7 should be flashed to Modified Drone Remote Control.

LipSync_LipLoft_Firmware should be flashed to LipSync (with installed Bluetooth Module)









