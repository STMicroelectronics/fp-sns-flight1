/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ble_function.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @brief   BLE function API definition
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
#include <stdio.h>
#include "ble_manager.h"
#include "ble_function.h"
#include "app_flight1.h"
#include "target_features.h"
#include "main.h"
#include "flight1_config.h"

/* Exported Variables --------------------------------------------------------*/
uint8_t connected = FALSE;

/* Private variables ------------------------------------------------------------*/
volatile uint32_t FeatureMask;

/* Private functions ---------------------------------------------------------*/
static uint32_t DebugConsoleCommandParsing(uint8_t *att_data, uint8_t data_length);

/**
  * @brief  Set Board Name.
  * @param  None
  * @retval None
  */
void set_board_name(void)
{
  sprintf(ble_stack_value.board_name, "%s%c%c%c", "FL1V",
          FLIGHT1_VERSION_MAJOR,
          FLIGHT1_VERSION_MINOR,
          FLIGHT1_VERSION_PATCH);

  /* Set Node Name */
  ReCallNodeNameFromMemory();
}

/**
  * @brief  Set Custom Advertize Data.
  * @param  uint8_t *manuf_data: Advertize Data
  * @retval None
  */
void ble_set_custom_advertise_data(uint8_t *manuf_data)
{
#ifndef BLE_MANAGER_SDKV2
  /**
    * For only SDKV1, user can add here the custom advertize data setting for the selected BLE features.
    * For example for the environmental features:
    *
    * BLE_SetCustomEnvAdvertizeData(manuf_data);
  */

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

#else /* BLE_MANAGER_SDKV2 */
  /* USER CODE BEGIN 2 */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1] = CUSTOM_FIRMWARE_ID; /* Custom Firmware */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2] = 0x0F;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4] = 0x00;
  /* USER CODE END 2 */
#endif /* BLE_MANAGER_SDKV2 */
}

/**
  * @brief  This function makes the parsing of the Debug Console
  * @param  uint8_t *att_data attribute data
  * @param  uint8_t data_length length of the data
  * @retval uint32_t SendBackData true/false
  */
uint32_t debug_console_parsing(uint8_t *att_data, uint8_t data_length)
{
  /* By default Answer with the same message received */
  uint32_t SendBackData = 1;

  {
    /* Received one write from Client on Terminal characteristc */
    SendBackData = DebugConsoleCommandParsing(att_data, data_length);
  }

  return SendBackData;
}

/**
  * @brief  This function makes the parsing of the Debug Console Commands
  * @param  uint8_t *att_data attribute data
  * @param  uint8_t data_length length of the data
  * @retval uint32_t SendBackData true/false
  */
