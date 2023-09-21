/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_flight1.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.2
  * @date    20-June-2023
  * @brief   This file provides code for flight1 application.
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

/* Includes ------------------------------------------------------------------*/
#include "app_flight1.h"

#include <math.h>
#include <limits.h>
#include "main.h"
#include "BLE_Manager.h"
#include "BLE_Function.h"

#include "TargetFeatures.h"
#include "bluenrg_utils.h"

#include "OTA.h"

#include "MetaDataManager.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported Variables --------------------------------------------------------*/

uint8_t LedEnabled= 0;

uint8_t LedTimerEnabled= 0;
uint8_t ObjectsDetectionTimerEnabled= 0;

uint8_t NodeName[8];

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Imported Variables --------------------------------------------------------*/

/* USER CODE BEGIN IV */

/* USER CODE END IV */

/* Private variables ---------------------------------------------------------*/
/* Table with All the known Meta Data */
MDM_knownGMD_t known_MetaData[]={
  {GMD_NODE_NAME,      (sizeof(NodeName))},
  {GMD_END    ,0}/* THIS MUST BE THE LAST ONE */
};

static volatile int ButtonPressed		= 0;
static volatile uint32_t ToF_MultiObjects_SendDistance   = 0;

static uint8_t BlinkLed         = 0;

/* The VL53L3CX is able to detect up to 4 objects */
static uint16_t ObjectsDistance[4]= {0, 0, 0, 0};
static uint8_t HumanPresence;

/* Private function prototypes -----------------------------------------------*/
static void User_Init(void);
static void User_Process(void);

static void ButtonCallback(void);
static void RangingLoop(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

void MX_FLIGHT1_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN FLIGHT1_Init_PreTreatment */

  /* USER CODE END FLIGHT1_Init_PreTreatment */

  /* Initialize FLIGHT1 application */

  User_Init();

  /* USER CODE BEGIN FLIGHT1_Init_PostTreatment */

  /* USER CODE END FLIGHT1_Init_PostTreatment */
}

/*
 * FLIGHT1 background task
 */
void MX_FLIGHT1_Process(void)
{
  /* USER CODE BEGIN FLIGHT1_Process_PreTreatment */

  /* USER CODE END FLIGHT1_Process_PreTreatment */

  /* Process of the FLIGHT1 application */

  User_Process();

  /* USER CODE BEGIN FLIGHT1_Process_PostTreatment */

  /* USER CODE END FLIGHT1_Process_PostTreatment */
}

/**
 * @brief  Initialize User process.
 *
 * @param  None
 * @retval None
 */
static void User_Init(void)
{
  uint8_t Check_BL;

  Led_TIM_Init();
  ToF_TIM_Init();

  InitTargetPlatform();

  /* Check the MetaDataManager */
  InitMetaDataManager((void *)&known_MetaData,MDM_DATA_TYPE_GMD,NULL);

  FLIGHT1_PRINTF("\n\t(HAL %ld.%ld.%ld_%ld)\r\n"
        "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n"
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
        " (KEIL)\r\n"
#elif defined (__GNUC__)
        " (STM32CubeIDE)\r\n"
#endif
        "\tSend Every %4d mS objects distance\r\n\n",
           HAL_GetHalVersion() >>24,
          (HAL_GetHalVersion() >>16)&0xFF,
          (HAL_GetHalVersion() >> 8)&0xFF,
           HAL_GetHalVersion()      &0xFF,
         __DATE__,__TIME__,
         ALGO_PERIOD_DISTANCE);

#ifdef FLIGHT1_DEBUG_CONNECTION
  FLIGHT1_PRINTF("Debug Connection         Enabled\r\n");
#endif /* FLIGHT1_DEBUG_CONNECTION */

#ifdef FLIGHT1_DEBUG_NOTIFY_TRAMISSION
  FLIGHT1_PRINTF("Debug Notify Trasmission Enabled\r\n\n");
#endif /* FLIGHT1_DEBUG_NOTIFY_TRAMISSION */

  /* Initialize the BlueNRG stack and services */
  BluetoothInit();

  /* Check the BootLoader Compliance */
  Check_BL= CheckBootLoaderCompliance();

  switch(Check_BL)
  {
  case 0:
    FLIGHT1_PRINTF("\r\nERROR: BootLoader NOT Compliant with FOTA procedure\r\n\n");
    break;
  case 1:
    FLIGHT1_PRINTF("\r\nBootLoader Compliant with FOTA procedure (IAR)\r\n\n");
    break;
  case 2:
    FLIGHT1_PRINTF("\r\nBootLoader Compliant with FOTA procedure (Keil)\r\n\n");
    break;
  case 3:
    FLIGHT1_PRINTF("\r\nBootLoader Compliant with FOTA procedure (STM32CubeIDE)\r\n\n");
    break;
  }

  /* Enable timer for led blinking */
  if(!LedTimerEnabled) {
    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_Base_Start_IT(&TimLedHandle) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }

    LedTimerEnabled= 1;
  }
}

