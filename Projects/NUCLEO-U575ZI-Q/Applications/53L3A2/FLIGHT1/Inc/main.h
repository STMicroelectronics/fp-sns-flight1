/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.1.0
  * @date    24-January-2025
  * @brief   Header for main.c file.
  *          This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

#include "hci_tl_interface.h"
#include "stm32u5xx_nucleo.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_TIM1_Init(void);
void MX_TIM2_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BSP_BUTTON_EXTI_IRQn EXTI13_IRQn
#define SPI1_CS_Pin GPIO_PIN_2
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_RST_Pin GPIO_PIN_13
#define SPI1_RST_GPIO_Port GPIOF
#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOG
#define LED_BLUE_Pin GPIO_PIN_7
#define LED_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
