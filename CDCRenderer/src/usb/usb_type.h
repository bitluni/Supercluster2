/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_type.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : This file contains all the functions prototypes for the
 *                      USB types firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __USB_TYPE_H
#define __USB_TYPE_H
//#include "debug.h"
#include "usb_conf.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef enum
{
  FALSE = 0,
  TRUE = !FALSE
} BOOL;

#endif /* __USB_TYPE_H */