/**
 * @brief  Configure the device as Client or Server and manage the communication
 *         between a client and a server.
 *
 * @param  None
 * @retval None
 */
static void User_Process(void)
{
  /* Blinking the Led */
    if(!connected) {
      if(BlinkLed) {
        BSP_LED_Toggle(LED2);
        TargetBoardFeatures.LedStatus = !TargetBoardFeatures.LedStatus;
		BlinkLed= 0;
      }
    }

    if(set_connectable){
      if(NecessityToSaveMetaDataManager) {
        uint32_t Success = EraseMetaDataManager();
        if(Success) {
          SaveMetaDataManager();
        }
      }

      /* Now update the BLE advertize data and make the Board connectable */
      setConnectable();
      set_connectable = FALSE;
    }

    /* handle BLE event */
    hci_user_evt_proc();

    /* Handle user button */
    if(ButtonPressed) {
      ButtonCallback();
      ButtonPressed=0;
    }

    /* */
    if(ToF_MultiObjects_SendDistance) {
      ToF_MultiObjects_SendDistance= 0;
      RangingLoop();
    }

    /* Wait for Event */
    __WFI();
}

/**
  * @brief  Callback for user button
  * @param  None
  * @retval None
  */
static void ButtonCallback(void)
{
  FLIGHT1_PRINTF("UserButton Pressed\r\n");
}

/**
  * @brief  Objects distance evaluation and human presence detect
  * @param  None
  * @retval None
  */
static void RangingLoop(void)
{
  uint32_t ret;
  uint8_t i, j;

  RANGING_SENSOR_Result_t Result;

  /* Polling mode */
  ret = VL53L3A2_RANGING_SENSOR_GetDistance(VL53L3A2_DEV_CENTER, &Result);

  if (ret == BSP_ERROR_NONE)
  {
    for (i = 0; i < RANGING_SENSOR_MAX_NB_ZONES; i++)
    {
      /* Number of the detected distances from the ToF sensor */
      FLIGHT1_PRINTF("\r\nNumber of objects detected= %ld\r\n", Result.ZoneResult[i].NumberOfTargets);

      /* Reset the objects distance data */
      for(j=0;j<4;j++)
        ObjectsDistance[j]= 0;

      /* Reset the Human Presence data */
      HumanPresence= 0;

      for (j = 0; j < Result.ZoneResult[i].NumberOfTargets; j++)
      {
        FLIGHT1_PRINTF("\tObject= %d status= %ld D= %5ldmm ",
                       j+1,
                       Result.ZoneResult[i].Status[j],
                       Result.ZoneResult[i].Distance[j]);

        if(Result.ZoneResult[i].Status[j] == 0)
        {
          ObjectsDistance[j]= Result.ZoneResult[i].Distance[j];

          if( (Result.ZoneResult[i].Distance[j] >= PRESENCE_MIN_DISTANCE_RANGE) &&
              (Result.ZoneResult[i].Distance[j] <= PRESENCE_MAX_DISTANCE_RANGE) )
          {
            HumanPresence++;
            FLIGHT1_PRINTF("Human Presence= %d", HumanPresence);
          }

          FLIGHT1_PRINTF("\r\n");
        }
        else
        {
          FLIGHT1_PRINTF("Not Valid Measure \r\n");
        }
      }

      BLE_ObjectsDetectionStatusUpdate(ObjectsDistance, HumanPresence);
    }
  }

  //HAL_Delay(POLLING_PERIOD);
}

