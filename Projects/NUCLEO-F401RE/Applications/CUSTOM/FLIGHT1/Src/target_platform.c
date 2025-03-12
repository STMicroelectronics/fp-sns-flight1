/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    target_platform.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.1.0
  * @date    24-January-2025
  * @brief   Initialization of the Target Platform
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

#include "target_features.h"
#include "ble_function.h"
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Imported Variables ---------------------------------------------------------*/

/* USER CODE BEGIN IP */

/* USER CODE END IP */

/* Exported variables ---------------------------------------------------------*/

TargetFeatures_t TargetBoardFeatures;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Local function prototypes --------------------------------------------------*/
static void Init_ToF_Sensor(void);

/* USER CODE BEGIN LFP */

/* USER CODE END LFP*/

/**
  * @brief LED_TIM_INSTANCE Initialization Function
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
  TIM_LED_HANDLE.Instance = LED_TIM_INSTANCE;
  TIM_LED_HANDLE.Init.Prescaler = ((SystemCoreClock / 10000) - 1);
  TIM_LED_HANDLE.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_LED_HANDLE.Init.Period = ((((SystemCoreClock / 10000) * ALGO_PERIOD_LED) / 1000) - 1);
  TIM_LED_HANDLE.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM_LED_HANDLE.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TIM_LED_HANDLE) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&TIM_LED_HANDLE, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&TIM_LED_HANDLE, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Led_TIM_INSTANCE_Init 2 */

  /* USER CODE END Led_TIM_INSTANCE_Init 2 */

}

/**
  * @brief TOF_TIM_INSTANCE Initialization Function
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
  TIM_DISTANCE_HANDLE.Instance = TOF_TIM_INSTANCE;
  TIM_DISTANCE_HANDLE.Init.Prescaler = ((SystemCoreClock / 10000) - 1);
  TIM_DISTANCE_HANDLE.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_DISTANCE_HANDLE.Init.Period = ((((SystemCoreClock / 10000) * ALGO_PERIOD_DISTANCE) / 1000) - 1);
  TIM_DISTANCE_HANDLE.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM_DISTANCE_HANDLE.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TIM_DISTANCE_HANDLE) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&TIM_DISTANCE_HANDLE, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&TIM_DISTANCE_HANDLE, &sMasterConfig) != HAL_OK)
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
  if (BSP_COM_Init(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  else
  {
    FLIGHT1_PRINTF("\033[2J\033[1;1f");
    FLIGHT1_PRINTF("UART Initialized\r\n");
  }
#endif /* FLIGHT1_ENABLE_PRINTF */

  /* Initialize ToF Sensor */
  Init_ToF_Sensor();

  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Initialize LED */
  BSP_LED_Init(LED2);

  FLIGHT1_PRINTF("\r\nSTMicroelectronics %s:\r\n"
                 "\tVersion %c.%c.%c\r\n"
                 "\t%s board"
                 "\r\n",
                 FLIGHT1_PACKAGENAME,
                 FLIGHT1_VERSION_MAJOR, FLIGHT1_VERSION_MINOR, FLIGHT1_VERSION_PATCH,
                 BLE_STM32_BOARD);

  /* Reset all the Target's Features */
  memset(&TargetBoardFeatures, 0, sizeof(TargetFeatures_t));

  FLIGHT1_PRINTF("\n\r");
}

/**  @brief  Initialize ToF Sensor
  *  @param  None
  *  @retval None
  */
static void Init_ToF_Sensor(void)
{
  int32_t ret;
  uint32_t Id;

  RANGING_SENSOR_Capabilities_t Cap;
  RANGING_SENSOR_ProfileConfig_t Profile;

  /* Initialize center device */
  ret = RANGING_SENSOR_INIT(TOF_INSTANCE);

  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("Init ToF sensor failed\r\n");
  }
  else
  {

    RANGING_SENSOR_READ_ID(TOF_INSTANCE, &Id);
    RANGING_SENSOR_GET_CAPABILITIES(TOF_INSTANCE, &Cap);

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
    ret = RANGING_SENSOR_CONFIG_PROFILE(TOF_INSTANCE, &Profile);

    FLIGHT1_PRINTF("\r\nToF sensor initialized\r\n");
    FLIGHT1_PRINTF("\tSensor Id: %04lX\r\n", Id);
    FLIGHT1_PRINTF("\tNumberOfZones: %lu\r\n", Cap.NumberOfZones);
    FLIGHT1_PRINTF("\tMaxNumberOfTargetsPerZone: %lu\r\n", Cap.MaxNumberOfTargetsPerZone);
    FLIGHT1_PRINTF("\tCustomROI: %lu\r\n", Cap.CustomROI);
    FLIGHT1_PRINTF("\tThresholdDetection: %lu\r\n", Cap.ThresholdDetection);

    if (ret != BSP_ERROR_NONE)
    {
      FLIGHT1_PRINTF("\tSet profile failed\r\n\n");
    }
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
  TargetBoardFeatures.LedStatus = 1;
}

/**
  * @brief  This function switches off the LED
  * @param  None
  * @retval None
  */
void LedOffTargetPlatform(void)
{
  BSP_LED_Off(LED2);
  TargetBoardFeatures.LedStatus = 0;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
