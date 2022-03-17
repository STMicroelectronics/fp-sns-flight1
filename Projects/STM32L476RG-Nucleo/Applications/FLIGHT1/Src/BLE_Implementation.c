/**
  ******************************************************************************
  * @file    BLE_Implementation.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   BLE Implementation template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.c.
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "BLE_Manager.h"
#include "OTA.h"

/* Exported Variables --------------------------------------------------------*/
uint8_t connected= FALSE;
int MasterTrusted = FALSE;
int32_t  NeedToClearSecureDB=0;

/* Private variables ------------------------------------------------------------*/
volatile uint32_t FeatureMask;
static uint16_t BLE_ConnectionHandle = 0;
static uint32_t SizeOfUpdateBlueFW=0;

#ifdef FLIGHT1_SECURE_CONNECTION
static uint8_t TrustedAddr[6];
static uint8_t CurrentAddr[6];
#endif /* FLIGHT1_SECURE_CONNECTION */
      
/* Private functions ---------------------------------------------------------*/
static uint32_t DebugConsoleParsing(uint8_t * att_data, uint8_t data_length);
//static void ReadRequestEnvFunction(void);
static void PairingCompletedFunction(uint8_t PairingStatus);

#ifdef FLIGHT1_SECURE_CONNECTION
static void SetConnectableFunction(uint8_t *ManufData);
#endif /* FLIGHT1_SECURE_CONNECTION */

static void DisconnectionCompletedFunction(void);
static void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t addr[6]);

static void WriteRequestConfigFunction(uint8_t * att_data, uint8_t data_length);
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length);

static void NotifyEventObjectsDetection(BLE_NotifyEvent_t Event);
static void NotifyEventLed(BLE_NotifyEvent_t Event);

/**********************************************************************************************
 * Callback functions prototypes to manage the extended configuration characteristic commands *
 **********************************************************************************************/
static void ExtExtConfigUidCommandCallback(uint8_t **UID);
static void ExtConfigInfoCommandCallback(uint8_t *Answer);
static void ExtConfigHelpCommandCallback(uint8_t *Answer);
static void ExtConfigVersionFwCommandCallback(uint8_t *Answer);

static void ExtConfigSetNameCommandCallback(uint8_t *NewName);

/** @brief Initialize the BlueNRG stack and services
  * @param  None
  * @retval None
  */
void BluetoothInit(void)
{
  /* BlueNRG stack setting */
  BLE_StackValue.IO_capabilities                    = IO_CAP_DISPLAY_ONLY;
  BLE_StackValue.GAP_Roles                          = GAP_PERIPHERAL_ROLE_IDB05A1;
  BLE_StackValue.MITM_ProtectionRequirements        = MITM_PROTECTION_REQUIRED;
  BLE_StackValue.Out_Of_Band_EnableData             = OOB_AUTH_DATA_ABSENT;
  BLE_StackValue.oob_data                           = NULL;
  BLE_StackValue.AuthenticationRequirements         = NO_BONDING;
  BLE_StackValue.ConfigValueOffsets                 = CONFIG_DATA_PUBADDR_OFFSET;
  BLE_StackValue.ConfigValuelength                  = CONFIG_DATA_PUBADDR_LEN;
  
  /* Use BLE Random Address */
  BLE_StackValue.OwnAddressType = RANDOM_ADDR;


  /* To set the TX power level of the bluetooth device ( -2,1 dBm )*/
  BLE_StackValue.EnableHighPowerMode= 1; /*  High Power */
  
  /* Values: 0x00 ... 0x31 - The value depends on the device */
  BLE_StackValue.PowerAmplifierOutputLevel =4;
  
  /* BlueNRG services setting */
  BLE_StackValue.EnableConfig    = 1;
  BLE_StackValue.EnableConsole   = 1;
  BLE_StackValue.EnableExtConfig = 1;
  
  /* For Enabling the Secure Connection */
  BLE_StackValue.EnableSecureConnection=0;
  /* Default Secure PIN */
  BLE_StackValue.SecurePIN=123456;
  /* For creating a Random Secure PIN */
  BLE_StackValue.EnableRandomSecurePIN = 0;
  
  /* Load Bluetooth NDEF Table in NFC TAG */
#ifdef FLIGHT1_SECURE_CONNECTION   
  if (TargetBoardFeatures.NFC_TAG_support)
  {
    /* For Enabling the Secure Connection */
    BLE_StackValue.EnableSecureConnection=1;
    /* For creating a Random Secure PIN */
    BLE_StackValue.EnableRandomSecurePIN = 1;
    
    for(int i=0;i<6;i++) {
      TrustedAddr[i]= 0;
    }
  }
#endif /* FLIGHT1_SECURE_CONNECTION */
  
  BLE_StackValue.AdvertisingFilter    = NO_WHITE_LIST_USE;
  
  if(BLE_StackValue.EnableSecureConnection) {
    /* Using the Secure Connection, the Rescan should be done by BLE chip */    
    BLE_StackValue.ForceRescan =0;
  } else {
    BLE_StackValue.ForceRescan =1;
  }
  
  InitBleManager();
}