static uint32_t DebugConsoleCommandParsing(uint8_t *att_data, uint8_t data_length)
{
  uint32_t SendBackData = 1;

  /* Help Command */
  if (!strncmp("help", (char *)(att_data), 4))
  {
    /* Print Legend */
    SendBackData = 0;
    /* List of the available commands */
    bytes_to_write = sprintf((char *)buffer_to_write, "Command:\r\n"
                             "info-> System Info\r\n"
                             "versionBle-> Ble Version\r\n"
                             "uid-> Write the STM32 UID\r\n"
                             "setName xxxxxxx-> Set the node name (Max 7 characters)\r\n");
    term_update(buffer_to_write, bytes_to_write);
  }
  /* info command */
  else if (!strncmp("info", (char *)(att_data), 4))
  {
    SendBackData = 0;

    bytes_to_write = sprintf((char *)buffer_to_write, "\r\nSTMicroelectronics %s:\r\n"
                             "\tVersion %c.%c.%c\r\n"
                             "\tSTM32F401RE-Nucleo board"
                             "\r\n",
                             FLIGHT1_PACKAGENAME,
                             FLIGHT1_VERSION_MAJOR, FLIGHT1_VERSION_MINOR, FLIGHT1_VERSION_PATCH);

    term_update(buffer_to_write, bytes_to_write);

    bytes_to_write = sprintf((char *)buffer_to_write, "\t(HAL %ld.%ld.%ld_%ld)\r\n"
                             "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
                             " (IAR)\r\n",
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
                             " (KEIL)\r\n",
#elif defined (__GNUC__)
                             " (STM32CubeIDE)\r\n",
#endif /* IDE */
                             HAL_GetHalVersion() >> 24,
                             (HAL_GetHalVersion() >> 16) & 0xFF,
                             (HAL_GetHalVersion() >> 8) & 0xFF,
                             HAL_GetHalVersion()      & 0xFF,
                             __DATE__, __TIME__);

    term_update(buffer_to_write, bytes_to_write);

    bytes_to_write = sprintf((char *)buffer_to_write, "Code compiled for CUSTOM board\r\n");
    term_update(buffer_to_write, bytes_to_write);
  }
  /* versionBle command */
  else if (!strncmp("versionBle", (char *)(att_data), 10))
  {
    uint8_t  hwVersion;
    uint16_t fwVersion;
    /* get the BlueNRG HW and FW versions */
    get_blue_nrg_version(&hwVersion, &fwVersion);
    bytes_to_write = sprintf((char *)buffer_to_write, "%s_%d.%d.%c\r\n",
                             "BlueNRG2",
                             (fwVersion >> 8) & 0xF,
                             (fwVersion >> 4) & 0xF,
                             ('a' + (fwVersion & 0xF)));
    term_update(buffer_to_write, bytes_to_write);
    SendBackData = 0;
  }
  /* UID command */
  else if ((att_data[0] == 'u') & (att_data[1] == 'i') & (att_data[2] == 'd'))
  {
    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)BLE_STM32_UUID;
    uint32_t MCU_ID = BLE_STM32_MCU_ID[0] & 0xFFF;
    bytes_to_write = sprintf((char *)buffer_to_write, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                             uid[ 3], uid[ 2], uid[ 1], uid[ 0],
                             uid[ 7], uid[ 6], uid[ 5], uid[ 4],
                             uid[11], uid[ 10], uid[9], uid[8],
                             MCU_ID);
    term_update(buffer_to_write, bytes_to_write);
    SendBackData = 0;
  }
  /* setName command */
  else if (!strncmp("setName ", (char *)(att_data), 8))
  {
    uint16_t NameLength = data_length - 1;

    if (NameLength > 8)
    {
      for (uint16_t i = 0; i < 7; i++)
      {
        ble_stack_value.board_name[i] = atoi(" ");
      }

      if ((NameLength - 8) > 7)
      {
        NameLength = 7;
      }
      else NameLength = NameLength - 8;

      for (uint16_t i = 0; i < NameLength; i++)
      {
        ble_stack_value.board_name[i] = att_data[i + 8];
      }

      UpdateNodeNameMetaData();

      bytes_to_write = sprintf((char *)buffer_to_write, "\nThe node name has been updated\r\n");
      term_update(buffer_to_write, bytes_to_write);
      bytes_to_write = sprintf((char *)buffer_to_write, "Disconnecting and riconnecting to see the new node name\r\n");
      term_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      bytes_to_write = sprintf((char *)buffer_to_write, "\nInsert the node name\r\n");
      term_update(buffer_to_write, bytes_to_write);
      bytes_to_write = sprintf((char *)buffer_to_write, "Use command: setName 'xxxxxxx'\r\n");
      term_update(buffer_to_write, bytes_to_write);
    }

    SendBackData = 0;
  }

  return SendBackData;
}

/**
  * @brief  This function is called when the peer device get disconnected.
  * @param  None
  * @retval None
  */
