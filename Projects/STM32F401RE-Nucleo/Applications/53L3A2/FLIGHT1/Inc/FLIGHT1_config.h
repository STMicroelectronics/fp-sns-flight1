/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLIGHT1_config.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.0
  * @date    06-March-2023
  * @brief   FP-SNS-FLIGHT1 configuration
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Exported define ------------------------------------------------------------*/
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

/* USER CODE END ED */

/*************** Don't Change the following defines *************/

/* Package Version only numbers 0->9 */
#define FLIGHT1_VERSION_MAJOR '5'
#define FLIGHT1_VERSION_MINOR '0'
#define FLIGHT1_VERSION_PATCH '0'

/* Package Name */
#define FLIGHT1_PACKAGENAME "FP-SNS-FLIGHT1"

#ifdef FLIGHT1_ENABLE_PRINTF
  #define FLIGHT1_PRINTF(...) printf(__VA_ARGS__)
#else /* FLIGHT1_ENABLE_PRINTF */
  #define FLIGHT1_PRINTF(...)
#endif /* FLIGHT1_ENABLE_PRINTF */

#endif /* __FLIGHT1_CONFIG_H */
