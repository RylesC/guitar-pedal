/*************************************************************************
 *  File Name:      codec.h
 *  Author:         LoudNoises
 *  Date Created:   2016-10-20
 *
 *  Sub-files:
 *      codec.c
 *
 *  Brief:
 *      Provides configuration settings for display
 *
*************************************************************************/
#ifndef CODEC_H_
#define CODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx.h"

#include "stm32f4-hal/stm32f4xx_hal_i2c.h"

#define CODEC_I2C_ADDRESS 	(0b0011010) << 1

/* I2C */
#define CODEC_I2C_GPIO		GPIOB
#define CODEC_I2C_SCL_PIN	GPIO_PIN_10
#define CODEC_I2C_SDA_PIN	GPIO_PIN_11

/* I2S */
#define CODEC_I2S_GPIO		GPIOB
#define CODEC_I2S_WS_PIN	GPIO_PIN_12
#define CODEC_I2S_extSD_PIN GPIO_PIN_14
#define CODEC_I2S_CK_PIN	GPIO_PIN_13
#define CODEC_I2S_SD_PIN	GPIO_PIN_15

#define CODEC_I2S_MCK_GPIO	GPIOC
#define CODEC_I2S_MCK_PIN	GPIO_PIN_6

/* Registers */
#define CODEC_LLINEIN_REG	0x00
#define CODEC_RLINEIN_REG	0x01
#define CODEC_LHPOUT_REG	0x02
#define CODEC_RHPOUT_REG	0x03
#define CODEC_APATHCTRL_REG	0x04
#define CODEC_DPATHCTRL_REG	0x05
#define CODEC_PDOWN_REG		0x06
#define CODEC_DFMT_REG		0x07
#define CODEC_SAMPCTRL_REG	0x08
#define CODEC_ACTIVE_REG	0x09
#define CODEC_RESET_REG		0x0f

HAL_StatusTypeDef CODEC_i2c2Init(void);
void CODEC_WriteRegister( uint8_t addr, uint8_t value );

#endif /* CODEC_H_ */
