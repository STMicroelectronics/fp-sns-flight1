/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    TargetFeatures.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.0
  * @date    06-March-2023
  * @brief   Specification of the HW Features for each target platform
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
#ifndef _TARGET_FEATURES_H_
#define _TARGET_FEATURES_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"
#include "stm32f4xx_hal_conf.h"

#include "53l3a2_ranging_sensor.h"

#include "FLIGHT1_config.h"

#include "MetaDataManager.h"

/* Imported Variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

/* USER CODE BEGIN IV */

/* USER CODE END IV */

/* Exported defines ------------------------------------------------------- */
/* Exported defines for TIMER Led Blinking ------------------------------------*/
#define TimLedHandle htim2
#define Led_TIM_INSTANCE TIM2
/* Exported defines for TIMER ToF ---------------------------------------------*/
#define TimDistanceHandle htim3
#define ToF_TIM_INSTANCE TIM3

#ifndef ALGO_PERIOD_LED
  #define ALGO_PERIOD_LED 1000
#endif /* ALGO_PERIOD_LED */

#ifndef ALGO_PERIOD_DISTANCE
  #define ALGO_PERIOD_DISTANCE 500
#endif /* ALGO_PERIOD_DISTANCE */

/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported macros ------------------------------------------------------- */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported types ------------------------------------------------------- */

/**
 * @brief  Target's Features data structure definition
 */
typedef struct
{
  uint8_t LedStatus;
} TargetFeatures_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported variables ------------------------------------------------------- */
extern TargetFeatures_t TargetBoardFeatures;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported functions ------------------------------------------------------- */
extern void Led_TIM_Init(void);
extern void ToF_TIM_Init(void);

extern void InitTargetPlatform(void);
extern void LedOnTargetPlatform(void);
extern void LedOffTargetPlatform(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* _TARGET_FEATURES_H_ */
