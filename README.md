# FP-SNS-FLIGHT1 Firmware Package

![latest tag](https://img.shields.io/github/v/tag/STMicroelectronics/fp-sns-flight1.svg?color=brightgreen)

The FP-SNS-FLIGHT1 is an STM32Cube function pack which lets you connect your IoT node to a smartphone via BLE and use a suitable Android or iOS like the ST BLE Sensor app to view real-time time-of-flight sensors data.
It uses the NDEF standard for simple and secure Bluetooth pairing, storing the necessary information on the NFC tag and thus simplifying the device configuration.
This package, together with the suggested combination of STM32 and ST devices can be used to develop specific wearable applications, or smart things applications in general.

The software runs on the STM32 microcontroller and includes all the necessary drivers to recognize the devices on the STM32 Nucleo development board and expansion boards.

The expansion is built on STM32Cube software technology to ease portability across different STM32 microcontrollers.

**FP-SNS-FLIGHT1 software features**

- Complete firmware to develop an IoT node with NFC, BLE connectivity and time-of-flight sensors

- Sample implementation available for X-NUCLEO-NFC04A1 (optional), X-NUCLEO-53L3A2 and X-NUCLEO-IDB05A2 connected to a NUCLEO-F401RE or NUCLEOL476RG

- Firmware compatible with ST BLE Sensor applications for Android/iOS, to perform sensor data reading, motion algorithm features demo and firmware update (Version 4.13.0/4.11.0 or higher)

- Easy portability across different MCU families, thanks to STM32Cube

- Free, user-friendly license terms

This firmware package includes Components Device Drivers, Board Support Package and example application for the STMicroelectronics:

- NUCLEO-F401RE nucleo development board and expansion boards:
  - X-NUCLEO-IDB05A2 Bluetooth Low energy expansion boards
  - X-NUCLEO-NFC04A1 NFC/RFID tag IC expansion board based on ST25DV04K (optional)
  - X-NUCLEO-53L3A2 ranging sensor with multi object detection and FoV (field of view) programming.
  
- NUCLEO-L476RG nucleo development board and expansion boards:
  - X-NUCLEO-IDB05A2 Bluetooth Low energy expansion boards
  - X-NUCLEO-NFC04A1 NFC/RFID tag IC expansion board based on ST25DV04K (optional)
  - X-NUCLEO-53L3A2 ranging sensor with multi object detection and FoV (field of view) programming.

[![The FP-SNS-FLIGHT1 package contents](_htmresc/FP-SNS-FLIGHT1_Software_Architecture.png)]()

Here is the list of references to user documents:

- [DB2862: STM32Cube function pack for IoT node with NFC, BLE connectivity and Time-of-Flight sensors](https://www.st.com/resource/en/data_brief/fp-sns-flight1.pdf)
- [UM2026: Getting started with the STM32Cube function pack for IoT node with NFC, BLE connectivity and time-of-flight sensors](https://www.st.com/resource/en/user_manual/um2026-getting-started-with-the-stm32cube-function-pack-for-iot-node-with-nfc-ble-connectivity-and-timeofflight-sensors-stmicroelectronics.pdf)
- [FP-SNS-FLIGHT1 Quick Start Guide](https://www.st.com/content/ccc/resource/sales_and_marketing/presentation/product_presentation/group0/80/95/e3/1e/fa/47/49/0d/FP-SNS-FLIGHT1%20Quick%20start%20guide/files/fp-sns-flight1_quick_start_guide.pdf/jcr:content/translations/en.fp-sns-flight1_quick_start_guide.pdf)

## Supported Devices and Boards

- NUCLEO-F401RE STM32 Nucleo-64 development board with STM32F401RE MCU, supports Arduino and ST morpho connectivity \[[NUCLEO-F401RE](https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-f401re.html)\]
- NUCLEO-L476RG STM32 Nucleo-64 development board with STM32L476RG MCU, supports Arduino and ST morpho connectivity \[[NUCLEO-L476RG](https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-l476rg.html)\]
- X-NUCLEO-IDB05A2 Bluetooth Low Energy expansion board based on the BLUENRG-M2SP module for STM32 Nucleo \[[X-NUCLEO-IDB05A2](https://www.st.com/en/ecosystems/x-nucleo-idb05a2.html)]
- X-NUCLEO-NFC04A1 Motion MEMS and environmental sensor expansion board for STM32 Nucleo \[[X-NUCLEO-NFC04A1](https://www.st.com/en/ecosystems/x-nucleo-nfc04a1.html)\]
- X-NUCLEO-53L3A2 Motion MEMS and environmental sensor expansion board for STM32 Nucleo \[[X-NUCLEO-53L3A2](https://www.st.com/en/evaluation-tools/x-nucleo-53l3a2.html)\]

## Known Issues

After pairing with STBLESensor application using NFC reader, after reset for pairing again the board with STBLESensor application without using NFC reader could be necessary restart
the mobile phone before the connection.

## Development Toolchains and Compilers

-   IAR Embedded Workbench for ARM (EWARM) toolchain V9.20.1 + STLink/V2
-   RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.32.0 + ST-LINK/V2
-   Integrated Development Environment for STM32 (STM32CubeIDE) V1.8.0 + ST-LINK
	
## Dependencies 

This software release is compatible with:

- [**ST BLE Sensor Android application**](https://play.google.com/store/apps/details?id=com.st.bluems)  V4.13.0 (or higher)
- [**ST BLE Sensor iOS application**](https://apps.apple.com/it/app/st-ble-sensor/id993670214)  V4.13.0 (or higher)