/**
 * @brief  EXTI line detection callback.
 * @param  uint16_t GPIO_Pin Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin){
  case GPIO_PIN_13:
    ButtonPressed=1;
    break;
  }
}

/**
  * @brief  Period elapsed callback in non blocking mode for Environmental timer
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==(&TimDistanceHandle)) {
    /* ToF multi objects distance */
    if(ObjectsDetectionTimerEnabled) {
      ToF_MultiObjects_SendDistance=1;
    }
  } else if(htim == (&TimLedHandle)) {
    /* Led */
    if(LedTimerEnabled)
    {
      BlinkLed=1;
    }
  }
}

/**
 * @brief  BSP Push Button callback
 * @param  Button Specifies the pin connected EXTI line
 * @retval None.
 */
void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);

  ButtonPressed = 1;
}

/**
  * @brief  Stars Measurement of the VL53L3CX component.
  * @param  None
  * @retval None
  */
void StartMeasurement(void)
{
  int32_t ret;

  FLIGHT1_PRINTF("Ranging sensor starts\r\n");

  ret = VL53L3A2_RANGING_SENSOR_Start(VL53L3A2_DEV_CENTER, RS_MODE_BLOCKING_CONTINUOUS);

  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("\tRanging sensor starts measurement failed\r\n");
    while(1);
  }
  else
  {
    FLIGHT1_PRINTF("\tRanging sensor starts measurement OK\r\n");
  }
}

/**
  * @brief  Stops Measurement of the VL53L3CX component.
  * @param  None
  * @retval None
  */
void StopMeasurement(void)
{
  int32_t ret;

  FLIGHT1_PRINTF("Ranging sensor stop\r\n");

  ret = VL53L3A2_RANGING_SENSOR_Stop(VL53L3A2_DEV_CENTER);

  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("\tRanging sensor stop measurement failed\r\n");
    while(1);
  }
  else
  {
    FLIGHT1_PRINTF("\tRanging sensor stop measurement OK\r\n");
  }
}

/**
 * @brief  Check if there are a valid Node Name Values in Memory and read them
 * @param  None
 * @retval unsigned char Success/Not Success
 */
unsigned char ReCallNodeNameFromMemory(void)
{
  /* ReLoad the Node Name Values from RAM */
  unsigned char Success=0;

  /* Recall the node name Credential saved */
  MDM_ReCallGMD(GMD_NODE_NAME,(void *)&NodeName);

  if(NodeName[0] != 0x12)
  {
    NodeName[0]= 0x12;

    for(int i=0; i<7; i++)
      NodeName[i+1]= BLE_StackValue.BoardName[i];

    MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
    NecessityToSaveMetaDataManager=1;

    FLIGHT1_PRINTF("\r\nNode name not present in FLASH\r\n");
    FLIGHT1_PRINTF("\tNode name written to FLASH= %s\r\n", BLE_StackValue.BoardName);
  }
  else
  {
    for(int i=0; i<7; i++)
      BLE_StackValue.BoardName[i]= NodeName[i+1];

    FLIGHT1_PRINTF("\r\nNode name read from FLASH (%s)\r\n", BLE_StackValue.BoardName);
  }

  return Success;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note This is a user implementation using WFI state
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay){
    __WFI();
  }
}