/**
 * @brief  Custom Service Initialization.
 * @param  None
 * @retval None
 */
void BLE_InitCustomService(void) {
  /* Define Custom Function for Debug Console Command parsing */
  CustomDebugConsoleParsingCallback = &DebugConsoleParsing;
  
  /* Define Custom Function for Pairing Completed */
  CustomPairingCompleted = &PairingCompletedFunction;
  
#ifdef FLIGHT1_SECURE_CONNECTION
  /* Define Custom Function for Connection Completed */
  CustomSetConnectable = &SetConnectableFunction;
#endif /* FLIGHT1_SECURE_CONNECTION */
  
  /* Define Custom Function for Connection Completed */
  CustomConnectionCompleted = &ConnectionCompletedFunction;
  
  /* Define Custom Function for Disconnection Completed */
  CustomDisconnectionCompleted = &DisconnectionCompletedFunction;
  
  /* Define Custom Command for Parsing Write on Config Char */
  CustomWriteRequestConfigCallback = &WriteRequestConfigFunction;
  
  CustomNotifyEventObjectsDetection= &NotifyEventObjectsDetection;
  CustomNotifyEventLed= &NotifyEventLed;
  
  /***********************************************************************************
   * Callback functions to manage the extended configuration characteristic commands *
   ***********************************************************************************/
  CustomExtConfigUidCommandCallback  = &ExtExtConfigUidCommandCallback;
  CustomExtConfigInfoCommandCallback = &ExtConfigInfoCommandCallback;
  CustomExtConfigHelpCommandCallback = &ExtConfigHelpCommandCallback;
  CustomExtConfigVersionFwCommandCallback = &ExtConfigVersionFwCommandCallback;
  
  CustomExtConfigSetNameCommandCallback = &ExtConfigSetNameCommandCallback;
  
  /**
  * For each features, user can assign here the pointer at the function for the read request data.
  * For example for the environmental features:
  * 
  * CustomReadRequestEnvFunctionPointer = &ReadRequestEnvFunction;
  * 
  * User can define and insert in the BLE_Implementation.c source code the functions for the read request data
  * ReadRequestEnvFunction function is already defined.
  *
  */
  
//  /* Define Custom Function for Read Request Environmental Data */
//  CustomReadRequestEnvFunctionPointer = &ReadRequestEnvFunction;
  
  /*******************
   * User code begin *
   *******************/
  
  /**
  * User can added here the custom service initialization for the selected BLE features.
  * For example for the environmental features:
  * 
  * //BLE_InitEnvService(PressEnable,HumEnable,NumTempEnabled)
  * BleManagerAddChar(BleCharPointer= BLE_InitEnvService(1, 1, 1));
  */
  
  /* Service initialization and adding for the Led features */
  BleManagerAddChar(BLE_InitLedService());
  
  /* Service initialization and adding for the Objects Detection features */
  BleManagerAddChar(BLE_InitObjectsDetectionService());
  
  /*****************
   * User code end *
   *****************/
}

