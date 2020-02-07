# Liploft

This project was created to enable a person with limited hand function, such as person with quadraplegia, to operate a radio frequency (RF) remote control drone. The project is based on a [LipSync Wireless](https://github.com/makersmakingchange/LipSync-Wireless), an open-source, mouth operated joystick. The user moves the joystick with their lips and mouth and uses sips and puffs of air on the joystick to interact with the drone remote control.

This GitHub repository will serve as a database for all files and documentation associated with the Liploft project. 

This project is a **work in progress**: for more information, or to get involved, visit the Makers Making Change forum thread: https://forum.makersmakingchange.com/t/lipsync-drone-control/196

## Table of Contents
- [Overview](#overview)
  * [Approach](#approach)
- [Download](#download)
- [Installation and Setup](#installation-and-setup)
  * [LipSync Wireless](#lipsync-wireless)
  * [Liploft Remote Control Interface](#liploft-remote-control-interface)
  * [Software](#software)
- [Operation](#operation)
  * [Pairing](#pairing)
  * [Controlling the Drone with LipSync](#controlling-the-drone-with-lipsync)
- [Contact Us](#contact-us)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>
  
## Overview
Liploft requires the following components:

1. A drone
2. The drone's remote control, modified to connect with the LipLoft interface.
3. A [Lipsync Wireless](https://github.com/makersmakingchange/LipSync-Wireless) with LipLoft firmware.

### Approach
The Liploft interface is comprised of a microcontroller, a Bluetooth module, and 4 digital potentiometers. The drone's remote controls is modified by removing the two physical joysticks and connecting the digital potetiometers to create virtual, software controllable joysticks. The Liploft acts as a Bluetooth master, connecting to the LipSync Wireless which acts as a Bluetooth slave. Physical movement of and and sips and puffs through the mouthpiece are digitized, transmitted to the Liploft interface and converted into virtual joystick signals.


## Download

 <table style="width:100%">
  <tr>
    <th>Resource</th>
    <th>Version</th>
    <th>Format</th>
    <th>Link</th>
  </tr>
    <tr>
    <td>Liploft All</td>
    <td>Beta 8</td>
    <td>ZIP</td>
    <td><a href="https://github.com/makersmakingchange/LipLoft/archive/master.zip">LipLoft-master.zip</a></td>
  </tr>
  <tr>
    <td>Liploft Manual</td>
    <td>Version 1.0</td>
    <td>PDF</td>
    <td><a href="https://github.com/makersmakingchange/LipLoft/raw/master/LipLoft_User_Manual.pdf">LipLoft_User_Manual.pdf</a></td>
  </tr>
  <tr>
    <td>Liploft BOM (XLSX)</td>
    <td></td>
    <td>XLSX</td>
    <td><a href="https://github.com/makersmakingchange/LipLoft/raw/master/LipLoft_BOM.xlsx">Liploft_BOM.xlsx</a></td>
  </tr>
  <tr>
    <td>Liploft BOM (CSV)</td>
    <td></td>
    <td>CSV</td>
    <td><a href="https://github.com/makersmakingchange/LipLoft/blob/master/LipLoft_BOM.csv" download target="_blank">Liploft_BOM.csv</a></td>
  </tr>
  <tr>
    <td>Lipsync Liploft Firmware</td>
    <td>1.1</td>
    <td>INO</td>
    <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Software/LipSync_Liploft_Firmware/LipSync_Liploft_Firmware.ino">LipSync_Liploft_Firmware.ino</a></td>
  </tr>
  <tr>
    <td>Liploft Interface Firmware</td>
    <td>1.9</td>
    <td>INO</td>
    <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Software/Liploft_Interface_Firmware/Liploft_Interface_Firmware.ino">Liploft_Interface_Firmware.ino</a></td>
  </tr>
  <tr>
    <td>Lipsync Manual Bluetooth Config</td>
    <td></td>
    <td>INO</td>
    <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Software/Liploft_Bluetooth_Config/Lipsync_Bluetooth_Config/Lipsync_Bluetooth_Config.ino">Lipsync_Bluetooth_Config.ino</a></td>
  </tr>
  <tr>
    <td>Interface Manual Bluetooth Config</td>
    <td></td>
    <td>INO</td>
    <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Software/Liploft_Bluetooth_Config/Interface_Bluetooth_Config/Interface_Bluetooth_Config.ino">Interface_Bluetooth_Config.ino</a></td>
  </tr>
  <tr>
    <td>Liploft Fritzing Board Layout</td>
    <td>Beta 8</td>
    <td>BRD</td>
    <td><a href="https://raw.githubusercontent.com/makersmakingchange/Liploft/master/Hardware/Electronics/Fritzing/Liploft_Breadboard.fzz">Liploft_Breadboard.fzz</a></td>
  </tr>
  <tr>
    <td>Liploft Interface Board Schematic</td>
    <td>Beta 8</td>
    <td>SCH</td>
    <td><a href="https://raw.githubusercontent.com/makersmakingchange/Liploft/master/Hardware/Electronics/Eagle/Liploft_Schematics.sch">Liploft_Schematics.sch</a></td>
  </tr>
  <tr>
    <td rowspan="3">Liploft Housing_design</td>
    <td rowspan="3">V5</td>
    <td rowspan="3">X3G,STL</td>
    <td><a href="https://raw.githubusercontent.com/makersmakingchange/Liploft/master/Hardware/Housing_design/LipSyncDrone/Don_Encl_v5_ABS_L.x3g">Don_Encl_v5_ABS_L.x3g</a></td>
  </tr>
   <tr>
  <td><a href="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Housing_design/Liploft_Top.stl">Liploft_Top.STL</a></td>
  </tr>
 <tr>
  <td><a href="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Housing_design/Liploft_Bottom.stl">Liploft_Bottom.STL</a></td>
</tr>
 
</tr>
  <tr>
    <td rowspan="8">Lipsync Housing_design</td>
    <td rowspan="8">2.2d</td>
    <td rowspan="8">STL</td>
    <td><a href="https://raw.githubusercontent.com/makersmakingchange/Liploft/master/Hardware/Housing_design/LipSync_Button.STL">LipSync_Button.STL</a></td>
  </tr>
  <tr>
  <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Hardware/Housing_design/LipSync_Front_Housing.STL">LipSync_Front_Housing.STL</a></td>
  </tr>
  <tr>
  <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Hardware/Housing_design/LipSync_Joystick_Base.STL">LipSync_Joystick_Base.STL</a></td>
</tr>
<tr>
  <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Hardware/Housing_design/LipSync_Joystick_Front.STL">LipSync_Joystick_Front.STL</a></td>
</tr>
<tr>
  <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Hardware/Housing_design/LipSync_Joystick_Rocker.STL">LipSync_Joystick_Rocker.STL</a></td>
</tr>
<tr>
    <td><a href="https://github.com/makersmakingchange/Liploft/raw/master/Hardware/Housing_design/LipSync_Joystick_Stand.STL">LipSync_Joystick_Stand.STL</a></td>
</tr>
 <tr>
  <td><a href="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Housing_design/LipSync_Rear_Housing.stl">LipSync_Rear_Housing.STL</a></td>
</tr>
<tr>
  <td><a href="https://raw.githubusercontent.com/makersmakingchange/Liploft/master/Hardware/Housing_design/Print_Settings.txt">Print_Settings.txt</a></td>
</tr>
</table> 

## Installation and Setup


### LipSync Wireless
1. Obtain a [Lipsync Wireless](https://github.com/makersmakingchange/LipSync-Wireless). There are several options for getting a Lipsync Wireless:
 * Build your own. All the necessary files, software, and assembly guides are available in the [Lipsync Wireless GitHub repository](https://github.com/makersmakingchange/LipSync-Wireless). 
 * Submit a build request through the [Makers Making Change website](https://www.makersmakingchange.com/project/lipsyncwireless/). MMC will attempt to pair you with a local volunteer maker to help build and program one. You'll be responsible for covering the cost of materials and shipping. Depending on your location, there may also be pre-built donated units available at little or no cost.
 * [Order a built LipSync](https://www.makersmakingchange.com/order-built-lipsync/) from the Makers Making Change website.
 
 2. It may be helpful to modify the rear housing of the LipSync Wireless so the LED on the Bluetooth module is visible.

### Liploft Remote Control Interface
The remote control interface requires some permanent modifications to the drone's remote control and the addition of several hardware components.

* Setup the circuit in breadboard according to the following diagram:

<p align="center">
<img align="center" src="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Electronics/Liploft_Breadboard_Picture.png" width="50%" height="50%" alt="Liploft Breadboard Picture"/>
</p>

* Setup the Bluetooth circuit in breadboard according to the following diagram:
  * Bluetooth module RTS pin to Bluetooth module CTS
  * Bluetooth module RX pin to Arduino TX pin 
  * Bluetooth module TX pin to Arduino RX pin 
  * Bluetooth module VCC pin to Arduino 5V pin 
  * Bluetooth module GND pin to Arduino GND pin 
  
  <p align="center">
<img align="center" src="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Electronics/Liploft_Bluetooth_Connection.png" width="50%" height="50%" alt="Liploft Bluetooth Connection"/>
</p>


* Setup the rest of circuit in breadboard according to the following diagram:
  * MCP4261 IC1 pin 1 to Arduino pin 10
  * MCP4261 IC2 pin 1 to Arduino pin 9
  * MCP4261 IC3 pin 1 to Arduino pin 8
  * MCP4261 IC4 pin 1 to Arduino pin 7
  * Led pin to Arduino pin 5
  * MCP4261 IC1,IC2,IC3,IC4 pin 2 to Arduino pin SCK
  * MCP4261 IC1,IC2,IC3,IC4 pin 3 to Arduino pin MOSI
  * MCP4261 IC1,IC2,IC3,IC4 pin 4 to Arduino pin GND
  * MCP4261 IC1,IC2,IC3,IC4 pin 5 to Arduino pin GND
  * MCP4261 IC1,IC2,IC3,IC4 pin 6 to channels of transmiter (potemtiometer) and same MCP4261 pin 7
  * MCP4261 IC1,IC2,IC3,IC4 pin 7 to 5.6K resistor and Arduino pin 5V in pullup configuration 
  * MCP4261 IC1,IC2,IC3,IC4 pin 8 to Arduino pin 5V
  
   <p align="center">
<img align="center" src="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Electronics/Liploft_Schematics.PNG" width="50%" height="70%" alt="Liploft Schematics"/>
</p>

* Setup the circuit for controller connection according to the following example:

<p align="center">
<img align="center" src="https://github.com/makersmakingchange/LipLoft/raw/master/Hardware/Electronics/Controller_Connection.jpeg" width="50%" height="50%" alt="Liploft Controller Connection Picture"/>
</p>

### Software
The Lipsync Wireless and Liploft interface require firmware.

*	Download the necessary files from the LipLoft GitHub repository.
  1. Visit LipLoft GitHub repository at https://github.com/makersmakingchange/LipLoft
  2. Click on “Clone or download” button” in green.
  3. Click on “Download Zip” to download the necessary file.
  4. Extract “LipLoft-master.zip” to the directory of your choice.
*	Download and install MCP4261 Arduino library 
  1. Visit MCP4261 library GitHub repository at https://github.com/dreamcat4/Mcp4261
  2. Click on “Clone or download” button” in green.
  3. Click on “Download Zip” to download the necessary file.
  4. Extract “Mcp4261-master.zip” to the directory of your choice.
  5. Rename “Mcp4261-master.zip” to “Mcp4261.zip”
  6. Open Arduino IDE
  7. Click on Sketch > Include Library > Add .zip Library
  8. Select “Mcp4261.zip”
 *	Upload Lipsync firmware 
  1. Connect the Lipsync Wireless to the computer using the USB cable.
  2. Verify and upload "LipSync_Liploft_Firmware.ino" to the Lipsync Wireless unit using Arduino IDE
  * Upload Liploft firmware
  1. Connect the Liploft interface to the computer using a USB cable.
  2. Verify and upload "Liploft_Interface_Firmware.ino" to the Liploft interface unit using Arduino IDE

  
## Operation

### Pairing
  1. Connect power to the LipSync Wireless by plugging in the USB cable to a power pack or other power source.
  2. Wait for 5 seconds. (You need to power the LipSync (Slave Bluetooth module) 5 seconds before the LipLoft unit (Master Bluetooth module) if you are connecting the LipSync to LipLoft for the first time.)
  3. Connect power to the LipLoft unit. The red LED on Bluetooth modules should start blinking faster and the green led on both Bluetooth modules should stay on to indicate connection is established between Lipsync and Liploft.
  
If the connection is not established, you can use code to manually configure the Bluetooth modules. The Bluetooth module in the Lipsync Wirelss can be reconfigured using "Lipsync_Bluetooth_Config.ino" code and the Bluetooth module in the Liploft interface unit can be reconfigured using "Interface_Bluetooth_Config.ino" code. You will need to upload both codes at the same time. The green led on both Bluetooth modules will stay on once the connection is established.

### Controlling the Drone with LipSync
* Arm the drone by puffing and sipping once (the LED on the Liploft Interface will turn on to indicate it is armed.)
* sip/puff controls the throttle
* LipSync joystick controls pitch and roll
* Gyro calibration is automated

 
## Contact Us

For technical questions, to get involved or share your LipSync experience we encourage you to visit the [forum thread](https://forum.makersmakingchange.com/t/lipsync-drone-control/196) for this project or contact info@makersmakingchange.com

