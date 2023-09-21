/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Function.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.0.2
  * @date    20-June-2023
  * @brief   BLE function API definition
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
#include <stdio.h>
#include "BLE_Manager.h"
#include "BLE_Function.h"
#include "app_flight1.h"
#include "TargetFeatures.h"
#include "main.h"
#include "FLIGHT1_config.h"

#include "OTA.h"

#include "MetaDataManager.h"

/* Exported Variables --------------------------------------------------------*/
uint8_t connected= FALSE;

/* Private variables ------------------------------------------------------------*/
volatile uint32_t FeatureMask;

static uint16_t BLE_ConnectionHandle = 0;
static uint32_t OTA_RemainingSize=0;

/* Private functions ---------------------------------------------------------*/
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length);

/**
 * @brief  Set Board Name.
 * @param  None
 * @retval None
 */
void SetBoardName(void)
{
  sprintf(BLE_StackValue.BoardName,"%s%c%c%c","FL1V",
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
void BLE_SetCustomAdvertiseData(uint8_t *manuf_data)
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
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1]=0x06; /* Custom Firmware */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2]=0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3]=0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4]=0x00;
  /* USER CODE END 2 */
#endif /* BLE_MANAGER_SDKV2 */
}

/**
* @brief  This function makes the parsing of the Debug Console
* @param  uint8_t *att_data attribute data
* @param  uint8_t data_length length of the data
* @retval uint32_t SendBackData true/false
*/
uint32_t DebugConsoleParsing(uint8_t * att_data, uint8_t data_length)
{
  /* By default Answer with the same message received */
  uint32_t SendBackData =1;

  if(OTA_RemainingSize!=0) {
    /* FP-IND-PREDMNT1 firwmare update */
    int8_t RetValue = UpdateFW(&OTA_RemainingSize,att_data, data_length,1);
    if(RetValue!=0) {
      Term_Update((uint8_t *)&RetValue,1);
      if(RetValue==1) {
        /* if OTA checked */
        //BytesToWrite =sprintf((char *)BufferToWrite,"The Board will restart in 5 seconds\r\n");
        //Term_Update(BufferToWrite,BytesToWrite);
        FLIGHT1_PRINTF("%s will restart in 5 seconds\r\n",FLIGHT1_PACKAGENAME);
        HAL_Delay(5000);
        HAL_NVIC_SystemReset();
      }
    }
    SendBackData=0;
  }
  else
  {
    /* Received one write from Client on Terminal characteristc */
    SendBackData = DebugConsoleCommandParsing(att_data,data_length);
  }

  return SendBackData;
}