/**
 * @brief  Set Custom Advertise Data.
 * @param  uint8_t *manuf_data: Advertise Data
 * @retval None
 */
void BLE_SetCustomAdvertiseData(uint8_t *manuf_data)
{
#ifndef BLE_MANAGER_SDKV2
  /**
  * User can add here the custom advertise data setting  for the selected BLE features.
  * For example for the environmental features:
  * 
  * BLE_SetCustomEnvAdvertiseData(manuf_data);
  */
  
  /* Custom advertise data setting for the led features */
  BLE_SetLedAdvertiseData(manuf_data);
  
  /* Custom advertise data setting for the Objects Detection features */
  BLE_SetObjectsDetectionAdvertiseData(manuf_data);
  
#else /* BLE_MANAGER_SDKV2 */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1]=0x03; /* Firmware ID */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2]=0x03; /* Dummy */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3]=0x33; /* Dummy */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4]=0x44; /* Dummy */
#endif /* BLE_MANAGER_SDKV2 */
}

/**
 * @brief  This function makes the parsing of the Configuration Commands
 * @param  uint8_t *att_data attribute data
 * @param  uint8_t data_length length of the data
 * @retval None
 */
static void WriteRequestConfigFunction(uint8_t * att_data, uint8_t data_length)
{
  FeatureMask = (att_data[3]) | (att_data[2]<<8) | (att_data[1]<<16) | (att_data[0]<<24);
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

/**
* @brief  This function makes the parsing of the Debug Console
* @param  uint8_t *att_data attribute data
* @param  uint8_t data_length length of the data
* @retval uint32_t SendBackData true/false
*/
static uint32_t DebugConsoleParsing(uint8_t * att_data, uint8_t data_length)
{
  /* By default Answer with the same message received */
  uint32_t SendBackData =1; 
  
  if(SizeOfUpdateBlueFW!=0) {
    /* FP-IND-PREDMNT1 firwmare update */
    int8_t RetValue = UpdateFWBlueMS(&SizeOfUpdateBlueFW,att_data, data_length,1);
    if(RetValue!=0) {
      Term_Update((uint8_t *)&RetValue,1);
      if(RetValue==1) {
        /* if OTA checked */
        //BytesToWrite =sprintf((char *)BufferToWrite,"The Board will restart in 5 seconds\r\n");
        //Term_Update(BufferToWrite,BytesToWrite);
        BLE_MANAGER_PRINTF("%s will restart in 5 seconds\r\n",BLE_FW_PACKAGENAME );
        HAL_Delay(5000);
        HAL_NVIC_SystemReset();
      }
    }
    SendBackData=0;
  } else {
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
  
  //static uint8_t SetVibrParam= 0;

    /* Help Command */
    if(!strncmp("help",(char *)(att_data),4)) {
    /* Print Legend */
    SendBackData=0;

    BytesToWrite =sprintf((char *)BufferToWrite,"Command:\r\n"
      "info-> System Info\r\n"
      "versionFw-> FW Version\r\n"
      "versionBle-> Ble Version\r\n");   
    Term_Update(BufferToWrite,BytesToWrite);
    } else if(!strncmp("versionFw",(char *)(att_data),9)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"%s_%s_%c.%c.%c\r\n",
                            BLE_STM32_MICRO,
                            BLE_FW_PACKAGENAME,
                            BLE_VERSION_FW_MAJOR,
                            BLE_VERSION_FW_MINOR,
                            BLE_VERSION_FW_PATCH);

      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0;
    } else if(!strncmp("info",(char *)(att_data),4)) {
      SendBackData=0;
      
      BytesToWrite =sprintf((char *)BufferToWrite,"\r\nSTMicroelectronics %s:\r\n"
          "\tVersion %c.%c.%c\r\n"
          "\tSTM32L476RG-Nucleo board"
          "\r\n",
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,BLE_VERSION_FW_MINOR,BLE_VERSION_FW_PATCH);
      Term_Update(BufferToWrite,BytesToWrite);

      BytesToWrite =sprintf((char *)BufferToWrite,"\t(HAL %ld.%ld.%ld_%ld)\r\n"
        "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n",
#elif defined (__CC_ARM)
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
    }  if(!strncmp("upgradeFw",(char *)(att_data),9)) {
      uint32_t uwCRCValue;
      uint8_t *PointerByte = (uint8_t*) &SizeOfUpdateBlueFW;

      SizeOfUpdateBlueFW=atoi((char *)(att_data+9));
      PointerByte[0]=att_data[ 9];
      PointerByte[1]=att_data[10];
      PointerByte[2]=att_data[11];
      PointerByte[3]=att_data[12];

      /* Check the Maximum Possible OTA size */
      if(SizeOfUpdateBlueFW>OTA_MAX_PROG_SIZE) {
        BLE_MANAGER_PRINTF("OTA %s SIZE=%ld > %d Max Allowed\r\n",BLE_FW_PACKAGENAME,SizeOfUpdateBlueFW, OTA_MAX_PROG_SIZE);
        /* UserAnswer with a wrong CRC value for signaling the problem to BlueMS application */
        PointerByte[0]= att_data[13];
        PointerByte[1]=(att_data[14]!=0) ? 0 : 1;/* In order to be sure to have a wrong CRC */
        PointerByte[2]= att_data[15];
        PointerByte[3]= att_data[16];
        BytesToWrite = 4;
        Term_Update(BufferToWrite,BytesToWrite);
      } else {
        PointerByte = (uint8_t*) &uwCRCValue;
        PointerByte[0]=att_data[13];
        PointerByte[1]=att_data[14];
        PointerByte[2]=att_data[15];
        PointerByte[3]=att_data[16];

        BLE_MANAGER_PRINTF("OTA %s SIZE=%ld uwCRCValue=%lx\r\n",BLE_FW_PACKAGENAME,SizeOfUpdateBlueFW,uwCRCValue);
	  
        /* Reset the Flash */
        StartUpdateFWBlueMS(SizeOfUpdateBlueFW,uwCRCValue);

        /* Reduce the connection interval */
        {
          int ret = aci_l2cap_connection_parameter_update_request(BLE_ConnectionHandle,
                                                        10 /* interval_min*/,
                                                        10 /* interval_max */,
                                                        0   /* slave_latency */,
                                                        400 /*timeout_multiplier*/);
          /* Go to infinite loop if there is one error */
          if (ret != BLE_STATUS_SUCCESS) {
            while (1) {
              BLE_MANAGER_PRINTF("Problem Changing the connection interval\r\n");
            }
          }
        }
        
        /* Signal that we are ready sending back the CRV value*/
        BufferToWrite[0] = PointerByte[0];
        BufferToWrite[1] = PointerByte[1];
        BufferToWrite[2] = PointerByte[2];
        BufferToWrite[3] = PointerByte[3];
        BytesToWrite = 4;
        Term_Update(BufferToWrite,BytesToWrite);
      }
      
      SendBackData=0;      
    } else if(!strncmp("versionBle",(char *)(att_data),10)) {
      uint8_t  hwVersion;
      uint16_t fwVersion;
      /* get the BlueNRG HW and FW versions */
      getBlueNRGVersion(&hwVersion, &fwVersion);
      BytesToWrite =sprintf((char *)BufferToWrite,"%s_%d.%d.%c\r\n",
                            "BlueNRG-MS",
                            fwVersion>>8, 
                            (fwVersion>>4)&0xF,
                            ('a'+(fwVersion&0xF)));
      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0; 
    } else if((att_data[0]=='u') & (att_data[1]=='i') & (att_data[2]=='d')) {
      /* Write back the STM32 UID */
      uint8_t *uid = (uint8_t *)STM32_UUID;
      uint32_t MCU_ID = STM32_MCU_ID[0]&0xFFF;
      BytesToWrite =sprintf((char *)BufferToWrite,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                            uid[ 3],uid[ 2],uid[ 1],uid[ 0],
                            uid[ 7],uid[ 6],uid[ 5],uid[ 4],
                            uid[11],uid[ 10],uid[9],uid[8],
                            MCU_ID);
      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0;
    } else if(!strncmp("setName ",(char *)(att_data),8)) {
      
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
        {
          NodeName[i]= att_data[i+7];
          BLE_StackValue.BoardName[i-1]= att_data[i+7];
        }
        
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

///**
// * @brief  This function is called when there is a Bluetooth Read request.
// * @param  None 
// * @retval None
// */
//static void ReadRequestEnvFunction(void)
//{
//  /* Read Request for Pressure,Humidity, and Temperatures*/
//  int32_t PressToSend;
//  uint16_t HumToSend;
//  int16_t TempToSend;
//
//  /* Read all the Environmental Sensors */
//  ReadEnvironmentalData(&PressToSend,&HumToSend, &TempToSend);
//  
//  /* Send the Data with BLE */
//  BLE_EnvironmentalUpdate(PressToSend,HumToSend,TempToSend, 0);
//  
//  BLE_MANAGER_PRINTF("Read for Env\r\n");
//}

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None 
 * @retval None
 */
static void DisconnectionCompletedFunction(void)
{
  connected = FALSE;
  
  LedEnabled= 0;
  
  if(ObjectsDetectionTimerEnabled)
  {
    if(HAL_TIM_Base_Stop_IT(&TimDistanceHandle) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }

    StopMeasurement();
      
    ObjectsDetectionTimerEnabled= 0;
  }
  
  /* Reset for any problem during FOTA update */
  SizeOfUpdateBlueFW = 0;
    
  BLE_MANAGER_PRINTF("Call to DisconnectionCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);
}

#ifdef FLIGHT1_SECURE_CONNECTION
/**
 * @brief  This function is called when the Set Connectable is doing.
 * @param  uint8_t *ManufData:  
 * @retval None
 */
static void SetConnectableFunction(uint8_t *ManufData)
{   
  if (TargetBoardFeatures.NFC_TAG_support)
  {
    sURI_Info URI;
    /* Prepare URI NDEF message content */
    strcpy( URI.protocol,"\0" );
    sprintf(URI.URI_Message,"blesensor://connect?Pin=%ld&Add=%02x:%02x:%02x:%02x:%02x:%02x",
                         BLE_StackValue.SecurePIN,
                         BLE_StackValue.BleMacAddress[5],
                         BLE_StackValue.BleMacAddress[4],
                         BLE_StackValue.BleMacAddress[3],
                         BLE_StackValue.BleMacAddress[2],
                         BLE_StackValue.BleMacAddress[1],
                         BLE_StackValue.BleMacAddress[0]);
    strcpy( URI.Information,"\0" );
    /* Write NDEF to EEPROM */
    while( NDEF_WriteURI( &URI ) != NDEF_OK );
    FLIGHT1_PRINTF("\r\nST25DV Bluetooth NDEF Table written\r\n\r\n");
  }
}
#endif /* FLIGHT1_SECURE_CONNECTION */

/**
 * @brief  This function is called when there is a Pairing Completed event.
 * @param  None 
 * @retval None
 */
static void PairingCompletedFunction(uint8_t PairingStatus)
{
  if(PairingStatus==SM_PAIRING_SUCCESS) {
    MasterTrusted=TRUE;
#ifdef FLIGHT1_SECURE_CONNECTION
    int i;
    for(i=0;i<6;i++) {
      TrustedAddr[i]= CurrentAddr[i];
    }
#endif /*FLIGHT1_SECURE_CONNECTION */
  }    
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  None 
 * @retval None
 */
static void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t addr[6])
{
  BLE_ConnectionHandle = ConnectionHandle;
  
  connected = TRUE;
  MasterTrusted = FALSE;
  
#ifdef FLIGHT1_SECURE_CONNECTION
  if (!TargetBoardFeatures.NFC_TAG_support)
      MasterTrusted = TRUE;
  else 
  {
    int i;
    int AlreadyTrusted=1;
      
    for(i=0;i<6;i++) {
      CurrentAddr[i]= addr[i];
      if(addr[i]!=TrustedAddr[i]){
        AlreadyTrusted=0;
      }
    }
    
    if(!AlreadyTrusted){
      /* Send a slave security request to the master */
      int ret = aci_gap_slave_security_request(ConnectionHandle,
                                               BLE_StackValue.AuthenticationRequirements,
                                               BLE_StackValue.MITM_ProtectionRequirements);
      if (ret != BLE_STATUS_SUCCESS) {
       FLIGHT1_PRINTF("\r\nGAP Slave secury request failed\r\n");
      }
    } else {
      FLIGHT1_PRINTF("Device already trusted\r\n");
      MasterTrusted = TRUE;
    }
  }
#else /* FLIGHT1_SECURE_CONNECTION */
  MasterTrusted = TRUE;
#endif /* FLIGHT1_SECURE_CONNECTION */
  
  BLE_MANAGER_PRINTF("Call to ConnectionCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);
}


/**
 * @brief  This function is called when there is a notify event for Objects Detection feature.
 * @param  BLE_NotifyEvent_t Event 
 * @retval None
 */
static void NotifyEventObjectsDetection(BLE_NotifyEvent_t Event)
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

/**
 * @brief  This function is called when there is a notify event for Led feature.
 * @param  BLE_NotifyEvent_t Event 
 * @retval None
 */
static void NotifyEventLed(BLE_NotifyEvent_t Event)
{
  if(Event == BLE_NOTIFY_SUB)
  {
    LedEnabled= 1;
    BLE_LedStatusUpdate(TargetBoardFeatures.LedStatus);
  }
    
  if(Event == BLE_NOTIFY_UNSUB)
    LedEnabled= 0;
}

/***********************************************************************************
 * Callback functions to manage the extended configuration characteristic commands *
 ***********************************************************************************/

/**
 * @brief  Callback Function for answering to the UID command
 * @param  uint8_t **UID STM32 UID Return value
 * @retval None
 */
static void ExtExtConfigUidCommandCallback(uint8_t **UID)
{
  *UID = (uint8_t *)STM32_UUID;
}


/**
 * @brief  Callback Function for answering to Info command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigInfoCommandCallback(uint8_t *Answer)
{
  uint8_t  hwVersion;
  uint16_t fwVersion;
  
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  sprintf((char *)Answer,"STMicroelectronics %s:\n"
    "Version %c.%c.%c\n"
    "%s board\n"
    "BlueNRG2 HW ver%d.%d\n"
    "BlueNRG2 FW ver%d.%d.%c\n"
    "(HAL %ld.%ld.%ld_%ld)\n"
    "Compiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
    " (IAR)",
#elif defined (__CC_ARM)
    " (KEIL)",
#elif defined (__GNUC__)
    " (STM32CubeIDE)",
#endif
    BLE_FW_PACKAGENAME,
    BLE_VERSION_FW_MAJOR,
    BLE_VERSION_FW_MINOR,
    BLE_VERSION_FW_PATCH,
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
static void ExtConfigHelpCommandCallback(uint8_t *Answer)
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
 * @brief  Callback Function for answering to VersionFw command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigVersionFwCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"%s_%s_%c.%c.%c",
      BLE_STM32_MICRO,
      BLE_FW_PACKAGENAME,
      BLE_VERSION_FW_MAJOR,
      BLE_VERSION_FW_MINOR,
      BLE_VERSION_FW_PATCH);
}

/**
 * @brief  Callback Function for managing the SetName command
 * @param  uint8_t *NewName
 * @retval None
 */
static void ExtConfigSetNameCommandCallback(uint8_t *NewName)
{ 
  BLE_MANAGER_PRINTF("New Board Name = <%s>\r\n", NewName);
  /* Change the Board Name */
  sprintf(BLE_StackValue.BoardName,"%s",NewName);
  
  for(int i=0; i<7; i++)
    NodeName[i+1]= BLE_StackValue.BoardName[i];
  
  MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
  NecessityToSaveMetaDataManager=1;
  
  BLE_MANAGER_PRINTF("\nThe node nome has been updated\r\n");
  BLE_MANAGER_PRINTF("Disconnecting and riconnecting to see the new node name\r\n");
}

