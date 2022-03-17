/**
  ******************************************************************************
  * @file    TargetPlatform.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   Initialization of the Target Platform
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
  
#include "TargetFeatures.h"
#include "main.h"

/* Imported Variables ---------------------------------------------------------*/

/* Exported variables ---------------------------------------------------------*/
TargetFeatures_t TargetBoardFeatures;

/* Local defines -------------------------------------------------------------*/

/* Local function prototypes --------------------------------------------------*/
static uint8_t NFC4_NDEF_URI_Init(void);
static void Init_53L3A2(void);


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
  
  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* Initialize LED */
  BSP_LED_Init(LED2);

  FLIGHT1_PRINTF("\r\nSTMicroelectronics %s:\r\n"
         "\tVersion %c.%c.%c\r\n"
         "\tSTM32F401RE-Nucleo board"
          "\r\n",
          FLIGHT1_PACKAGENAME,
          FLIGHT1_VERSION_MAJOR,FLIGHT1_VERSION_MINOR,FLIGHT1_VERSION_PATCH);

  /* Reset all the Target's Features */
  memset(&TargetBoardFeatures, 0, sizeof(TargetFeatures_t));
  
  /* Initialize X-NUCLEO-53L3A2 */
  Init_53L3A2();
  
  /* Initialize the NFC TAG component and write the URI */
  TargetBoardFeatures.NFC_TAG_support= NFC4_NDEF_URI_Init();
  
  FLIGHT1_PRINTF("\n\r");
}

/** @brief  Initialize X-NUCLEO-53L3A2
 *  @param  None
 *  @retval None
 */
static void Init_53L3A2(void)
{
  int32_t ret;
  uint32_t Id;
  
  RANGING_SENSOR_Capabilities_t Cap;
  RANGING_SENSOR_ProfileConfig_t Profile;
  
  ret = VL53L3A2_RANGING_SENSOR_Init(VL53L3A2_DEV_CENTER);
  
  if (ret != BSP_ERROR_NONE)
  {
    FLIGHT1_PRINTF("X-NUCLEO-53L3A2 init failed\r\n");
  }
  else
  {
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
    
    FLIGHT1_PRINTF("X-NUCLEO-53L3A2 initialized\r\n");
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

/** @brief Initialize all the NFC4 NDEF URI tag
 *  @param None
 *  @retval Returns SUCCESS if NFC TAG is present otherwise ERROR
 */
static uint8_t NFC4_NDEF_URI_Init(void)
{
  sURI_Info URI;
  
  uint8_t NFC_TAG_Present= 0;
  
  /* Init ST25DV driver */
  if( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) == NFCTAG_OK ){
    FLIGHT1_PRINTF("\r\nNFC TAG Initialized\r\n");
    NFC_TAG_Present= 1;
  } else {
    FLIGHT1_PRINTF("\r\nNFC TAG not present\r\n\n");
    return NFC_TAG_Present;
  }

  /* Reset Mailbox enable to allow write to EEPROM */
  NFC04A1_NFCTAG_ResetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);

  NfcTag_SelectProtocol(NFCTAG_TYPE5);

  /* Check if no NDEF detected, init mem in Tag Type 5 */
  if( NfcType5_NDEFDetection( ) != NDEF_OK )
  {
    CCFileStruct.MagicNumber = NFCT5_MAGICNUMBER_E1_CCFILE;
    CCFileStruct.Version = NFCT5_VERSION_V1_0;
    CCFileStruct.MemorySize = ( ST25DV_MAX_SIZE / 8 ) & 0xFF;
    CCFileStruct.TT5Tag = 0x05;
    /* Init of the Type Tag 5 component (M24LR) */
    while( NfcType5_TT5Init( ) != NFCTAG_OK );
  }

  /* Prepare URI NDEF message content */
  strcpy( URI.protocol,URI_ID_0x01_STRING );
  strcpy( URI.URI_Message,"st.com/en/ecosystems/stm32-open-development-environment.html" );
  strcpy( URI.Information,"\0" );
  
  /* Write NDEF to EEPROM */
  HAL_Delay(5);
  if( NDEF_WriteURI( &URI ) == NDEF_OK ) {
    FLIGHT1_PRINTF("NFC TAG URI written =\r\n\"www.st.com/en/ecosystems/stm32-open-development-environment.html\"\r\n\n");
  } else {
    FLIGHT1_PRINTF("Error Writing URI on NFC TAG\r\n\n");
    return ERROR;
  }
  
  return NFC_TAG_Present;
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

/**
 * @brief User function for Erasing the Flash data for MDM
 * @param None
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForErasingFlash(void) {
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t Success=1;

  EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FLASH_SECTOR_7;
  EraseInitStruct.NbSectors = 1;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK){
    /* Error occurred while sector erase. 
      User can add here some code to deal with this error. 
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    Success=0;
    Error_Handler();
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/**
 * @brief User function for Saving the MDM  on the Flash
 * @param void *InitMetaDataVector Pointer to the MDM beginning
 * @param void *EndMetaDataVector Pointer to the MDM end
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForSavingFlash(void *InitMetaDataVector,void *EndMetaDataVector)
{
  uint32_t Success=1;

  /* Store in Flash Memory */
  uint32_t Address = MDM_FLASH_ADD;
  uint32_t *WriteIndex;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  for(WriteIndex =((uint32_t *) InitMetaDataVector); WriteIndex<((uint32_t *) EndMetaDataVector); WriteIndex++) {
    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, Address,*WriteIndex) == HAL_OK){
      Address = Address + 4;
    } else {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      Error_Handler();
      Success=0;
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
 
  return Success;
}

