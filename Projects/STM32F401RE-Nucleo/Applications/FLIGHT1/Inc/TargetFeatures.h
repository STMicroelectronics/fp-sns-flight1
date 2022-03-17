/**
  ******************************************************************************
  * @file    TargetFeatures.h 
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   Specification of the HW Features for each target platform
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
   
/* Code for NDEF message software Library */
#include "common.h"
#include "tagtype5_wrapper.h"
#include "lib_NDEF_URI.h"
   
#include "53l3a2_ranging_sensor.h"

#include "FLIGHT1_config.h"
#include "MetaDataManager.h"

/* Exported defines ------------------------------------------------------- */

/* Exported macros ------------------------------------------------------- */

/* Private define ------------------------------------------------------------*/
#define VL53L1_INT_Pin          GPIO_PIN_4
#define VL53L1_INT_GPIO_Port    GPIOA

/* Exported types ------------------------------------------------------- */
   
/**
 * @brief  Target's Features data structure definition
 */
typedef struct
{
  /* NFC Tag support - No by default */
  uint8_t NFC_TAG_support;
  
  uint8_t LedStatus;
} TargetFeatures_t;

/* Exported variables ------------------------------------------------------- */
extern TargetFeatures_t TargetBoardFeatures;

/* Exported functions ------------------------------------------------------- */
extern void InitTargetPlatform(void);
extern void LedOnTargetPlatform(void);
extern void LedOffTargetPlatform(void);

#ifdef __cplusplus
}
#endif

#endif /* _TARGET_FEATURES_H_ */


