/**
  ******************************************************************************
  * @file    stm32xxx_hal.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   This file allows to select the right stm32 hal file
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
#ifndef __STM32xxx_HAL_H
#define __STM32xxx_HAL_H


#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
//#include "mxconstants.h"

#ifdef STM32F401xE
#include "stm32f4xx_hal.h"
#define MCU_NAME "STM32F401xE"
#endif
#ifdef STM32L476xx
#include "stm32l4xx_hal.h"
#define MCU_NAME "STM32L476"
#endif
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if TRACE_UART
#	define trace_printf uart_printf
extern int uart_printf(const char *msg, ...);
#else
#	define trace_printf(...) (void)0
#	define uart_printf(...) (void)0
#endif
 /* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __STM32xxx_HAL_H */