/**
 * @brief  This function makes the parsing of the Debug Console Commands
 * @param  uint8_t *att_data attribute data
 * @param  uint8_t data_length length of the data
 * @retval uint32_t SendBackData true/false
 */
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  uint32_t SendBackData = 1;

  /* Help Command */
  if(!strncmp("help",(char *)(att_data),4))
  {
    /* Print Legend */
    SendBackData=0;

    BytesToWrite =sprintf((char *)BufferToWrite,"Command:\r\n"
      "info-> System Info\r\n"
      "versionFw-> FW Version\r\n"
      "versionBle-> Ble Version\r\n"
	  "uid-> Write the STM32 UID\r\n"
	  "setName xxxxxxx-> Set the node name (Max 7 characters)\r\n");
    Term_Update(BufferToWrite,BytesToWrite);
  }
  else if(!strncmp("info",(char *)(att_data),4))
  {
    SendBackData=0;

    BytesToWrite =sprintf((char *)BufferToWrite,"\r\nSTMicroelectronics %s:\r\n"
        "\tVersion %c.%c.%c\r\n"
        "\tSTM32F401RE-Nucleo board"
         "\r\n",
         FLIGHT1_PACKAGENAME,
         FLIGHT1_VERSION_MAJOR,FLIGHT1_VERSION_MINOR,FLIGHT1_VERSION_PATCH);

    Term_Update(BufferToWrite,BytesToWrite);

    BytesToWrite =sprintf((char *)BufferToWrite,"\t(HAL %ld.%ld.%ld_%ld)\r\n"
        "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n",
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
        " (KEIL)\r\n",
#elif defined (__GNUC__)
        " (STM32CubeIDE)\r\n",
#endif
        HAL_GetHalVersion() >>24,
        (HAL_GetHalVersion() >>16)&0xFF,
        (HAL_GetHalVersion() >> 8)&0xFF,
        HAL_GetHalVersion()      &0xFF,
        __DATE__,__TIME__);

    Term_Update(BufferToWrite,BytesToWrite);

    BytesToWrite =sprintf((char *)BufferToWrite,"Code compiled for X-NUCLEO-53L3A2\r\n");
    Term_Update(BufferToWrite,BytesToWrite);
  }
  else if(!strncmp("versionFw",(char *)(att_data),9))
  {
    BytesToWrite =sprintf((char *)BufferToWrite,"%s_%s_%c.%c.%c\r\n",
                          BLE_STM32_MICRO,
                          FLIGHT1_PACKAGENAME,
                          FLIGHT1_VERSION_MAJOR,
                          FLIGHT1_VERSION_MINOR,
                          FLIGHT1_VERSION_PATCH);
    Term_Update(BufferToWrite,BytesToWrite);
    SendBackData=0;
  }
  else if(!strncmp("upgradeFw",(char *)(att_data),9))
  {
    uint32_t OTA_crc;
    uint8_t *PointerByte = (uint8_t*) &OTA_RemainingSize;

    OTA_RemainingSize=atoi((char *)(att_data+9));
    PointerByte[0]=att_data[ 9];
    PointerByte[1]=att_data[10];
    PointerByte[2]=att_data[11];
    PointerByte[3]=att_data[12];

    /* Check the Maximum Possible OTA size */
    if(OTA_RemainingSize>OTA_MAX_PROG_SIZE)
    {
      FLIGHT1_PRINTF("OTA %s SIZE=%ld > %ld Max Allowed\r\n",FLIGHT1_PACKAGENAME,OTA_RemainingSize, OTA_MAX_PROG_SIZE);
      /* Answer with a wrong CRC value for signaling the problem to BlueMS application */
      BufferToWrite[0]= att_data[13];
      BufferToWrite[1]=(att_data[14]!=0) ? 0 : 1;/* In order to be sure to have a wrong CRC */
      BufferToWrite[2]= att_data[15];
      BufferToWrite[3]= att_data[16];
      BytesToWrite = 4;
      Term_Update(BufferToWrite,BytesToWrite);
    }
    else
    {
      PointerByte = (uint8_t*) &OTA_crc;
      PointerByte[0]=att_data[13];
      PointerByte[1]=att_data[14];
      PointerByte[2]=att_data[15];
      PointerByte[3]=att_data[16];

      FLIGHT1_PRINTF("OTA %s SIZE=%ld OTA_crc=%lx\r\n",FLIGHT1_PACKAGENAME,OTA_RemainingSize,OTA_crc);

      /* Reset the Flash */
      StartUpdateFW(OTA_RemainingSize,OTA_crc);

      /* Reduce the connection interval */
      {
        int ret = aci_l2cap_connection_parameter_update_req(BLE_ConnectionHandle,
                                                      10 /* interval_min*/,
                                                      10 /* interval_max */,
                                                      0   /* slave_latency */,
                                                      400 /*timeout_multiplier*/);
        /* Go to infinite loop if there is one error */
        if (ret != BLE_STATUS_SUCCESS) {
          while (1) {
            FLIGHT1_PRINTF("Problem Changing the connection interval\r\n");
          }
        }
      }

      /* Signal that we are ready sending back the CRC value*/
      BufferToWrite[0] = PointerByte[0];
      BufferToWrite[1] = PointerByte[1];
      BufferToWrite[2] = PointerByte[2];
      BufferToWrite[3] = PointerByte[3];
      BytesToWrite = 4;
      Term_Update(BufferToWrite,BytesToWrite);
    }

    SendBackData=0;
  }
  else if(!strncmp("versionBle",(char *)(att_data),10))
  {
    uint8_t  hwVersion;
    uint16_t fwVersion;
    /* get the BlueNRG HW and FW versions */
    getBlueNRGVersion(&hwVersion, &fwVersion);
      BytesToWrite =sprintf((char *)BufferToWrite,"%s_%d.%d.%c\r\n",
                            "BlueNRG2",
                            (fwVersion>>8)&0xF,
                            (fwVersion>>4)&0xF,
                            ('a' + (fwVersion&0xF)));
    Term_Update(BufferToWrite,BytesToWrite);
    SendBackData=0;
  }
  else if((att_data[0]=='u') & (att_data[1]=='i') & (att_data[2]=='d'))
  {
    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)BLE_STM32_UUID;
    uint32_t MCU_ID = BLE_STM32_MCU_ID[0]&0xFFF;
    BytesToWrite =sprintf((char *)BufferToWrite,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                          uid[ 3],uid[ 2],uid[ 1],uid[ 0],
                          uid[ 7],uid[ 6],uid[ 5],uid[ 4],
                          uid[11],uid[ 10],uid[9],uid[8],
                          MCU_ID);
    Term_Update(BufferToWrite,BytesToWrite);
    SendBackData=0;
  }
  else if(!strncmp("setName ",(char *)(att_data),8)) {

      //int NameLength= strcspn((const char *)att_data,"\n");
      int NameLength= data_length -1;

      if(NameLength > 8)
      {
        for(int i=1;i<8;i++)
          NodeName[i]= atoi(" ");

        if((NameLength - 8) > 7)
          NameLength= 7;
        else NameLength= NameLength - 8;

        for(int i=1;i<NameLength+1;i++)
          NodeName[i]= att_data[i+7];

        MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
        NecessityToSaveMetaDataManager=1;

        BytesToWrite =sprintf((char *)BufferToWrite,"\nThe node nome has been updated\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
        BytesToWrite =sprintf((char *)BufferToWrite,"Disconnecting and riconnecting to see the new node name\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
      }
      else
      {
        BytesToWrite =sprintf((char *)BufferToWrite,"\nInsert the node name\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
        BytesToWrite =sprintf((char *)BufferToWrite,"Use command: setName 'xxxxxxx'\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
      }

      SendBackData=0;
  }

  return SendBackData;
}

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None
 * @retval None
 */
void DisconnectionCompletedFunction(void)
{
  connected = FALSE;

  LedEnabled= 0;

  /* Enable timer for led blinking */
  if(!LedTimerEnabled) {
    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_Base_Start_IT(&TimLedHandle) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }

    LedTimerEnabled= 1;
  }

  /* Disable all timer */
  if(ObjectsDetectionTimerEnabled)
  {
    /* Stop the TIM Base generation in interrupt mode */
    if(HAL_TIM_Base_Stop_IT(&TimDistanceHandle) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }

    StopMeasurement();

    ObjectsDetectionTimerEnabled= 0;
  }

  /* Reset for any problem during FOTA update */
  OTA_RemainingSize = 0;

  FLIGHT1_PRINTF("Call to DisconnectionCompletedFunction\r\n");
  HAL_Delay(100);
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  None
 * @retval None
 */
void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6])
{
 BLE_ConnectionHandle = ConnectionHandle;

  connected = TRUE;

  /* Disable timer for led blinking */
  if(LedTimerEnabled) {
    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_Base_Stop_IT(&TimLedHandle) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }

    LedTimerEnabled= 0;
  }

  FLIGHT1_PRINTF("Call to ConnectionCompletedFunction\r\n");
  HAL_Delay(100);
}

