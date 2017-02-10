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

/*======================================================================*/
/*                          GLOBAL DEPENDENCIES                         */
/*======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx.h"

#include "stm32f4-hal/stm32f4xx_hal_i2c.h"
#include "stm32f4-hal/stm32f4xx_hal_i2s.h"

/*======================================================================*/
/*                      GLOBAL CONSTANT DEFINITIONS                     */
/*======================================================================*/
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
#define CODEC_RESET_REG		0x0F

/* Left Line In Register */
#define LLINEIN_LINMUTE		0x07
#define LLINEIN_LRINBOTH	0x08

/* Right Line In Register */
#define RLINEIN_RINMUTE		0x07
#define RLINEIN_RLINBOTH	0x08

/* Left Headphone Out Register */
#define LHPOUT_LZCEN		0x07
#define LHPOUT_LRHPBOTH		0x08

/* Right Headphone Out Register */
#define RHPOUT_RZCEN		0x07
#define RHPOUT_RLHPBOTH		0x08

/* Analogue Audio Path Control Register */
#define APATHCTRL_MICBOOST	0x00
#define APATHCTRL_MUTEMIC	0x01
#define APATHCTRL_INSEL		0x02
#define APATHCTRL_BYPASS	0x03
#define APATHCTRL_DACSEL	0x04
#define APATHCTRL_SIDETONE	0x05

/* Digital Audio Path Control Register */
#define DPATHCTRL_ADCHPD	0x00
#define DPATHCTRL_DACMU		0x03
#define DPATHCTRL_HPOR		0x04

/* Power Down Control Register */
#define PDOWN_LINEINPD		0x00
#define PDOWN_MICPD			0x01
#define PDOWN_ADCPD			0x02
#define PDOWN_DACPD			0x03
#define PDOWN_OUTPD			0x04
#define PDOWN_OSCPD			0x05
#define PDOWN_CLKOUTPD		0x06
#define PDOWN_POWEROFF		0x07

/* Digital Audio Interface Format Register */
#define DFMT_LRP			0x04
#define DFMT_LRSWAP			0x05
#define DFMT_MS				0x06
#define DFMT_BCLKINV		0x07

/* Sampling Control Register */
#define SAMPCTRL_USB_NORMAL	0x00
#define SAMPCTRL_BOSR		0x01
#define SAMPCTRL_CLKIDIV2	0x06
#define SAMPCTRL_CLKODIV2	0x07

/* Active Control Register */
#define ACTIVE_ACTIVEBIT	0x00

/*======================================================================*/
/*                      GLOBAL VARIABLE DECLARATIONS                    */
/*======================================================================*/
I2C_HandleTypeDef I2cHandle;
I2S_HandleTypeDef I2sHandle;

/*======================================================================*/
/*                          FUNCTION PROTOTYPES                         */
/*======================================================================*/
void 				CODEC_GPIOInit(void);
HAL_StatusTypeDef 	CODEC_i2c2Init(void);
HAL_StatusTypeDef 	CODEC_i2s2Init(uint32_t audioFrequency);

void 				CODEC_WriteRegister(uint8_t addr, uint16_t value);
void 				CODEC_Reset(void);
void 				CODEC_Test(void);

#endif /* CODEC_H_ */
