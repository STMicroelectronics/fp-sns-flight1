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
/* Meta data dimension (Multiply of 16 bytes)*/
#define META_DATA_DIMENSION 48
/* Meta data memory address for bank 1 */
#define ADDRESS_META_DATA_BANK1 0x081FE000
/* Meta data memory address for bank 2 */
#define ADDRESS_META_DATA_BANK2 0x080FE000
/* Magic number for a valid Fw Id saved on flash */
#define FW_ID_MAGIC_NUM 0xDEADBEEF

/* Private types ----------------------------------------------------------- */
/* Typedef Meta Data Structure */

typedef struct
{
  uint8_t  NodeName[9];
  uint8_t  NodeName_2[9];
  uint32_t FwIdMagicNum_Bank_1;
  uint16_t FwIdBank_1;
  uint32_t FwIdMagicNum_Bank_2;
  uint16_t FwIdBank_2;
  uint8_t  Padding[18]; /* we could write Multiply of 16 bytes at a time...
                           2 + 16 to preserve the correct data write for the last 4 bytes*/
} MetaData_t;

/* Exported Variables --------------------------------------------------------*/
uint8_t LedEnabled = 0;

uint8_t LedTimerEnabled = 0;
uint8_t ObjectsDetectionTimerEnabled = 0;

uint32_t CurrentActiveBank = 0;
uint32_t OthersBank = 0;
uint16_t FwId_Bank1;
uint16_t FwId_Bank2;

volatile uint32_t SwapBanks   = 0;

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

/* Enable Disable the jump to second flash bank and reboot board */
static void EnableDisableDualBoot(void);

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
  Led_TIM_Init();
  ToF_TIM_Init();

  InitTargetPlatform();
  CurrentActiveBank = 0;
  /* Check if we are running from Bank1 or Bank2 */
  {
    FLASH_OBProgramInitTypeDef    OBInit;
    /* Allow Access to Flash control registers and user Flash */
    HAL_FLASH_Unlock();
    /* Allow Access to option bytes sector */
    HAL_FLASH_OB_Unlock();
    /* Get the Dual boot configuration status */
    HAL_FLASHEx_OBGetConfig(&OBInit);
    if (((OBInit.USERConfig) & (OB_SWAP_BANK_ENABLE)) == OB_SWAP_BANK_ENABLE)
    {
      CurrentActiveBank = 2;
      OthersBank = 1;
    }
    else
    {
      CurrentActiveBank = 1;
      OthersBank = 2;
    }
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
  }

  /* Check the Meta Data */
  if (CurrentActiveBank == 1)
  {
    FLIGHT1_PRINTF("\r\nRead Meta data (0x%x)\r\n", ADDRESS_META_DATA_BANK1);
    memcpy((void *)&FirmwareMetaData, (void *)ADDRESS_META_DATA_BANK1, sizeof(MetaData_t));
  }

  if (CurrentActiveBank == 2)
  {
    FLIGHT1_PRINTF("\r\nRead Meta data (0x%x)\r\n", ADDRESS_META_DATA_BANK2);
    memcpy((void *)&FirmwareMetaData, (void *)ADDRESS_META_DATA_BANK2, sizeof(MetaData_t));
  }

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

    if (CurrentActiveBank == 1)
    {
      if (FwId_Bank2 == FW_ID_NOT_VALID)
      {
        /* Enable the Banks Swap only if there is a valid fw on second bank */
        ble_extended_configuration_value.banks_swap = 0;
      }
    }

    if (CurrentActiveBank == 2)
    {
      if (FwId_Bank1 == FW_ID_NOT_VALID)
      {
        /* Enable the Banks Swap only if there is a valid fw on second bank */
        ble_extended_configuration_value.banks_swap = 0;
      }
    }
    set_connectable_ble();
    set_connectable = FALSE;
  }

  /* handle BLE event */
  hci_user_evt_proc();

  /* Swap the Flash Banks */
  if (SwapBanks)
  {
    EnableDisableDualBoot();
    SwapBanks = 0;
  }

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
  * @brief  Enable Disable the jump to second flash bank and reboot board
  * @param  None
  * @retval None
  */
