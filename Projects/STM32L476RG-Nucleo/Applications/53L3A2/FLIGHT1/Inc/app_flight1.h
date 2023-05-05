/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_flight1.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.0
  * @date    06-March-2023
  * @brief   Header file for app_flight1.c
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
#ifndef APP_FLIGHT1_H
#define APP_FLIGHT1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_exti.h"
#include "stm32l4xx_nucleo.h"

#include "FLIGHT1_config.h"

/* Imported Variables --------------------------------------------------------*/

/* USER CODE BEGIN IV */

/* USER CODE END IV */

/* Exported Define -----------------------------------------------------------*/

/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported Variables --------------------------------------------------------*/
extern uint8_t LedEnabled;

extern uint8_t LedTimerEnabled;
extern uint8_t ObjectsDetectionTimerEnabled;

extern uint8_t NodeName[];

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported Functions --------------------------------------------------------*/
void MX_FLIGHT1_Init(void);
void MX_FLIGHT1_Process(void);

extern unsigned char ReCallNodeNameFromMemory(void);

extern void StartMeasurement(void);
extern void StopMeasurement(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* APP_FLIGHT1_H */
