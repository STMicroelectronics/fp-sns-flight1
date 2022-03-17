/**
  ******************************************************************************
  * @file    main.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version 4.2.0
  * @date    28-Feb-2022
  * @brief   Main program body
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

/**
 * @mainpage FP-SNS-FLIGHT1 Bluetooth Low Energy and Sensors Software
 *
 * @image html st_logo.png
 *
 * <b>Introduction</b>
 *
 * This firmware package includes Components Device Drivers, Board Support Package
 * and example application for the following STMicroelectronics elements:
 * <ul><li>X-NUCLEO-IDB05A2 Bluetooth Low energy expansion boards</li>
 * <li>X-NUCLEO-NFC04A1 NFC/RFID tag IC expansion board based on ST25DV04K</li>
 * <li>X-NUCLEO-53L3A2  Multi-targets ranging sensor expansion board based on VL53L3CX for STM32 Nucleo.</li>
 * <li>NUCLEO-F401RE Nucleo board</li></ul>
 *
 * @attention
 * <b>Important Hardware Additional Information</b><br>
 * <br>\image html X-NUCLEO-NFC04A1_HW_Info.png "Figure: X-NUCLEO-NFC04A1 expansion board"
 * <ul><li>Before to connect X-NUCLEO-NFC04A1 with X-NUCLEO-IDB05A2 expansion board through the Arduino UNO R3 extension connector,
 * remove the resistors R1 and R11 onto X-NUCLEO-NFC04A1 board.</li></ul>
 *
 * <b>Example Application</b>
 *
 * The Example application initizializes all the Components and Library creating 3 Custom Bluetooth services:
 * <ul><li>The first service exposes all the HW characteristics related to time-of-flight sensors data and Led status.</li>
 * <li>The second service exposes the console services where we have stdin/stdout and stderr capabilities</li>
 * <li>The last Service is used for configuration purpose</li></ul>
 *
 * The example application allows the user to control the initialization phase via UART.
 * Launch a terminal application and set the UART port to 115200 bps, 8 bit, No Parity,
 * 1 stop bit.
 *
 * This example must be used with the related ST BLE Sensor Android/iOS application available on Play/itune store,
 * in order to read the sent information by Bluetooth Low Energy protocol
 *
 *                              -----------------
 *                              | Known Issues: |
 *                              -----------------
 * After pairing with STBLESensor application using NFC reader, after reset for pairing again the board with STBLESensor application
 * without using NFC reader could be necessary restart the the bluetooth of the mobile phone before the connection.
 *
 *                              --------------------
 *                              | Very Important : |
 *                              --------------------
 * The implementation allow the Firmware-Over-The-Air (FOTA).
 * 
 * 1) The Firmware-Over-The-Air (FOTA) is done using the ST BLE Sensor Android/iOS application (Version 3.0.0 and above)
 *
 * 2) This example must run starting at address 0x08004000 in memory and works ONLY if the BootLoader 
 * is saved at the beginning of the FLASH (address 0x08000000)
 *
 * 3) For each IDE (IAR/µVision/STM32CubeIDE) there are some scripts *.bat and *.sh that makes the following operations:
 *    - Full Flash Erase
 *    - Load the BootLoader on the right flash region
 *    - Load the Program (after the compilation) on the right flash region (This could be used for a FOTA)
 *    - Dump back one single binary that contain BootLoader+Program that could be
 *      flashed at the flash beginning (address 0x08000000) (This COULD BE NOT used for FOTA)
 *    - Reset the board
 * .
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <limits.h>
#include "TargetFeatures.h"
#include "main.h"
#include "MetaDataManager.h"
#include "OTA.h"
#include "bluenrg_utils.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Polling period in milliseconds */
#define POLLING_PERIOD (250U)

/* Exported Variables -------------------------------------------------------------*/
uint8_t LedEnabled= 0;
uint8_t ObjectsDetectionTimerEnabled= 0;

volatile uint32_t HCI_ProcessEvent= 0;

TIM_HandleTypeDef TimDistanceHandle;

uint8_t NodeName[8];

/* Table with All the known Meta Data */
MDM_knownGMD_t known_MetaData[]={
  {GMD_NODE_NAME,      (sizeof(NodeName))},
  {GMD_END    ,0}/* THIS MUST BE THE LAST ONE */
};

/* Private variables ---------------------------------------------------------*/
static uint8_t StartSTBLESensor = FALSE;

static volatile uint32_t ToF_MultiObjects_SendDistance   = 0;

/* The VL53L3CX is able to detect up to 4 objects */
static uint16_t ObjectsDistance[4]= {0, 0, 0, 0};
static uint8_t HumanPresence;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

static void InitTimers(void);

static unsigned char ReCallNodeNameFromMemory(void);

