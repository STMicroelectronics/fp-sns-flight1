/**
  ******************************************************************************
  * file    readme.txt
  * Version V2.0.0
  * date    21-May-2019
  ******************************************************************************
  *
  * Copyright (c) 2021 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0055, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0055
  *
  ******************************************************************************
  */

Application Description 

This firware is one Boot Loader that must run at the Flash beginning (0x08000000 address) and has the purpouse to apply the Firmware-Over-The-Air (FOTA) updated receveid, already checked and stored in Flash, or run the program normally if there is not one update.

For STM32F401RE (Nucleo) or STM32F446RE (Nucleo or BlueCoin) the 512Kbyte of Flash are splitted on 8 sectors:

|sector 0   | sector 1  | sector 2  | sector 3  | sector 4  | sector 5  | sector 6  | sector 7  |
|16K        | 16K       | 16K       | 16K       | 64K       | 128K      | 128K      | 128K      |
|0x08000000 |0x08004000 |0x08008000 |0x0800C000 |0x08010000 |0x08020000 |0x08040000 |0x08060000 |
|-----------------------------------------------------------------------------------------------|
|BootLoader | Running Program                                           |  FOTA                 |


This Boot Loader must be Loaded on Sector 0 and it checks if there is one FOTA stored on Sector 6 (0x08040000 address):
- If YES:
  - it erases the sectors from 1 to 5
  - it copies the FOTA on that sectors
  - it erases the FOTA region after the update.
- if NOT:
  - it works like a trampoline for executing the normal program stored from sector1 (0x08004000 address)

The FOTA must be less than 240Kbytes


For STM32L476RG (Nucleo or SensorTile [STEVAL-STLCS01V1]) the 1Mbytes of Flash is Divided on 2 banks of 512Kbytes each one, and each bank is splitted on 256 pages of 2Kbytes.

|Page 0->7  | Page 4->255       | Page 256->511 |
|16K        | 504K              | 512K          |
|0x08000000 |0x08004000         |0x08080000     |
|-----------------------------------------------|
|  Reg1     |       Reg2        |    Reg3       |
|BootLoader | Running Program   |    FOTA       |


This Boot Loader must be Loaded on Sector 0 and it checks if there is one FOTA stored (0x08080000 address):
- If YES:
  - it erases the necessary space on Region 2 (Reg2)
  - it copies the FOTA on that sectors
  - it erases the FOTA region (Reg3) after the update.
- if NOT:
  - it works like a trampoline for executing the normal program stored from Region 2 (0x08004000 address)

The FOTA must be less than 504Kbytes

The "Running Program" and the FOTA are compiled for running from 0x08004000 address. If they are placed at the beginning of the FLASH they doesn't work.
And without the BootLoader that programs could not be executed.

 /******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/