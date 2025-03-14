/**
  ******************************************************************************
  * @file    ble_tilt_sensing.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.0.0
  * @date    25-September-2024
  * @brief   Add Tilt Sensing service using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "ble_manager.h"
#include "ble_manager_common.h"

/* Private define ------------------------------------------------------------*/
#define COPY_TILT_SENSING_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x0D,0x00,0x02,\
                                                               0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Tilt Sensing service */
static ble_char_object_t ble_char_tilt_sensing;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_tilt_sensing(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
__weak void read_request_tilt_sensing_function(ble_angles_output_t *tilt_sensing_measure);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_tilt_sensing(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                          uint8_t data_length, uint8_t *att_data);
#if (BLUE_CORE != BLUENRG_LP)
static void read_request_tilt_sensing(void *ble_char_pointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void read_request_tilt_sensing(void *ble_char_pointer,
                                      uint16_t handle,
                                      uint16_t connection_handle,
                                      uint8_t operation_type,
                                      uint16_t attr_val_offset,
                                      uint8_t data_length,
                                      uint8_t data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Tilt Sensing service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Tilt Sensing service
  */
ble_char_object_t *ble_init_tilt_sensing_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Tilt Sensing info service */
  ble_char_pointer = &ble_char_tilt_sensing;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_tilt_sensing;
  ble_char_pointer->read_request_cb = read_request_tilt_sensing;
  COPY_TILT_SENSING_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 12; /* 2 byte timestamp, 12 byte tilt sensing measurement */
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE Tilt Sensing features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update Tilt Sensing characteristic
  * @param  ble_angles_output_t tilt_sensing_measure Tilt Sensing Recognized
  * @retval ble_status_t   Status
  */
ble_status_t ble_tilt_sensing_update(ble_angles_output_t tilt_sensing_measure)
{
  ble_status_t ret;

  uint8_t buff[2 + 12];
  uint8_t buff_pos;

  float temp_float;
  uint32_t *temp_buff = (uint32_t *) & temp_float;

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff_pos = 2;

  temp_float = tilt_sensing_measure.angles_array[2];
  STORE_LE_32(&buff[buff_pos], *temp_buff);
  buff_pos += ((uint8_t)4);

  temp_float = tilt_sensing_measure.angles_array[0];
  STORE_LE_32(&buff[buff_pos], *temp_buff);
  buff_pos += ((uint8_t)4);

  temp_float = tilt_sensing_measure.angles_array[1];
  STORE_LE_32(&buff[buff_pos], *temp_buff);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_tilt_sensing, 0, 2 + 12, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Tilt Sensing Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Tilt Sensing Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Tilt Sensing is subscribed or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_tilt_sensing(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                          uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_tilt_sensing(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_tilt_sensing(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->TiltSensing=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->TiltSensing=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if (BLUE_CORE != BLUENRG_LP)
static void read_request_tilt_sensing(void *void_char_pointer, uint16_t handle)
{
  ble_angles_output_t tilt_sensing_measure;
  read_request_tilt_sensing_function(&tilt_sensing_measure);
  ble_tilt_sensing_update(tilt_sensing_measure);
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void read_request_tilt_sensing(void *ble_char_pointer,
                                      uint16_t handle,
                                      uint16_t connection_handle,
                                      uint8_t operation_type,
                                      uint16_t attr_val_offset,
                                      uint8_t data_length,
                                      uint8_t data[])
{
  ble_status_t ret;

  ble_angles_output_t tilt_sensing_measure;
  uint8_t buff[2 + 12];
  uint8_t buff_pos;
  float temp_float;
  uint32_t *temp_buff = (uint32_t *) & temp_float;

  read_request_tilt_sensing_function(&tilt_sensing_measure);

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff_pos = 2;

  temp_float = tilt_sensing_measure.angles_array[2];
  STORE_LE_32(&buff[buff_pos], *temp_buff);
  buff_pos += ((uint8_t)4);

  temp_float = tilt_sensing_measure.angles_array[0];
  STORE_LE_32(&buff[buff_pos], *temp_buff);
  buff_pos += ((uint8_t)4);

  temp_float = tilt_sensing_measure.angles_array[1];
  STORE_LE_32(&buff[buff_pos], *temp_buff);

  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2 + 12, buff);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Tilt Sensing Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Tilt Sensing Char\r\n");
    }
  }

  ret = aci_gatt_srv_authorize_resp_nwk(connection_handle, handle,
                                        operation_type, 0, attr_val_offset,
                                        data_length, data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gatt_srv_authorize_resp_nwk() failed: 0x%02x\r\n", ret);
  }
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_tilt_sensing(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify tilt sensing function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_tilt_sensing could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the read request events *
  *******************************************************/
/**
  * @brief  Callback Function for Tilt Sensing read request.
  * @param  ble_angles_output_t *tilt_sensing_measure Tilt Sensing Angle value
  * @retval None
  */
__weak void read_request_tilt_sensing_function(ble_angles_output_t *tilt_sensing_measure)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(tilt_sensing_measure);

  BLE_MANAGER_PRINTF("\r\nRead request tilt sensing function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the read_request_tilt_sensing_function could be implemented in the user file
   */
}
