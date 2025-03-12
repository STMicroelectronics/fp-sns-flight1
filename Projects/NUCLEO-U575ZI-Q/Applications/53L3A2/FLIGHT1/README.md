## <b>FLIGHT1 Application Description</b>

Application for NUCLEO-U575ZI-Q with X-NUCLEO-BNRG2A1 and X-NUCLEO-53L3A2 expansion boards.

Example Description:

The FP-SNS-FLIGHT1 is an STM32Cube function pack which lets you connect your IoT node to a smartphone via BLE and use a suitable Android or iOS like the ST BLE Sensor app to view real-time time-of-flight sensors data.
This package, together with the suggested combination of STM32 and ST devices can be used to develop specific wearable applications, or smart things applications in general.
The software runs on the STM32 microcontroller and includes all the necessary drivers to recognize the devices on the STM32 Nucleo development board and expansion boards.
The Example application initizializes all the Components and Library creating 3 Custom Bluetooth services:

 - The first service exposes all the HW characteristics related to time-of-flight sensors data and Led status.
 - The second service exposes the console services where we have stdin/stdout and stderr capabilities
 - The last Service is used for configuration purpose

The example application allows the user to control the initialization phase via UART.
Launch a terminal application and set the UART port to 115200 bps, 8 bit, No Parity, 1 stop bit.
 
This example must be used with the related ST BLE Sensor Android (Version 5.2.0 or higher) or iOS (Version 5.2.0 or higher) application available on Play/itune store,
in order to read the sent information by Bluetooth Low Energy protocol

### <b>Important Hardware Additional Information</b>

BlueNRG-2 library does not work with the stock firmware that is loaded in the BLE module of X-NUCLEO-BNRG2A1 expansion board.
For this reason:

- first of all, it is needed to solder on X-NUCLEO-BNRG2A1, if it is not soldered, a 0 Ohm resistor at R117
- then you can use a standard ST-Link V2-1 with 5 jumper wires female-female together with STSW-BNRGFLASHER software tool
  (currently available only for Windows PC) in order to update the firmware of the BLE module of X-NUCLEO-BNRG2A1.
   
Read user manual for more details.

### <b>Very Important</b>

- After code generation from STM32CubeMX tool before building for Keil, open the "Options for Target" and in the tab "Target" check the box "Use MicroLib".
- After code generation from STM32CubeMX tool before building for all IDE, set as ROM size 0x080FDFFF

### <b>Keywords</b>

BLE, BLE_Manager, BlueNRG-2, SPI, I2C, UART, ToF

### <b>Hardware and Software environment</b>

  - This example runs on STM32 Nucleo devices with:
    - BlueNRG-2 STM32 expansion board (X-NUCLEO-BNRG2A1)
	- Multi-targets ranging sensor expansion board X-NUCLEO-53L3A2 based on VL53L3CX for STM32 Nucleo.
  - This example has been tested with STMicroelectronics:
    - NUCLEO-U575ZI-Q board
	
ADDITIONAL_BOARD : X-NUCLEO-BNRG2A1 https://www.st.com/en/ecosystems/x-nucleo-bnrg2a1.html
ADDITIONAL_COMP : BlueNRG-M2SP https://www.st.com/en/wireless-connectivity/bluenrg-2.html

ADDITIONAL_BOARD : X-NUCLEO-53L3A2 https://www.st.com/en/evaluation-tools/x-nucleo-53l3a2.html
ADDITIONAL_COMP : VL53L3CX https://www.st.com/en/imaging-and-photonics-solutions/vl53l3cx.html

### <b>Known Limitations</b>

FLIGHT1 application compiled with STM32CubeIDE doesn't work with BootLoader example compiled with MDK-ARM IDE.

### <b>Dependencies</b>

STM32Cube packages:

  - STM32U5xx drivers from STM32CubeU5 V1.7.0
  
X-CUBE packages:

  - X-CUBE-BLE2 V3.3.0
  - X-CUBE-BLEMGR V2.0.0
  - X-CUBE-TOF1 V3.4.2

### <b>How to use it?</b>

This package contains projects for 3 IDEs viz. IAR, Keil µVision 5 and Integrated Development Environment for STM32. 
In order to make the  program work, you must do the following:

 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.

For IAR:

 - Open IAR toolchain (this firmware has been successfully tested with Embedded Workbench V9.20.1).
 - Open the IAR project file EWARM/Project.eww
 - Rebuild all files and load your image into target memory.
 - Run the example.

For Keil µVision 5:

 - Open Keil µVision 5 toolchain (this firmware has been successfully tested with MDK-ARM Professional Version: 5.38.0).
 - Open the µVision project file MDK-ARM/FLIGHT1.uvprojx
 - Rebuild all files and load your image into target memory.
 - Run the example.
 
For Integrated Development Environment for STM32:

 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.17.0).
 - Set the default workspace proposed by the IDE (please be sure that there are not spaces in the workspace path).
 - Press "File" -> "Import" -> "Existing Projects into Workspace"; press "Browse" in the "Select root directory" and choose the path where the System
   Workbench project is located (it should be STM32CubeIDE). 
 - Rebuild all files and load your image into target memory.
 - Run the example.

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2025 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.