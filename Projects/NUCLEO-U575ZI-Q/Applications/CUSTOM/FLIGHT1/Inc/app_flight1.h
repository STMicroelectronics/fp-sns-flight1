/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_flight1.h
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   Header file for app_flight1.c
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
#ifndef APP_FLIGHT1_H
#define APP_FLIGHT1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_exti.h"
#include "stm32u5xx_nucleo.h"

#include "flight1_config.h"

/* Imported Variables --------------------------------------------------------*/

/* USER CODE BEGIN IV */

/* USER CODE END IV */

/* Exported Defines ----------------------------------------------------------*/
#define STM32U5xx

/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported Variables --------------------------------------------------------*/
extern uint8_t LedEnabled;

extern uint8_t LedTimerEnabled;
extern uint8_t ObjectsDetectionTimerEnabled;

extern uint32_t CurrentActiveBank;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported Functions --------------------------------------------------------*/
void MX_FLIGHT1_Init(void);
void MX_FLIGHT1_Process(void);

/* Update Node Name on to Meta Data */
extern void UpdateNodeNameMetaData(void);
/* User function for Erasing the Flash data */
extern uint32_t EraseMetaData(void);
/* User function for Saving the Meta Data on the Flash */
extern void SaveMetaData(void);

extern unsigned char ReCallNodeNameFromMemory(void);

extern void StartMeasurement(void);
extern void StopMeasurement(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* APP_FLIGHT1_H */
