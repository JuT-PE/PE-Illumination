/*!
 * @file statuscode.h
 * Statuscode 
 * Copyright (C) 2018 Photon Energy GmbH 
 * Autor: junchao.tang@photon-energy.de
 */

#ifndef STATUSCODE_H
#define STATUSCODE_H

/* ID definition */
//#define LED_ID_VIS    0x01   ///< visible LED 
//#define LED_ID_IR     0x02   ///< infrared LED
#define LED_ID_IR     0x01  ///< infrared LED address for V2.1.10
#define LED_ID_VIS    0x02  ///< visible LED address for V2.1.10 

/* Address of components */
#define LED_AD_EN     0x00  ///< enable address
#define LED_AD_SEG1   0x01  ///< enable segment 1 
#define LED_AD_SEG2   0x02  ///< enable segment 2
#define LED_AD_SEG3   0x03  ///< enable segment 3 
#define LED_AD_SEG4   0x04  ///< enable segment 4

#define LED_AD_PWM    0x10  ///< pwm address

/* Cmd Running result */
#define RUN_SUCCESS    0    ///< running succeed 
#define RUN_FAIL       1    ///< running failed

/* Error Code definition */
#define E_OK           0    ///< succeed
#define E_FAIL        -1    ///< common failure 
#define E_BREAK       -2    ///< processing break
#define E_VALID       -3    ///< validation failed    
#define E_RANGE       -4    ///< out of range
#define E_UNVALID     -5    ///< unvalid 
#define E_SAFETY      -6    ///< error because of safety
#define E_NOEXIST     -7    ///< not existed

#endif /* STATUSCODE_H */
