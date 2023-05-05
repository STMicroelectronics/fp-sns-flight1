/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    TargetPlatform.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.0
  * @date    06-March-2023
  * @brief   Initialization of the Target Platform
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

#include "TargetFeatures.h"
#include "BLE_Function.h"
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Imported Variables ---------------------------------------------------------*/

/* USER CODE BEGIN IP */

/* USER CODE END IP */

/* Exported variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

TargetFeatures_t TargetBoardFeatures;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Local function prototypes --------------------------------------------------*/
static void Init_53L3A2(void);

/* USER CODE BEGIN LFP */

/* USER CODE END LFP*/

/**
  * @brief Led_TIM_INSTANCE Initialization Function
  * @param None
  * @retval None
  */
void Led_TIM_Init(void)
{

  /* USER CODE BEGIN Led_TIM_INSTANCE_Init 0 */

  /* USER CODE END Led_TIM_INSTANCE_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  TimLedHandle.Instance = Led_TIM_INSTANCE;
  TimLedHandle.Init.Prescaler = ((SystemCoreClock / 10000) - 1);
  TimLedHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimLedHandle.Init.Period = ((((SystemCoreClock / 10000) * ALGO_PERIOD_LED) / 1000)- 1);
  TimLedHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TimLedHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TimLedHandle) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&TimLedHandle, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&TimLedHandle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Led_TIM_INSTANCE_Init 2 */

  /* USER CODE END Led_TIM_INSTANCE_Init 2 */

}

/**
  * @brief TimDistanceHandle Initialization Function
  * @param None
  * @retval None
  */
void ToF_TIM_Init(void)
{

  /* USER CODE BEGIN TimDistanceHandle_Init 0 */

  /* USER CODE END TimDistanceHandle_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TimDistanceHandle.Instance = ToF_TIM_INSTANCE;
  TimDistanceHandle.Init.Prescaler = ((SystemCoreClock / 10000) - 1);
  TimDistanceHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimDistanceHandle.Init.Period = ((((SystemCoreClock / 10000) * ALGO_PERIOD_DISTANCE) / 1000)- 1);
  TimDistanceHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TimDistanceHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TimDistanceHandle) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&TimDistanceHandle, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&TimDistanceHandle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TimDistanceHandle_Init 2 */

  /* USER CODE END TimDistanceHandle_Init 2 */

}

/**
  * @brief  Initialize all the Target platform's Features
  * @param  None
  * @retval None
  */
void InitTargetPlatform(void)
{
#ifdef FLIGHT1_ENABLE_PRINTF
  /* UART Initialization */
  if(BSP_COM_Init(COM1) != BSP_ERROR_NONE) {
    Error_Handler();
  } else {
    FLIGHT1_PRINTF("\033[2J\033[1;1f");
    FLIGHT1_PRINTF("UART Initialized\r\n");
  }
#endif /* FLIGHT1_ENABLE_PRINTF */

  /* Initialize X-NUCLEO-53L3A2 */
  Init_53L3A2();

  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Initialize LED */
  BSP_LED_Init(LED2);

  FLIGHT1_PRINTF("\r\nSTMicroelectronics %s:\r\n"
         "\tVersion %c.%c.%c\r\n"
         "\t%s board"
          "\r\n",
          FLIGHT1_PACKAGENAME,
          FLIGHT1_VERSION_MAJOR,FLIGHT1_VERSION_MINOR,FLIGHT1_VERSION_PATCH,
          BLE_STM32_BOARD);

  /* Reset all the Target's Features */
  memset(&TargetBoardFeatures, 0, sizeof(TargetFeatures_t));

  FLIGHT1_PRINTF("\n\r");
}

/** @brief  Initialize X-NUCLEO-53L3A2
 *  @param  None
 *  @retval None
 */
static void Init_53L3A2(void)
{
  int32_t ret;
  uint16_t i2c_addr;
  uint32_t Id;

  RANGING_SENSOR_Capabilities_t Cap;
  RANGING_SENSOR_ProfileConfig_t Profile;

   /* Put center device in shutdown mode */
  VL53L3A2_RANGING_SENSOR_SetPowerMode(VL53L3A2_DEV_CENTER, RANGING_SENSOR_POWERMODE_OFF);
  /* Power on center device */
  VL53L3A2_RANGING_SENSOR_SetPowerMode(VL53L3A2_DEV_CENTER, RANGING_SENSOR_POWERMODE_ON);
  /* Initialize center device */
  ret = VL53L3A2_RANGING_SENSOR_Init(VL53L3A2_DEV_CENTER);

  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("X-NUCLEO-53L3A2 init failed\r\n");
  }
  else
  {
    /* left: 0x54, center: 0x56, right: 0x58 */
    i2c_addr = (RANGING_SENSOR_VL53L3CX_ADDRESS + (VL53L3A2_DEV_CENTER + 1) * 2);
    VL53L3A2_RANGING_SENSOR_SetAddress(VL53L3A2_DEV_CENTER, i2c_addr);

    VL53L3A2_RANGING_SENSOR_ReadID(VL53L3A2_DEV_CENTER, &Id);
    VL53L3A2_RANGING_SENSOR_GetCapabilities(VL53L3A2_DEV_CENTER, &Cap);

    Profile.RangingProfile = RS_MULTI_TARGET_LONG_RANGE;
    /* 16 ms < TimingBudget < 500 ms */
    Profile.TimingBudget = 30;
    /* Not necessary in simple ranging */
    Profile.Frequency = 0;
    /* Enable Ambient profile - Enable: 1, Disable: 0 */
    Profile.EnableAmbient = 1;
    /* Enable Signal profile - Enable: 1, Disable: 0 */
    Profile.EnableSignal = 1;

    /* Set the profile if different from default one */
    ret= VL53L3A2_RANGING_SENSOR_ConfigProfile(VL53L3A2_DEV_CENTER, &Profile);

    FLIGHT1_PRINTF("\r\nX-NUCLEO-53L3A2 initialized\r\n");
    FLIGHT1_PRINTF("\tSensor Id: %04lX\r\n", Id);
    FLIGHT1_PRINTF("\tNumberOfZones: %lu\r\n", Cap.NumberOfZones);
    FLIGHT1_PRINTF("\tMaxNumberOfTargetsPerZone: %lu\r\n", Cap.MaxNumberOfTargetsPerZone);
    FLIGHT1_PRINTF("\tCustomROI: %lu\r\n", Cap.CustomROI);
    FLIGHT1_PRINTF("\tThresholdDetection: %lu\r\n", Cap.ThresholdDetection);

    if (ret != BSP_ERROR_NONE)
      FLIGHT1_PRINTF("\tSet profile failed\r\n\n");
    else
      FLIGHT1_PRINTF("\tSet profile ok\r\n\n");
  }
}

/**
  * @brief  This function switches on the LED
  * @param  None
  * @retval None
  */
void LedOnTargetPlatform(void)
{
  BSP_LED_On(LED2);
  TargetBoardFeatures.LedStatus=1;
}

/**
  * @brief  This function switches off the LED
  * @param  None
  * @retval None
  */
void LedOffTargetPlatform(void)
{
  BSP_LED_Off(LED2);
  TargetBoardFeatures.LedStatus=0;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
