/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ota.h
  * @author  System Research & Applications Team - Catania Lab.
  * @version 5.1.0
  * @date    24-January-2025
  * @brief   Over-the-Air Update API
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _OTA_H_
#define _OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/
/* Max Program Size (we keep the last sector for FW Meta Data) */
#define OTA_MAX_PROG_SIZE (FLASH_BANK_SIZE - FLASH_PAGE_SIZE)

/* Exported functions ---------------------------------------------------------*/

/* API for preparing the Flash for receiving the Update.
   It defines also the Size of the Update and the CRC value expected */
extern void StartUpdateFW(uint32_t SizeOfUpdate, uint32_t uwCRCValue);

/* API for storing chuck of data to Flash.
 * When it has received the total number of byte defined by StartUpdateFW,
 * it computes the CRC value and if it matches the expected CRC value,
 * it writes the Magic Number in Flash for BootLoader */
extern int8_t UpdateFW(uint32_t *SizeOfUpdate, uint8_t *att_data, int32_t data_length, uint8_t WriteMagicNum);

#ifdef __cplusplus
}
#endif

#endif /* _OTA_H_ */

