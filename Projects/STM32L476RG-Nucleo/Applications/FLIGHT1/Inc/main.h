/**
  ******************************************************************************
  * @file    main.h 
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   Header for main.c module
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "FLIGHT1_config.h"
#include "TargetFeatures.h"
#include "BLE_Manager.h"

/* Exported functions ------------------------------------------------------- */
extern void Error_Handler(void);
extern void StartMeasurement(void);
extern void StopMeasurement(void);

/* Exported variables  ------------------------------------------------------ */
extern uint8_t NodeName[];

extern uint8_t LedEnabled;
extern uint8_t ObjectsDetectionTimerEnabled;

extern TIM_HandleTypeDef    TimDistanceHandle;

/* Exported defines --------------------------------------------------------- */

/************************************************/
/* Algorithms Frequency and Time Period section */
/*       used for timers initialization         */
/************************************************/

/* Update frequency for environmental sensor [Hz] */
#define ALGO_FREQ_DISTANCE   2U
/* Compute the prescaler value to have TIM3 counter clock equal to 2 KHz */
#define TIM_CLOCK_DISTANCE   2000U
/* Update period for environmental sensor [ms] */
#define ALGO_PERIOD_DISTANCE (1000U / ALGO_FREQ_DISTANCE)

#endif /* __MAIN_H */
