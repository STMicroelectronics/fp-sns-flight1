/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Function.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.0
  * @date    06-March-2023
  * @brief   Bluetooth support
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
#ifndef _BLE_FUNCTION_H_
#define _BLE_FUNCTION_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported define ------------------------------------------------------------*/
/* STM32 board type*/
#define BLE_STM32_BOARD "STM32L476RGTX-NUCLEO"

/* Exported Variables --------------------------------------------------------*/
extern uint8_t connected;

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FUNCTION_H_ */
