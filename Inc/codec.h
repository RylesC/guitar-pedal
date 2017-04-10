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
#include <stdbool.h>
#include "stm32f4xx.h"
#include "i2c.h"
#include "i2s.h"

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

#define CODEC_IN_DEFAULT_VOL 0b10111
#define CODEC_HP_DEFAULT_VOL 0b1111001

/*======================================================================*/
/*                      GLOBAL VARIABLE DECLARATIONS                    */
/*======================================================================*/
union LLineIn
{
	uint8_t data[2];

	struct {
		uint8_t LINMUTE		: 1;
		uint8_t ADDR 		: 7;
		uint8_t LINVOL		: 5;
		uint8_t RESERVED	: 2;
		uint8_t LRINBOTH 	: 1;
	} reg;
} LLineIn;

union RLineIn
{
	uint8_t data[2];

	struct {
		uint8_t RINMUTE		: 1;
		uint8_t ADDR 		: 7;
		uint8_t RINVOL		: 5;
		uint8_t RESERVED	: 2;
		uint8_t RLINBOTH 	: 1;
	} reg;
} RLineIn;

union LHPOut
{
	uint8_t data[2];

	struct {
		uint8_t LRHPBOTH	: 1;
		uint8_t ADDR 		: 7;
		uint8_t LHPVOL		: 7;
		uint8_t LZCEN 		: 1;
	} reg;
} LHPOut;

union RHPOut
{
	uint8_t data[2];

	struct {
		uint8_t RLHPBOTH	: 1;
		uint8_t ADDR 		: 7;
		uint8_t RHPVOL		: 7;
		uint8_t RZCEN 		: 1;
	} reg;
} RHPOut;

union APathControl
{
	uint8_t data[2];

	struct {
		uint8_t RESERVED	: 1;
		uint8_t ADDR 		: 7;
		uint8_t MICBOOST	: 1;
		uint8_t MUTEMIC		: 1;
		uint8_t INSEL		: 1;
		uint8_t BYPASS		: 1;
		uint8_t DACSEL 		: 1;
		uint8_t SIDETONE	: 1;
		uint8_t SIDEATT 	: 2;
	} reg;
} APathControl;

union DPathControl
{
	uint8_t data[2];

	struct {
		uint8_t RESERVED1 	: 1;
		uint8_t ADDR 		: 7;
		uint8_t ADCHPD		: 1;
		uint8_t DEEMP		: 2;
		uint8_t DACMU 		: 1;
		uint8_t HPOR		: 1;
		uint8_t RESERVED2	: 3;
	} reg;
} DPathControl;

union PDownControl
{
	uint8_t data[2];

	struct {
		uint8_t RESERVED	: 1;
		uint8_t ADDR		: 7;
		uint8_t LINEINPD	: 1;
		uint8_t MICPD		: 1;
		uint8_t ADCPD		: 1;
		uint8_t DACPD		: 1;
		uint8_t OUTPD		: 1;
		uint8_t OSCPD		: 1;
		uint8_t CLKOUTPD	: 1;
		uint8_t POWEROFF	: 1;
	} reg;
} PDownControl;

union DAInterfaceFormat
{
	uint8_t data[2];

	struct {
		uint8_t RESERVED	: 1;
		uint8_t ADDR		: 7;
		uint8_t FORMAT		: 2;
		uint8_t IWL			: 2;
		uint8_t LRP			: 1;
		uint8_t LRSWAP		: 1;
		uint8_t MS			: 1;
		uint8_t BCLKINV		: 1;
	} reg;
} DAInterfaceFormat;

union SamplingControl
{
	uint8_t data[2];

	struct {
		uint8_t RESERVED	: 1;
		uint8_t ADDR		: 7;
		uint8_t USBNORMAL	: 1;
		uint8_t BOSR		: 1;
		uint8_t SR			: 4;
		uint8_t CLKIDIV2	: 1;
		uint8_t CLKODIV2	: 1;
	} reg;
} SamplingControl;

union ActiveControl
{
	uint8_t data[2];

	struct {
		uint8_t RESERVED1	: 1;
		uint8_t ADDR		: 7;
		uint8_t ACTIVE		: 1;
		uint8_t RESERVED2	: 7;

	} reg;
} ActiveControl;

union ResetReg
{
	uint8_t data[2];

	struct {
		uint8_t RESET1		: 1;
		uint8_t ADDR		: 7;
		uint8_t RESET2		: 8;
	} reg;
} ResetReg;

uint16_t codecTxBuffer[8192];
uint16_t codecRxBuffer[8192];

/*======================================================================*/
/*                          FUNCTION PROTOTYPES                         */
/*======================================================================*/
void CODEC_Init				(void);
void CODEC_startReadWrite	(void);
void CODEC_sendReceive(uint16_t *pTx, uint16_t *pRx);


#endif /* CODEC_H_ */