static void ButtonCallback(void);
static void RangingLoop(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  HAL_Init();

  /* Configure the System clock */
  SystemClock_Config();

  InitTargetPlatform();

  /* Check the MetaDataManager */
  InitMetaDataManager((void *)&known_MetaData,MDM_DATA_TYPE_GMD,NULL);  

  FLIGHT1_PRINTF("\n\t(HAL %ld.%ld.%ld_%ld)\r\n"
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
  
  /* Set Node Name */
  ReCallNodeNameFromMemory();

  /* Check the BootLoader Compliance */
  FLIGHT1_PRINTF("\r\n");
  if(CheckBootLoaderCompliance()) {
    FLIGHT1_PRINTF("BootLoader Compliant with FOTA procedure\r\n\n");
  } else {
    FLIGHT1_PRINTF("ERROR: BootLoader NOT Compliant with FOTA procedure\r\n\n");
  }

  if(TargetBoardFeatures.NFC_TAG_support)
  {
    FLIGHT1_PRINTF("\r\nPress the User button for starting the STBLESensor mobile application\r\n");
  }
  
  FLIGHT1_PRINTF("\r\n");
   
  if(!TargetBoardFeatures.NFC_TAG_support)
  {
    HAL_Delay(100);
    ButtonCallback();
    HAL_Delay(100);
  }
  
  InitTimers();
  
  /* Infinite loop */
  while (1){
    /* Handle user button */
    if(BSP_PB_GetState(BUTTON_KEY)) {
      ButtonCallback();
      HAL_Delay(200); /* Button debouncing */
    }
    
    if(StartSTBLESensor)
    {
      /* Led Blinking when there is not a client connected */
      if(!connected) {
        if(!TargetBoardFeatures.LedStatus) {
          if(!(HAL_GetTick()&0x3FF)) {
            LedOnTargetPlatform();
          }
        } else {
          if(!(HAL_GetTick()&0x3F)) {
            LedOffTargetPlatform();
          }
        }
      }

      if(set_connectable){
        if(NecessityToSaveMetaDataManager) {
          uint32_t Success = EraseMetaDataManager();
          if(Success) {
            SaveMetaDataManager();
          }
        }

        /* Now update the BLE advertise data and make the Board connectable */
        setConnectable();
        set_connectable = FALSE;
      }
      
      /* handle BLE event */
      if(HCI_ProcessEvent) {
        HCI_ProcessEvent=0;
        hci_user_evt_proc();
      }
      
      if(ToF_MultiObjects_SendDistance)
      {
        ToF_MultiObjects_SendDistance= 0;
        RangingLoop();
      }
    }
    else /* StartSTBLESensor */
    {
      /* Led ON after reset */
      LedOnTargetPlatform();
    } /* StartSTBLESensor */

    /* Wait for Event */
    __WFI();
  }
}

/**
  * @brief  Callback for user button
  * @param  None
  * @retval None
  */
static void ButtonCallback(void)
{
  if(!StartSTBLESensor)
  {
#ifdef FLIGHT1_DEBUG_CONNECTION
    FLIGHT1_PRINTF("Debug Connection         Enabled\r\n");
#endif /* FLIGHT1_DEBUG_CONNECTION */

#ifdef FLIGHT1_DEBUG_NOTIFY_TRAMISSION
    FLIGHT1_PRINTF("Debug Notify Trasmission Enabled\r\n\n");
#endif /* FLIGHT1_DEBUG_NOTIFY_TRAMISSION */
  
    /* Initialize the BlueNRG stack and services */
    BluetoothInit();
    
    StartSTBLESensor= TRUE;
  }
  else
  {
    FLIGHT1_PRINTF("User Button Pressed\r\n");
  }
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
 * @brief  Function for initializing timers:
 * @param  None
 * @retval None
 */
static void InitTimers(void)
{
  uint32_t uwPrescalerValue;

  /* Compute the prescaler value to have TIM3 counter clock equal to TIM_CLOCK_DISTANCE Hz */
  uwPrescalerValue = (uint32_t) ((SystemCoreClock / TIM_CLOCK_DISTANCE) - 1);
  
  /* Set TIM4 instance ( Environmental ) */
  TimDistanceHandle.Instance = TIM3;
  /* Initialize TIM4 peripheral */
  TimDistanceHandle.Init.Period = (TIM_CLOCK_DISTANCE  / ALGO_FREQ_DISTANCE) - 1;
  TimDistanceHandle.Init.Prescaler = uwPrescalerValue;
  TimDistanceHandle.Init.ClockDivision = 0;
  TimDistanceHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&TimDistanceHandle) != HAL_OK) {
    /* Initialization Error */
  }
}

/**
 * @brief  Check if there are a valid Node Name Values in Memory and read them
 * @param  None
 * @retval unsigned char Success/Not Success
 */
static unsigned char ReCallNodeNameFromMemory(void)
{
  /* ReLoad the Node Name Values from RAM */
  unsigned char Success=0;
  
  //Set the BLE Board Name 
  sprintf(BLE_StackValue.BoardName,"%s%c%c%c","FL1V",
          FLIGHT1_VERSION_MAJOR,
          FLIGHT1_VERSION_MINOR,
          FLIGHT1_VERSION_PATCH);

  /* Recall the node name Credential saved */
  MDM_ReCallGMD(GMD_NODE_NAME,(void *)&NodeName);
  
  if(NodeName[0] != 0x12)
  {
    NodeName[0]= 0x12;
    
    for(int i=0; i<7; i++)
      NodeName[i+1]= BLE_StackValue.BoardName[i];
    
    MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
    NecessityToSaveMetaDataManager=1;
  }
  else
  {
    for(int i=0; i<7; i++)
      BLE_StackValue.BoardName[i]= NodeName[i+1];
  }

  return Success;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow:
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 84000000
  *            HCLK(Hz)                       = 84000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 16
  *            PLL_N                          = 336
  *            PLL_P                          = 4
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
    Error_Handler();
  }
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==(&TimDistanceHandle)) {
    /* ToF multi objects distance */
    if(ObjectsDetectionTimerEnabled)
      ToF_MultiObjects_SendDistance=1;   
  } 
}

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

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1){
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: FLIGHT1_PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1){
  }
}
#endif /* USE_FULL_ASSERT */

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
