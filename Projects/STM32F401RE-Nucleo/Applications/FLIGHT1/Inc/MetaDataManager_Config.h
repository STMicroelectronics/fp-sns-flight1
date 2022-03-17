/**
  ******************************************************************************
  * @file    MetaDataManager_Config.h 
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   Meta Data Manager Config Header File
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
#ifndef _META_DATA_MANAGER_CONFIG_H_
#define _META_DATA_MANAGER_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ---------------------------------------------------- */
#include <stdlib.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"
#include "FLIGHT1_config.h"

#define MDM_PRINTF FLIGHT1_PRINTF

#ifdef __cplusplus
}
#endif

#endif /* _META_DATA_MANAGER_CONFIG_H_ */
