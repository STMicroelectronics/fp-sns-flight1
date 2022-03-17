/**
  ******************************************************************************
  * @file    FLIGHT1_config.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   FP-SNS-FLIGHT1 configuration
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
#ifndef __FLIGHT1_CONFIG_H
#define __FLIGHT1_CONFIG_H

/* Exported define ------------------------------------------------------------*/
/* Uncomment the following define for accepting only authorized connection */
#define FLIGHT1_SECURE_CONNECTION

/*************** Debug Defines ******************/
#define FLIGHT1_ENABLE_PRINTF

/* For enabling connection and notification subscriptions debug */
#define FLIGHT1_DEBUG_CONNECTION

/* For enabling transmission for notified services (except for quaternions) */
#define FLIGHT1_DEBUG_NOTIFY_TRAMISSION

/* Range distances where identify the human presence */
#define PRESENCE_MIN_DISTANCE_RANGE 300
#define PRESENCE_MAX_DISTANCE_RANGE 800

/*************** Don't Change the following defines *************/

/* Package Version only numbers 0->9 */
#define FLIGHT1_VERSION_MAJOR '4'
#define FLIGHT1_VERSION_MINOR '2'
#define FLIGHT1_VERSION_PATCH '0'

/* Define the FLIGHT1 Name MUST be 7 char long */
#define NAME_BLUEMS 'F','L','1','V',FLIGHT1_VERSION_MAJOR,FLIGHT1_VERSION_MINOR,FLIGHT1_VERSION_PATCH

/* Package Name */
#define FLIGHT1_PACKAGENAME "FP-SNS-FLIGHT1"

#ifdef FLIGHT1_ENABLE_PRINTF
  #define FLIGHT1_PRINTF(...) printf(__VA_ARGS__)
#else /* FLIGHT1_ENABLE_PRINTF */
  #define FLIGHT1_PRINTF(...)
#endif /* FLIGHT1_ENABLE_PRINTF */

/* STM32 Unique ID */
#ifdef USE_STM32F4XX_NUCLEO
#define STM32_UUID ((uint32_t *)0x1FFF7A10)
#endif /* USE_STM32F4XX_NUCLEO */

#ifdef USE_STM32L4XX_NUCLEO
#define STM32_UUID ((uint32_t *)0x1FFF7590)
#endif /* USE_STM32L4XX_NUCLEO */

#ifdef USE_STM32L0XX_NUCLEO
#define STM32_UUID ((uint32_t *)0x1FF80050)
#endif /* USE_STM32L0XX_NUCLEO */

/* STM32 MCU_ID */
#define STM32_MCU_ID ((uint32_t *)0xE0042000)

#endif /* __FLIGHT1_CONFIG_H */
