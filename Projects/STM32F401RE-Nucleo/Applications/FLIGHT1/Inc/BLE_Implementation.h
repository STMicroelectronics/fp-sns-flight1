/**
  ******************************************************************************
  * @file    BLE_Implementation.h 
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.h.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/  
#ifndef _BLE_IMPLEMENTATION_H_
#define _BLE_IMPLEMENTATION_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/

/**
* User can added here the header file for the selected BLE features.
* For example:
* #include "BLE_Environmental.h"
* #include "BLE_Inertial.h"
*/

#include "BLE_Led.h" 
#include "BLE_ObjectsDetection.h" 

/* Exported Defines --------------------------------------------------------*/
     
/* Select the used hardware platform
 *
 * STEVAL-WESU1                         --> BLE_MANAGER_STEVAL_WESU1_PLATFORM
 * STEVAL-STLKT01V1 (SensorTile)        --> BLE_MANAGER_SENSOR_TILE_PLATFORM
 * STEVAL-BCNKT01V1 (BlueCoin)          --> BLE_MANAGER_BLUE_COIN_PLATFORM
 * STEVAL-IDB008Vx                      --> BLE_MANAGER_STEVAL_IDB008VX_PLATFORM
 * STEVAL-BCN002V1B (BlueTile)          --> BLE_MANAGER_STEVAL_BCN002V1_PLATFORM
 * STEVAL-MKSBOX1V1 (SensorTile.box)    --> BLE_MANAGER_SENSOR_TILE_BOX_PLATFORM
 * DISCOVERY-IOT01A                     --> BLE_MANAGER_DISCOVERY_IOT01A_PLATFORM                                   
 * STEVAL-STWINKT1                      --> BLE_MANAGER_STEVAL_STWINKT1_PLATFORM
 * STEVAL-STWINKT1B                     --> BLE_MANAGER_STEVAL_STWINKT1B_PLATFORM
 * STEVAL-STWINBX1                      --> BLE_MANAGER_STEVAL_STWINBX1_PLATFORM
 *                                      --> BLE_MANAGER_SENSOR_TILE_BOX_PRO_PLATFORM
 * STEVAL-ASTRA1                        --> BLE_MANAGER_STEVAL_ASTRA1_PLATFORM
 * STM32NUCLEO Board                    --> BLE_MANAGER_NUCLEO_PLATFORM
 * STM32F446RE-NUCLEO                   --> BLE_MANAGER_STM32F446RE_NUCLEO_PLATFORM
 * STM32L053R8-NUCLEO                   --> BLE_MANAGER_STM32L053R8_NUCLEO_PLATFORM
 * STM32L476RG-NUCLEO                   --> BLE_MANAGER_STM32L476RG_NUCLEO_PLATFORM
 * STM32F401RE-NUCLEO                   --> BLE_MANAGER_STM32F401RE_NUCLEO_PLATFORM
 * UNDEF_PLATFORM                       --> UNDEF_PLATFORM
 *
 * For example:
 * #define BLE_MANAGER_USED_PLATFORM	BLE_MANAGER_NUCLEO_PLATFORM
 *
*/

/* Used platform */
#define BLE_MANAGER_USED_PLATFORM       BLE_MANAGER_STM32F401RE_NUCLEO_PLATFORM

/* STM32 Unique ID */
#define BLE_STM32_UUID STM32_UUID

/* STM32  Microcontrolles type */
#define BLE_STM32_MICRO "F401"

/* STM32 board type*/
#define BLE_STM32_BOARD "STM32F401RE-NUCLEO"

/* Package Version firmware */
#define BLE_VERSION_FW_MAJOR    FLIGHT1_VERSION_MAJOR
#define BLE_VERSION_FW_MINOR    FLIGHT1_VERSION_MINOR
#define BLE_VERSION_FW_PATCH    FLIGHT1_VERSION_PATCH

/* Firmware Package Name */
#define BLE_FW_PACKAGENAME      FLIGHT1_PACKAGENAME
   
/* Feature mask for LED */
#define FEATURE_MASK_LED 0x20000000
   
/* Exported Variables ------------------------------------------------------- */
extern uint8_t connected;
extern int MasterTrusted;
extern int32_t  NeedToClearSecureDB;

/* Exported functions ------------------------------------------------------- */
extern void BLE_InitCustomService(void);
extern void BLE_SetCustomAdvertiseData(uint8_t *manuf_data);
extern void BluetoothInit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_IMPLEMENTATION_H_ */

