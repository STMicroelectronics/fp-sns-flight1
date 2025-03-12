/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_flight1.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.1.0
  * @date    24-January-2025
  * @brief   This file provides code for flight1 application.
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

/* Includes ------------------------------------------------------------------*/
#include "app_flight1.h"

#include <math.h>
#include <limits.h>
#include "main.h"
#include "ble_manager.h"
#include "ble_function.h"

#include "target_features.h"
#include "bluenrg_utils.h"

#include "ota.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private macro ------------------------------------------------------------*/
/* Base address for meta data of the firmware */
#define ADDRESS_META_DATA ((uint32_t)(FLASH_END - 0xFFF))
/* Meta data dimension (Multiply of 8 bytes)*/
#define META_DATA_DIMENSION 16

/* Private types ----------------------------------------------------------- */
/* Typedef Meta Data Structure */

typedef struct
{
  uint8_t  NodeName[9];
  uint8_t  Padding[7]; /* we could write Multiply of 8 bytes at a time... */
} MetaData_t;

/* Exported Variables --------------------------------------------------------*/
uint8_t LedEnabled = 0;

uint8_t LedTimerEnabled = 0;
uint8_t ObjectsDetectionTimerEnabled = 0;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Imported Variables --------------------------------------------------------*/

/* USER CODE BEGIN IV */

/* USER CODE END IV */

/* Private variables ---------------------------------------------------------*/
static MetaData_t FirmwareMetaData;
static uint32_t NecessityToSaveMetaData = 0;

static volatile uint8_t ButtonPressed = 0;
static volatile uint32_t ToF_MultiObjects_SendDistance = 0;

static uint8_t BlinkLed = 0;

/* The VL53L3CX is able to detect up to 4 objects */
static uint16_t ObjectsDistance[4] = {0, 0, 0, 0};
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

  /* Check the Meta Data */
  FLIGHT1_PRINTF("\r\nRead Meta data (0x%lx)\r\n", ADDRESS_META_DATA);
  memcpy((void *)&FirmwareMetaData, (void *)ADDRESS_META_DATA, sizeof(MetaData_t));

  FLIGHT1_PRINTF("\n\t(HAL %ld.%ld.%ld_%ld)\r\n"
                 "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
                 " (IAR)\r\n"
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
                 " (KEIL)\r\n"
#elif defined (__GNUC__)
                 " (STM32CubeIDE)\r\n"
#endif /* IDE */
                 "\tSend Every %4d mS objects distance\r\n\n",
                 HAL_GetHalVersion() >> 24,
                 (HAL_GetHalVersion() >> 16) & 0xFF,
                 (HAL_GetHalVersion() >> 8) & 0xFF,
                 HAL_GetHalVersion()      & 0xFF,
                 __DATE__, __TIME__,
                 ALGO_PERIOD_DISTANCE);

#ifdef FLIGHT1_DEBUG_CONNECTION
  FLIGHT1_PRINTF("Debug Connection         Enabled\r\n");
#endif /* FLIGHT1_DEBUG_CONNECTION */

#ifdef FLIGHT1_DEBUG_NOTIFY_TRAMISSION
  FLIGHT1_PRINTF("Debug Notify Transmission Enabled\r\n\n");