static void EnableDisableDualBoot(void)
{
  FLASH_OBProgramInitTypeDef    OBInit;
  /* Set BFB2 bit to enable boot from Flash Bank2 */
  /* Allow Access to Flash control registers and user Flash */
  HAL_FLASH_Unlock();

  /* Allow Access to option bytes sector */
  HAL_FLASH_OB_Unlock();

  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OBInit);

  /* Enable/Disable dual boot feature */
  OBInit.OptionType = OPTIONBYTE_USER;
  OBInit.USERType   = OB_USER_SWAP_BANK;

  if (((OBInit.USERConfig) & (FLASH_OPTR_SWAP_BANK)) == FLASH_OPTR_SWAP_BANK)
  {
    OBInit.USERConfig &= ~FLASH_OPTR_SWAP_BANK;
    FLIGHT1_PRINTF("->Disable DualBoot\r\n");
  }
  else
  {
    OBInit.USERConfig = FLASH_OPTR_SWAP_BANK;
    FLIGHT1_PRINTF("->Enable DualBoot\r\n");
  }

  if (HAL_FLASHEx_OBProgram(&OBInit) != HAL_OK)
  {
    /*
    Error occurred while setting option bytes configuration.
    User can add here some code to deal with this error.
    To know the code error, user can call function 'HAL_FLASH_GetError()'
    */
    Error_Handler();
  }

  /* Start the Option Bytes programming process */
  if (HAL_FLASH_OB_Launch() != HAL_OK)
  {
    /*
    Error occurred while reloading option bytes configuration.
    User can add here some code to deal with this error.
    To know the code error, user can call function 'HAL_FLASH_GetError()'
    */
    Error_Handler();
  }
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();
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
    if (CurrentActiveBank == 1)
    {
      FirmwareMetaData.NodeName[i + 1] = ble_stack_value.board_name[i];
    }

    if (CurrentActiveBank == 2)
    {
      FirmwareMetaData.NodeName_2[i + 1] = ble_stack_value.board_name[i];
    }
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
  * @brief  Check if there is a valid Node Name Values in Memory for bank 2 and read them
  * @param  None
  * @retval unsigned char Success/Not Success
  */
unsigned char ReCallNodeName2FromMemory(void)
{
  /* ReLoad the Node Name Values from RAM */
  unsigned char Success = 0;

  if (FirmwareMetaData.NodeName_2[0] != 0x12)
  {
    FirmwareMetaData.NodeName_2[0] = 0x12;

    for (uint32_t i = 0; i < 7; i++)
    {
      FirmwareMetaData.NodeName_2[i + 1] = ble_stack_value.board_name[i];
    }

    FirmwareMetaData.NodeName_2[8] = '\0';

    NecessityToSaveMetaData = 1;

    FLIGHT1_PRINTF("\r\nNode name not present in FLASH\r\n");
    FLIGHT1_PRINTF("\tNode name written to FLASH= %s\r\n", ble_stack_value.board_name);
  }
  else
  {
    for (uint32_t i = 0; i < 7; i++)
    {
      ble_stack_value.board_name[i] = FirmwareMetaData.NodeName_2[i + 1];
    }

    FLIGHT1_PRINTF("\r\nNode name read from FLASH (%s)\r\n", ble_stack_value.board_name);
  }

  return Success;
}

/**
  * @brief  Check if there is a valid firmware Id Values in Memory for bank 1 and read them
  * @param  None
  * @retval unsigned char Success/Not Success
  */
unsigned char ReCallBank1FwIdFromMemory(void)
{
  /* ReLoad the Node Name Values from RAM */
  unsigned char Success = 0;

  if (FirmwareMetaData.FwIdMagicNum_Bank_1 != FW_ID_MAGIC_NUM)
  {
    FirmwareMetaData.FwIdMagicNum_Bank_1 = FW_ID_MAGIC_NUM;

    FLIGHT1_PRINTF("\r\nBank 1 FW ID not present in FLASH");

    if (CurrentActiveBank == 1)
    {
      FirmwareMetaData.FwIdBank_1 = CUSTOM_FIRMWARE_ID;
      FLIGHT1_PRINTF("\r\nBank 1 FW ID written to FLASH= 0x%x\r\n", FirmwareMetaData.FwIdBank_1);
    }

    if (CurrentActiveBank == 2)
    {
      FirmwareMetaData.FwIdBank_1 = FW_ID_NOT_VALID;
      FLIGHT1_PRINTF("\r\nBank 1 not valid FW ID written to FLASH= 0x%x\r\n", FirmwareMetaData.FwIdBank_1);
    }

    NecessityToSaveMetaData = 1;
  }
  else
  {
    if (CurrentActiveBank == 1)
    {
      if (FirmwareMetaData.FwIdBank_1 != CUSTOM_FIRMWARE_ID)
      {
        FirmwareMetaData.FwIdBank_1 = CUSTOM_FIRMWARE_ID;
        NecessityToSaveMetaData = 1;
        FLIGHT1_PRINTF("\r\nBank 1 FW ID updated and written to FLASH= 0x%x\r\n\r\n", FirmwareMetaData.FwIdBank_1);
      }
      else
      {
        FLIGHT1_PRINTF("\r\nBank 1 FW ID read from FLASH= 0x%x\r\n", FirmwareMetaData.FwIdBank_1);
      }
    }

    if (CurrentActiveBank == 2)
    {
      FLIGHT1_PRINTF("\r\nBank 1 FW ID read from FLASH= 0x%x\r\n", FirmwareMetaData.FwIdBank_1);
    }
  }

  FwId_Bank1 = FirmwareMetaData.FwIdBank_1;

  return Success;
}