void disconnection_completed_function(void)
{
  connected = FALSE;

  LedEnabled = 0;

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

  /* Disable all timer */
  if (ObjectsDetectionTimerEnabled)
  {
    /* Stop the TIM Base generation in interrupt mode */
    if (HAL_TIM_Base_Stop_IT(&TIM_DISTANCE_HANDLE) != HAL_OK)
    {
      /* Stopping Error */
      Error_Handler();
    }

    StopMeasurement();

    ObjectsDetectionTimerEnabled = 0;
  }

  FLIGHT1_PRINTF("Call to disconnection_completed_function\r\n");
  HAL_Delay(100);
}

/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  None
  * @retval None
  */
void connection_completed_function(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6])
{

  connected = TRUE;

  /* Disable timer for led blinking */
  if (LedTimerEnabled)
  {
    /* Stop the TIM Base generation in interrupt mode */
    if (HAL_TIM_Base_Stop_IT(&TIM_LED_HANDLE) != HAL_OK)
    {
      /* Stopping Error */
      Error_Handler();
    }

    BSP_LED_Off(LED2);
    TargetBoardFeatures.LedStatus = 0;

    LedTimerEnabled = 0;
  }

  FLIGHT1_PRINTF("Call to connection_completed_function\r\n");
  HAL_Delay(100);
}

/**
  * @brief  This function makes the parsing of the Configuration Commands
  * @param uint8_t *att_data attribute data
  * @param uint8_t data_length length of the data
  * @retval None
  */
void write_request_config_function(uint8_t *att_data, uint8_t data_length)
{
  uint32_t FeatureMask = (att_data[3]) | (att_data[2] << 8) | (att_data[1] << 16) | (att_data[0] << 24);
  uint8_t Command = att_data[4];
  uint8_t Data    = att_data[5];

  switch (FeatureMask)
  {
    case FEATURE_MASK_LED:
      /* Led events */
#ifdef FLIGHT1_DEBUG_CONNECTION
      if (ble_std_term_service == BLE_SERV_ENABLE)
      {
        bytes_to_write = sprintf((char *)buffer_to_write, "Conf Sig F=%lx C=%2x\n\r", FeatureMask, Command);
        term_update(buffer_to_write, bytes_to_write);
      }
      else
      {
        FLIGHT1_PRINTF("Conf Sig F=%lx C=%2x\r\n", FeatureMask, Command);
      }
#endif /* FLIGHT1_DEBUG_CONNECTION */
      switch (Command)
      {
        case 1:
          LedOnTargetPlatform();
          config_update(FEATURE_MASK_LED, Command, Data);
          break;
        case 0:
          LedOffTargetPlatform();
          config_update(FEATURE_MASK_LED, Command, Data);
          break;
      }
      /* Update the LED feature */
      if (LedEnabled)
      {
        ble_led_status_update(TargetBoardFeatures.LedStatus);
      }
      break;
  }
}

/***************************************************
  * Callback functions to manage the notify events *
  **************************************************/
/**
  * @brief  Enable/Disable BLE Features
  * @param  ble_notify_event_t Event: Subscription/Unsubscription BLE event
  * @retval None
  */
void notify_event_led(ble_notify_event_t Event)
{
  /* Led Features */
  if (Event == BLE_NOTIFY_SUB)
  {
    LedEnabled = 1;
    ble_led_status_update(TargetBoardFeatures.LedStatus);
  }

  if (Event == BLE_NOTIFY_UNSUB)
  {
    LedEnabled = 0;
  }
}

/**
  * @brief  Callback Function for Led read request.
  * @param  uint8_t *led_status Status of the led
  * @retval None
  */
void read_request_led_function(uint8_t *led_status)
{
  *led_status = TargetBoardFeatures.LedStatus;
}

/**
  * @brief  Enable/Disable BLE Features
  * @param  ble_notify_event_t Event: Subscription/Unsubscription BLE event
  * @retval None
  */