#endif /* FLIGHT1_DEBUG_NOTIFY_TRAMISSION */

  /* Initialize the BlueNRG stack and services */
  bluetooth_init();

  /* Check the BootLoader Compliance */
  Check_BL = CheckBootLoaderCompliance();

  switch (Check_BL)
  {
    case 0xFF:
      FLIGHT1_PRINTF("\r\nERROR: BootLoader NOT Compliant with FOTA procedure\r\n\n");
      break;
    case 0x00:
      FLIGHT1_PRINTF("\r\nBootLoader Compliant with FOTA procedure (IAR)\r\n\n");
      break;
    case 0x01:
      FLIGHT1_PRINTF("\r\nBootLoader Compliant with FOTA procedure (Keil)\r\n\n");
      break;
    case 0x02:
      FLIGHT1_PRINTF("\r\nBootLoader Compliant with FOTA procedure (STM32CubeIDE)\r\n\n");
      break;
  }

  /* Enable timer for led blinking */
  if (!LedTimerEnabled)
  {
    /* Start the TIM Base generation in interrupt mode */
    if (HAL_TIM_Base_Start_IT(&TIM_LED_HANDLE) != HAL_OK)
    {
      /* Stopping Error */
      Error_Handler();
    }

    LedTimerEnabled = 1;
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
  if (!connected)
  {
    if (BlinkLed)
    {
      BSP_LED_Toggle(LED2);
      TargetBoardFeatures.LedStatus = !TargetBoardFeatures.LedStatus;
      BlinkLed = 0;
    }
  }

  if (set_connectable)
  {
    if (NecessityToSaveMetaData)
    {
      uint32_t Success = EraseMetaData();
      FLIGHT1_PRINTF("Erase Meta Data\r\n");
      if (Success)
      {
        SaveMetaData();
        FLIGHT1_PRINTF("Save Meta Data\r\n");
      }

      NecessityToSaveMetaData = 0;
    }

    /* Now update the BLE advertize data and make the Board connectable */
    enable_extended_configuration_command();

    set_connectable_ble();
    set_connectable = FALSE;
  }

  /* handle BLE event */
  hci_user_evt_proc();

  /* Handle user button */
  if (ButtonPressed)
  {
    ButtonCallback();
    ButtonPressed = 0;
  }

  /* */
  if (ToF_MultiObjects_SendDistance)
  {
    ToF_MultiObjects_SendDistance = 0;
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
  uint8_t i;
  uint8_t j;

  RANGING_SENSOR_Result_t Result;

  /* Polling mode */
  ret = RANGING_SENSOR_GET_DISTANCE(TOF_INSTANCE, &Result);

  if (ret == BSP_ERROR_NONE)
  {
    for (i = 0; i < RANGING_SENSOR_MAX_NB_ZONES; i++)
    {
      /* Number of the detected distances from the ToF sensor */
      FLIGHT1_PRINTF("\r\n");
      FLIGHT1_PRINTF("Number of objects detected= %ld\r\n", Result.ZoneResult[i].NumberOfTargets);

      /* Reset the objects distance data */
      for (j = 0; j < 4; j++)
      {
        ObjectsDistance[j] = 0;
      }

      /* Reset the Human Presence data */
      HumanPresence = 0;

      for (j = 0; j < Result.ZoneResult[i].NumberOfTargets; j++)
      {
        FLIGHT1_PRINTF("\tObject= %d status= %ld D= %5ldmm ",
                       j + 1,
                       Result.ZoneResult[i].Status[j],
                       Result.ZoneResult[i].Distance[j]);

        if (Result.ZoneResult[i].Status[j] == 0)
        {
          ObjectsDistance[j] = Result.ZoneResult[i].Distance[j];

          if ((Result.ZoneResult[i].Distance[j] >= PRESENCE_MIN_DISTANCE_RANGE) &&
              (Result.ZoneResult[i].Distance[j] <= PRESENCE_MAX_DISTANCE_RANGE))
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

      ble_objects_detection_status_update(ObjectsDistance, HumanPresence);
    }
  }
}

/**
  * @brief  EXTI line detection callback.
  * @param  uint16_t GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case GPIO_PIN_13:
      ButtonPressed = 1;
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
  if (htim == (&TIM_DISTANCE_HANDLE))
  {
    /* ToF multi objects distance */
    if (ObjectsDetectionTimerEnabled)
    {
      ToF_MultiObjects_SendDistance = 1;
    }
  }
  else if (htim == (&TIM_LED_HANDLE))
  {
    /* Led */
    if (LedTimerEnabled)
    {
      BlinkLed = 1;
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

  ret = RANGING_SENSOR_START(TOF_INSTANCE, RS_MODE_BLOCKING_CONTINUOUS);

  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("\tRanging sensor starts measurement failed\r\n");
    while (1);
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

  ret = RANGING_SENSOR_STOP(TOF_INSTANCE);

  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("\tRanging sensor stop measurement failed\r\n");
    while (1);
  }
  else
  {
    FLIGHT1_PRINTF("\tRanging sensor stop measurement OK\r\n");
  }
}

/**
  * @brief  Update Node Name on to Meta Data
  * @param  None
  * @retval None
  */
void UpdateNodeNameMetaData(void)
{
  for (uint32_t i = 0; i < 7; i++)
  {
    FirmwareMetaData.NodeName[i + 1] = ble_stack_value.board_name[i];
  }

  /* Save the node name in the meta data */
  NecessityToSaveMetaData = 1;
}

/**
  * @brief  Check if there are a valid Node Name Values in Memory and read them
  * @param  None
  * @retval unsigned char Success/Not Success
  */
unsigned char ReCallNodeNameFromMemory(void)
{
  /* ReLoad the Node Name Values from RAM */
  unsigned char Success = 0;

  if (FirmwareMetaData.NodeName[0] != 0x12)
  {
    FirmwareMetaData.NodeName[0] = 0x12;

    for (uint32_t i = 0; i < 7; i++)
    {
      FirmwareMetaData.NodeName[i + 1] = ble_stack_value.board_name[i];
    }

    FirmwareMetaData.NodeName[8] = '\0';

    NecessityToSaveMetaData = 1;

    FLIGHT1_PRINTF("\r\nNode name not present in FLASH\r\n");
    FLIGHT1_PRINTF("\tNode name written to FLASH= %s\r\n", ble_stack_value.board_name);
  }
  else
  {
    for (uint32_t i = 0; i < 7; i++)
    {
      ble_stack_value.board_name[i] = FirmwareMetaData.NodeName[i + 1];
    }

    FLIGHT1_PRINTF("\r\nNode name read from FLASH (%s)\r\n", ble_stack_value.board_name);
  }

  return Success;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

#if defined (STM32L471xx) || defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || \
    defined (STM32L486xx) || defined (STM32L496xx) || defined (STM32L4A6xx) || defined (STM32L4P5xx) || \
    defined (STM32L4Q5xx) || defined (STM32L4R5xx) || defined (STM32L4R7xx) || defined (STM32L4R9xx) || \
  defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
    /* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
    /* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }
#else /* STM32L4yyxx */
  bank = FLASH_BANK_1;
#endif /* STM32L4yyxx */

  return bank;
}

/**
  * @brief User function for Erasing the Flash data
  * @param None
  * @retval uint32_t Success/NotSuccess [1/0]
  */
uint32_t EraseMetaData(void)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t Success = 1;

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = GetBank(ADDRESS_META_DATA);
  EraseInitStruct.Page        = GetPage(ADDRESS_META_DATA);
  EraseInitStruct.NbPages     = 2;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /* Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    Success = 0;
    FLIGHT1_PRINTF("Error occurred while page erase\r\n");
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/**
  * @brief User function for Saving the MDM  on the Flash
  * @param None
  * @retval None
  */
void SaveMetaData(void)
{
  /* Store in Flash Memory */
  uint32_t Address;
  uint32_t BankInfoAddress;
  uint32_t Count;

  Address = ADDRESS_META_DATA;
  BankInfoAddress = (uint32_t)&FirmwareMetaData;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  for (Count = 0; Count < META_DATA_DIMENSION; Count += 8)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, (*(uint64_t *)BankInfoAddress)) != HAL_OK)
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      FLIGHT1_PRINTF("Error occurred while writing data in Flash memory\r\n");
    }

    Address = Address + 8;
    BankInfoAddress = BankInfoAddress + 8;
  }

  NecessityToSaveMetaData = 0;

  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
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
  while ((HAL_GetTick() - tickstart) < Delay)
  {
    __WFI();
  }
}