/**
  * @brief  Check if there is a valid firmware Id Values in Memory for bank 2 and read them
  * @param  None
  * @retval unsigned char Success/Not Success
  */
unsigned char ReCallBank2FwIdFromMemory(void)
{
  /* ReLoad the Node Name Values from RAM */
  unsigned char Success = 0;

  if (FirmwareMetaData.FwIdMagicNum_Bank_2 != FW_ID_MAGIC_NUM)
  {
    FirmwareMetaData.FwIdMagicNum_Bank_2 = FW_ID_MAGIC_NUM;

    FLIGHT1_PRINTF("\r\nBank 2 FW ID not present in FLASH");

    if (CurrentActiveBank == 1)
    {
      FirmwareMetaData.FwIdBank_2 = FW_ID_NOT_VALID;
      FLIGHT1_PRINTF("\r\nBank 2 not valid FW ID written to FLASH= 0x%x\r\n\r\n", FirmwareMetaData.FwIdBank_2);
    }

    if (CurrentActiveBank == 2)
    {
      FirmwareMetaData.FwIdBank_2 = CUSTOM_FIRMWARE_ID;
      FLIGHT1_PRINTF("\r\nBank 2 FW ID written to FLASH= 0x%x\r\n\r\n", FirmwareMetaData.FwIdBank_2);
    }

    NecessityToSaveMetaData = 1;
  }
  else
  {
    if (CurrentActiveBank == 2)
    {
      if (FirmwareMetaData.FwIdBank_2 != CUSTOM_FIRMWARE_ID)
      {
        FirmwareMetaData.FwIdBank_2 = CUSTOM_FIRMWARE_ID;
        NecessityToSaveMetaData = 1;
        FLIGHT1_PRINTF("\r\nBank 2 FW ID updated and written to FLASH= 0x%x\r\n\r\n", FirmwareMetaData.FwIdBank_2);
      }
      else
      {
        FLIGHT1_PRINTF("\r\nBank 2 FW ID read from FLASH= 0x%x\r\n", FirmwareMetaData.FwIdBank_2);
      }
    }

    if (CurrentActiveBank == 1)
    {
      FLIGHT1_PRINTF("\r\nBank 2 FW ID read from FLASH= 0x%x\r\n", FirmwareMetaData.FwIdBank_2);
    }
  }

  FwId_Bank2 = FirmwareMetaData.FwIdBank_2;

  return Success;
}

/**
  * @brief User function for Erasing the Flash data
  * @param None
  * @retval uint32_t Success/NotSuccess [1/0]
  */
uint32_t EraseMetaData(void)
{
  /* We need to change both the Banks' info */
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t Success = 1;

  /* Disable instruction cache prior to internal cacheable memory update */
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    Error_Handler();
  }

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  EraseInitStruct.Page        = FLASH_PAGE_NB - 1;
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.NbPages     = 1;

  EraseInitStruct.Banks  = FLASH_BANK_2;

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
  else
  {
    FLIGHT1_PRINTF("End FLASH Erase %lu Pages of %dBytes\r\n", EraseInitStruct.NbPages, FLASH_PAGE_SIZE);
  }

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

  if (CurrentActiveBank == 1)
  {
    Address = ADDRESS_META_DATA_BANK1;
  }

  if (CurrentActiveBank == 2)
  {
    Address = ADDRESS_META_DATA_BANK2;
  }

  BankInfoAddress = (uint32_t)&FirmwareMetaData;

  /* Disable instruction cache prior to internal cacheable memory update */
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    Error_Handler();
  }

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  for (Count = 0; Count < META_DATA_DIMENSION; Count += 16)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Address, BankInfoAddress) != HAL_OK)
    {
      FLIGHT1_PRINTF("Error HAL_FLASH_Program\r\n");
    }

    Address += 16;
    BankInfoAddress += 16;
  }

  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Re-enable instruction cache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
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