/**
* @brief  This function makes the parsing of the Configuration Commands
* @param uint8_t *att_data attribute data
* @param uint8_t data_length length of the data
* @retval None
*/
void WriteRequestConfigFunction(uint8_t * att_data, uint8_t data_length)
{
  uint32_t FeatureMask = (att_data[3]) | (att_data[2]<<8) | (att_data[1]<<16) | (att_data[0]<<24);
  uint8_t Command = att_data[4];
  uint8_t Data    = att_data[5];

  switch (FeatureMask) {
    case FEATURE_MASK_LED:
      /* Led events */
#ifdef FLIGHT1_DEBUG_CONNECTION
      if(BLE_StdTerm_Service==BLE_SERV_ENABLE) {
        BytesToWrite = sprintf((char *)BufferToWrite,"Conf Sig F=%lx C=%2x\n\r",FeatureMask,Command);
        Term_Update(BufferToWrite,BytesToWrite);
      } else {
        FLIGHT1_PRINTF("Conf Sig F=%lx C=%2x\r\n",FeatureMask,Command);
      }
#endif /* FLIGHT1_DEBUG_CONNECTION */
     switch(Command) {
      case 1:
        LedOnTargetPlatform();
        Config_Update(FEATURE_MASK_LED,Command,Data);
        break;
      case 0:
        LedOffTargetPlatform();
        Config_Update(FEATURE_MASK_LED,Command,Data);
        break;
     }
     /* Update the LED feature */
     if(LedEnabled) {
       BLE_LedStatusUpdate(TargetBoardFeatures.LedStatus);
     }
    break;
  }
}

