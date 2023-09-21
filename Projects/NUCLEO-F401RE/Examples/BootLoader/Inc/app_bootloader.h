/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_bootloader.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 3.1.0
  * @date    6-March-2023
  * @brief   Header file for app_bootloader.c
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
#ifndef APP_BOOTLOADER_H
#define APP_BOOTLOADER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_exti.h"
#include "stm32f4xx_nucleo.h"

/* Imported Variables --------------------------------------------------------*/

/* USER CODE BEGIN IV */

/* USER CODE END IV */

/* Exported Define -----------------------------------------------------------*/

/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported Variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported Functions --------------------------------------------------------*/
void MX_BootLoader_Init(void);
void MX_BootLoader_Process(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* APP_BOOTLOADER_H */
