/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : This file contains all the functions prototypes for the
 *                      USB description firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __USB_DESC_H
#define __USB_DESC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ch32v20x.h"

#define USB_DEVICE_DESCRIPTOR_TYPE 0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE 0x02
#define USB_STRING_DESCRIPTOR_TYPE 0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE 0x05

#define DEF_USBD_UEP0_SIZE 64
#define DEF_USBD_MAX_PACK_SIZE 64

#define USBD_SIZE_DEVICE_DESC 18
#define USBD_SIZE_CONFIG_DESC 67
#define USBD_SIZE_STRING_LANGID 4
#define USBD_SIZE_STRING_VENDOR 16
#define USBD_SIZE_STRING_PRODUCT 22
#define USBD_SIZE_STRING_SERIAL 22

extern const uint8_t USBD_DeviceDescriptor[USBD_SIZE_DEVICE_DESC];
extern const uint8_t USBD_ConfigDescriptor[USBD_SIZE_CONFIG_DESC];
extern const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID];
extern const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR];
extern const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT];
extern uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL];

#ifdef __cplusplus
}
#endif

#endif /* __USB_DESC_H */