/**************************************************
 * Callback functions to manage the notify events *
 **************************************************/
/**
  * @brief  Enable/Disable BLE Features
  * @param  None
  * @retval None
  */
void NotifyEventLed(BLE_NotifyEvent_t Event)
{
  /* Led Features */
  if(Event == BLE_NOTIFY_SUB)
  {
    LedEnabled= 1;
    BLE_LedStatusUpdate(TargetBoardFeatures.LedStatus);
  }

  if(Event == BLE_NOTIFY_UNSUB)
    LedEnabled= 0;
}

void NotifyEventObjectsDetection(BLE_NotifyEvent_t Event)
{
  if( (Event == BLE_NOTIFY_SUB) &&
      (!ObjectsDetectionTimerEnabled) )
  {
    StartMeasurement();

    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_Base_Start_IT(&TimDistanceHandle) != HAL_OK){
      /* Starting Error */
      Error_Handler();
    }

    ObjectsDetectionTimerEnabled= 1;
  }

  if( (Event == BLE_NOTIFY_UNSUB) &&
      (ObjectsDetectionTimerEnabled) )
  {
    if(HAL_TIM_Base_Stop_IT(&TimDistanceHandle) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }

    StopMeasurement();

    ObjectsDetectionTimerEnabled= 0;
  }
}

/***********************************************************************************
 * Callback functions to manage the extended configuration characteristic commands *
 ***********************************************************************************/
/**
 * @brief  Callback Function for answering to VersionFw command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
void ExtConfigVersionFwCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"%s_%s_%c.%c.%c",
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
void ExtConfigInfoCommandCallback(uint8_t *Answer)
{
  uint8_t  hwVersion;
  uint16_t fwVersion;

  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  sprintf((char *)Answer,"STMicroelectronics %s:\n"
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
#endif
    "\nCode compiled for X-NUCLEO-53L3A2 board\n",

    FLIGHT1_PACKAGENAME,
    FLIGHT1_VERSION_MAJOR,
    FLIGHT1_VERSION_MINOR,
    FLIGHT1_VERSION_PATCH,
    BLE_STM32_BOARD,
    ((hwVersion>>4)&0x0F),
    (hwVersion&0x0F),
    (fwVersion>>8)&0xF,
    (fwVersion>>4)&0xF,
    ('a' + (fwVersion&0xF)),
    HAL_GetHalVersion() >>24,
    (HAL_GetHalVersion() >>16)&0xFF,
    (HAL_GetHalVersion() >> 8)&0xFF,
    HAL_GetHalVersion()      &0xFF,
    __DATE__,__TIME__);
}

/**
 * @brief  Callback Function for answering to Help command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
void ExtConfigHelpCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"List of available command:\n"
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
void ExtConfigSetNameCommandCallback(uint8_t *NewName)
{
  FLIGHT1_PRINTF("New Board Name = <%s>\r\n", NewName);
  /* Change the Board Name */
  sprintf(BLE_StackValue.BoardName,"%s",NewName);

  for(int i=0; i<7; i++)
    NodeName[i+1]= BLE_StackValue.BoardName[i];

  MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
  NecessityToSaveMetaDataManager=1;

  FLIGHT1_PRINTF("\nThe node nome has been updated\r\n");
  FLIGHT1_PRINTF("Disconnecting and riconnecting to see the new node name\r\n");
}
