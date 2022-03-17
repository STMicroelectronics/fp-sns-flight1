# FLIGHT1 Application Description for STM32F401RE

The FP-SNS-FLIGHT1 is an STM32Cube function pack which lets you connect your IoT node to a smartphone via BLE and use a suitable Android or iOS like the ST BLE Sensor app to view real-time time-of-flight sensors data.
It uses the NDEF standard for simple and secure Bluetooth pairing, storing the necessary information on the NFC tag and thus simplifying the device configuration.
This package, together with the suggested combination of STM32 and ST devices can be used to develop specific wearable applications, or smart things applications in general.
The software runs on the STM32 microcontroller and includes all the necessary drivers to recognize the devices on the STM32 Nucleo development board and expansion boards.


This firmware package includes Components Device Drivers, Board Support Package and example application for the following STMicroelectronics elements:
 - X-NUCLEO-IDB05A2 Bluetooth Low energy expansion boards 
 - X-NUCLEO-NFC04A1 NFC/RFID tag IC expansion board based on ST25DV04K (optional)
 - X-NUCLEO-53L3A2  Multi-targets ranging sensor expansion board based on VL53L3CX for STM32 Nucleo.
 - NUCLEO-F401RE    Nucleo boards

The Example application initizializes all the Components and Library creating 3 Custom Bluetooth services:
 - The first service exposes all the HW characteristics related to time-of-flight sensors data and Led status.
 - The second service exposes the console services where we have stdin/stdout and stderr capabilities
 - The last Service is used for configuration purpose

The example application allows the user to control the initialization phase via UART.
Launch a terminal application and set the UART port to 115200 bps, 8 bit, No Parity, 1 stop bit.
 
This example must be used with the related ST BLE Sensor Android (Version 4.13.0 or higher) or iOS (Version 4.11.0 or higher) application available on Play/itune store,
in order to read the sent information by Bluetooth Low Energy protocol

## Important Hardware Additional Information

There is an hardware conflict between the X-NUCLEO-NFC04A1 expansion board and the X-NUCLEO-IDB05A2 expansion board through the Arduino UNO R3 extension connector.
The hardware conflict is onto pin 5 of the CN5 arduido connector and onto pin 8 of the CN9 arduido connector.
For X-NUCLEO-NFC04A1:
 - In the pin 5 of the CN5 arduido connector resistor R1  for ST25DV GPO
 - In the pin 8 of the CN9 arduido connector resistor R11 ST25DV LPD
For X-NUCLEO-IDB05A2:
 - In the pin 5 of the CN5 arduido connector MISO_SPI for BLE
 - In the pin 8 of the CN9 arduido connector Reset Line for BLE
For this reason, before to connect X-NUCLEO-NFC04A1 expansion board with X-NUCLEO-IDB05A2 expansion board through the Arduino UNO R3 extension connector,
remove the resistors R1 and R11 onto X-NUCLEO-NFC04A1 board.

## Very Important

1) The Firmware-Over-The-Air (FOTA) is done using the ST BLE Sensor Android/iOS application (Version 4.13.0 and above)
 
2) This example must run starting at address 0x08004000 in memory and works ONLY if the BootLoader is saved at the beginning of the FLASH (address 0x08000000)
 
3) For each IDE (IAR/µVision/STM32CubeIDE) there are some scripts *.bat/*.sh that makes the following operations:
    - Full Flash Erase
    - Load the BootLoader on the rigth flash region
    - Load the Program (after the compilation) on the rigth flash region (This could be used for a FOTA)
    - Dump back one single binary that contain BootLoader+Program that could be 
      flashed at the flash beginning (address 0x08000000) (This COULD BE NOT used for FOTA)
    - Reset the board
	
## Known Issues

After pairing with STBLESensor application using NFC reader, after reset for pairing again the board with STBLESensor application without using NFC reader
could be necessary restart the bluetooth of the mobile phone before the connection.

## Dependencies

STM32Cube packages:
  - STM32F4xx drivers from STM32CubeF4 V1.27.0
  
X-CUBE packages:
  - X-CUBE-BLE1 V6.2.2
  - X-CUBE-TOF1 V3.0.0
  - X-CUBE-NFC4 V2.0.3

## Hardware and Software environment

- This example runs on Sensor expansion board attached to STM32F401RE devices can be easily tailored to any other supported device and development board.
- This example must be used with the related ST BLE Sensor Android/iOS application (Version 4.13.0/4.11.0 or higher) available on Play/itune store, in order to read the sent information by Bluetooth Low Energy protocol.
- Inside the Binary Directory there are the following binaries:
  - STM32F401RE-Nucleo_FLIGHT1_v4.2.0.bin    (Program without BootLoader. COULD BE USED     for FOTA)
  - STM32F401RE-Nucleo_FLIGHT1_BL_v4.2.0.bin (Program with BootLoader.    COULD NOT BE USED for FOTA)

## How to use it ?

This package contains projects for 3 IDEs viz. IAR, Keil µVision 5 and Integrated Development Environment for STM32. 
In order to make the  program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.

For IAR:
 - Open IAR toolchain (this firmware has been successfully tested with Embedded Workbench V9.20.1).
 - Open the IAR project file EWARM\STM32F401RE-Nucleo_FLIGHT1.eww
 - Rebuild all files and run these script that you find on the same directory:
   - CleanFLIGHT1_IAR_F401.bat

For Keil µVision 5:
 - Open Keil µVision 5 toolchain (this firmware has been successfully tested with MDK-ARM Professional Version: 5.32.0).
 - Open the Keil µVision 5 project file MDK-ARM\Project.uvprojx
 - Rebuild all files and run these script that you find on the same directory:
   - CleanFLIGHT1_MDK-ARM_F401.bat
 
For Integrated Development Environment for STM32:
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.8.0).
 - Set the default workspace proposed by the IDE (please be sure that there are not spaces in the workspace path).
 - Press "File" -> "Import" -> "Existing Projects into Workspace"; press "Browse" in the "Select root directory" and choose the path where the System
   Workbench project is located (it should be STM32CubeIDE). 
 - Rebuild all files and and run these script that you find on the same directory:
   - if you are on windows and you had installed the STM32 ST-Link utility:
     - CleanFLIGHT1_STM32CubeIDE_F401.bat
   - Otherwise (Linux/iOS or Windows without the STM32 ST-Link Utility):
     - CleanFLIGHT1_STM32CubeIDE_F401.sh
