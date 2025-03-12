/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flight1_config.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.1.0
  * @date    24-January-2025
  * @brief   FP-SNS-FLIGHT1 configuration
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLIGHT1_CONFIG_H
#define __FLIGHT1_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported define ------------------------------------------------------------*/

/***************************************
  *  Remapping instance sensor defines *
  **************************************/

#define TOF_INSTANCE               VL53L3A2_DEV_CENTER
#define TOF_RANGING_SENSOR_ADDRESS RANGING_SENSOR_VL53L3CX_ADDRESS

/*********************************
  *  Remapping APIsensor defines *
  ********************************/

#define RANGING_SENSOR_SET_POWER_MODE    VL53L3A2_RANGING_SENSOR_SetPowerMode
#define RANGING_SENSOR_INIT              VL53L3A2_RANGING_SENSOR_Init
#define RANGING_SENSOR_SET_ADDRESS       VL53L3A2_RANGING_SENSOR_SetAddress
#define RANGING_SENSOR_READ_ID           VL53L3A2_RANGING_SENSOR_ReadID
#define RANGING_SENSOR_GET_CAPABILITIES  VL53L3A2_RANGING_SENSOR_GetCapabilities
#define RANGING_SENSOR_CONFIG_PROFILE    VL53L3A2_RANGING_SENSOR_ConfigProfile
#define RANGING_SENSOR_GET_DISTANCE      VL53L3A2_RANGING_SENSOR_GetDistance
#define RANGING_SENSOR_START             VL53L3A2_RANGING_SENSOR_Start
#define RANGING_SENSOR_STOP              VL53L3A2_RANGING_SENSOR_Stop

#define POWERMODE_ON                   RANGING_SENSOR_POWERMODE_ON   /*!< Run */
#define POWERMODE_OFF                  RANGING_SENSOR_POWERMODE_OFF  /*!< Standby */

/* USER CODE BEGIN ED */

/*************** Debug Defines ******************/
#define FLIGHT1_ENABLE_PRINTF

/* For enabling connection and notification subscriptions debug */
#define FLIGHT1_DEBUG_CONNECTION

/* For enabling transmission for notified services (except for quaternions) */
#define FLIGHT1_DEBUG_NOTIFY_TRAMISSION

/* Range distances where identify the human presence */
#define PRESENCE_MIN_DISTANCE_RANGE 300
#define PRESENCE_MAX_DISTANCE_RANGE 800

/* Firmware Id */
#define CUSTOM_FIRMWARE_ID 0x02

/* USER CODE END ED */

/*************** Don't Change the following defines *************/

/* Package Version only numbers 0->9 */
#define FLIGHT1_VERSION_MAJOR '5'
#define FLIGHT1_VERSION_MINOR '1'
#define FLIGHT1_VERSION_PATCH '0'

/* Package Name */
#define FLIGHT1_PACKAGENAME "FP-SNS-FLIGHT1"

#ifdef FLIGHT1_ENABLE_PRINTF
#define FLIGHT1_PRINTF(...) printf(__VA_ARGS__)
#else /* FLIGHT1_ENABLE_PRINTF */
#define FLIGHT1_PRINTF(...)
#endif /* FLIGHT1_ENABLE_PRINTF */

#ifdef __cplusplus
}
#endif

#endif /* __FLIGHT1_CONFIG_H */