void notify_event_objects_detection(ble_notify_event_t Event)
{
  if ((Event == BLE_NOTIFY_SUB) &&
      (!ObjectsDetectionTimerEnabled))
  {
    StartMeasurement();

    /* Start the TIM Base generation in interrupt mode */
    if (HAL_TIM_Base_Start_IT(&TIM_DISTANCE_HANDLE) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }

    ObjectsDetectionTimerEnabled = 1;
  }

  if ((Event == BLE_NOTIFY_UNSUB) &&
      (ObjectsDetectionTimerEnabled))
  {
    if (HAL_TIM_Base_Stop_IT(&TIM_DISTANCE_HANDLE) != HAL_OK)
    {
      /* Stopping Error */
      Error_Handler();
    }

    StopMeasurement();

    ObjectsDetectionTimerEnabled = 0;
  }
}

/************************************************************************************
  * Callback functions to manage the extended configuration characteristic commands *
  ***********************************************************************************/
/**
  * @brief  Callback Function for answering to VersionFw command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
void ext_config_version_fw_command_callback(uint8_t *Answer)
{
  /* Write package name and version firmware info */
  sprintf((char *)Answer, "%s_%s_%c.%c.%c",
          BLE_STM32_MICRO,
          FLIGHT1_PACKAGENAME,
          FLIGHT1_VERSION_MAJOR,
          FLIGHT1_VERSION_MINOR,
          FLIGHT1_VERSION_PATCH);
}

/**
  * @brief  Callback Function for answering to Info command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
void ext_config_info_command_callback(uint8_t *Answer)
{
  /* BlueNRG HW version info */
  uint8_t  hwVersion;
  /* BlueNRG FW version info */
  uint16_t fwVersion;

  /* get the BlueNRG HW and FW versions */
  get_blue_nrg_version(&hwVersion, &fwVersion);
  /* Write firmware info */
  sprintf((char *)Answer, "STMicroelectronics %s:\n"
          "Version %c.%c.%c\n"
          "%s board\n"
          "BlueNRG-2 HW ver%d.%d\n"
          "BlueNRG-2 FW ver%d.%d.%c\n"
          "(HAL %ld.%ld.%ld_%ld)\n"
          "Compiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
          " (IAR)"
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
          " (KEIL)"
#elif defined (__GNUC__)
          " (STM32CubeIDE)"
#endif /* IDE */
          "\nCode compiled for CUSTOM board\n",
          FLIGHT1_PACKAGENAME,
          FLIGHT1_VERSION_MAJOR,
          FLIGHT1_VERSION_MINOR,
          FLIGHT1_VERSION_PATCH,
          BLE_STM32_BOARD,
          ((hwVersion >> 4) & 0x0F),
          (hwVersion & 0x0F),
          (fwVersion >> 8) & 0xF,
          (fwVersion >> 4) & 0xF,
          ('a' + (fwVersion & 0xF)),
          HAL_GetHalVersion() >> 24,
          (HAL_GetHalVersion() >> 16) & 0xFF,
          (HAL_GetHalVersion() >> 8) & 0xFF,
          HAL_GetHalVersion()      & 0xFF,
          __DATE__, __TIME__);
}

/**
  * @brief  Callback Function for answering to Help command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
void ext_config_help_command_callback(uint8_t *Answer)
{
  /* List of available command */
  sprintf((char *)Answer, "List of available command:\n"
          "1) Board Report\n"
          "- STM32 UID\n"
          "- Version Firmware\n"
          "- Info\n"
          "- Help\n\n"
          "2) Board Settings\n"
          "- Set Name\n");
}

/**
  * @brief  Callback Function for managing the SetName command
  * @param  uint8_t *NewName
  * @retval None
  */
void ext_config_set_name_command_callback(uint8_t *NewName)
{
  FLIGHT1_PRINTF("New Board Name = <%s>\r\n", NewName);
  /* Change the Board Name */
  sprintf(ble_stack_value.board_name, "%s", NewName);

  UpdateNodeNameMetaData();

  FLIGHT1_PRINTF("\nThe node name has been updated\r\n");
  FLIGHT1_PRINTF("Disconnecting and riconnecting to see the new node name\r\n");
}

