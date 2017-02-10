/*************************************************************************
 *  File Name:      codec.c
 *  Author:         LoudNoises
 *  Date Created:   2016-10-20
 *
 *  Brief:
 *      Contains configuration functions for the codec
 *
*************************************************************************/

#include "codec.h"

void CODEC_GPIOInit(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	__GPIOB_CLK_ENABLE();

	// Configure GPIO pins for I2C2
	GPIO_InitStruct.Pin 		= CODEC_I2C_SCL_PIN | CODEC_I2C_SDA_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull		= GPIO_NOPULL; 		// External 4k7 pullups used
	GPIO_InitStruct.Speed		= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate	= GPIO_AF4_I2C2;
	HAL_GPIO_Init(CODEC_I2C_GPIO, &GPIO_InitStruct);

	// Configure GPIO pins for I2S
	GPIO_InitStruct.Pin 		= CODEC_I2S_WS_PIN | CODEC_I2S_extSD_PIN |
								  CODEC_I2S_CK_PIN | CODEC_I2S_SD_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate	= GPIO_AF5_SPI2;
	HAL_GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStruct);

	// Configure I2S MCK
	GPIO_InitStruct.Pin			= CODEC_I2S_MCK_PIN;
	HAL_GPIO_Init(CODEC_I2S_MCK_GPIO, &GPIO_InitStruct);
}

HAL_StatusTypeDef CODEC_i2c2Init(void)
{
	HAL_StatusTypeDef	initSuccess;

	// Enable I2C peripheral clocks
	__HAL_RCC_I2C2_CLK_ENABLE();

	// Configure I2C settings
	I2cHandle.Instance = I2C2;
	I2cHandle.Init.AddressingMode 	= I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.ClockSpeed 		= 400000;
	I2cHandle.Init.DualAddressMode 	= I2C_DUALADDRESS_DISABLED;
	I2cHandle.Init.DutyCycle		= I2C_DUTYCYCLE_2;
	I2cHandle.Init.GeneralCallMode	= I2C_GENERALCALL_DISABLED;
	I2cHandle.Init.NoStretchMode	= I2C_NOSTRETCH_DISABLED;
	I2cHandle.Init.OwnAddress1		= 0x11;

	initSuccess = HAL_I2C_Init(&I2cHandle);

	return(initSuccess);
}

HAL_StatusTypeDef CODEC_i2s2Init(uint32_t audioFrequency)
{
	HAL_StatusTypeDef	initSuccess;

	// Enable I2S peripheral clocks (same clock as SPI2)
	__HAL_RCC_SPI2_CLK_ENABLE();

	// Configure I2S settings
	I2sHandle.Init.AudioFreq 		= audioFrequency;
	I2sHandle.Init.CPOL 			= I2S_CPOL_LOW;
	I2sHandle.Init.DataFormat 		= I2S_DATAFORMAT_24B;
	I2sHandle.Init.Standard 		= I2S_STANDARD_PHILIPS;
	I2sHandle.Init.FullDuplexMode 	= I2S_FULLDUPLEXMODE_ENABLE;
	I2sHandle.Init.MCLKOutput 		= I2S_MCLKOUTPUT_ENABLE;
	I2sHandle.Init.Mode 			= I2S_MODE_MASTER_TX;

	initSuccess = HAL_I2S_Init(&I2sHandle);

	return(initSuccess);
}

void CODEC_WriteRegister(uint8_t addr, uint16_t value)
{
	uint8_t data[2];

	// 7-bit address & first data bit
	data[0] = (uint8_t) (((addr << 1) & 0xFE) | ((value >> 8) & 0x01));

	// Bottom 8 bits
	data[1] = (uint8_t) (value & 0xFF);

	// Write to register in codec format
	// This command should send the codec's I2C address followed by B15-8 and B7-0 of the register
	while(HAL_I2C_Master_Transmit(&I2cHandle, CODEC_I2C_ADDRESS, data, 2, 1000) != HAL_OK);
}

void CODEC_Reset(void)
{
	CODEC_WriteRegister(CODEC_RESET_REG, 0);
}

void CODEC_Test(void)
{
	CODEC_WriteRegister(CODEC_PDOWN_REG, 		0b0000000000000010);
	CODEC_WriteRegister(CODEC_RLINEIN_REG, 		0b0000000000010111);
	CODEC_WriteRegister(CODEC_APATHCTRL_REG, 	0b0000000000001010);
	CODEC_WriteRegister(CODEC_ACTIVE_REG,		0b0000000000000001);
}

void CODEC_PowerDown(uint8_t linein, uint8_t mic, uint8_t adc, uint8_t dac, uint8_t osc, uint8_t clkout, uint8_t poweroff)
{
	uint8_t pdValue = (poweroff << 8) | (clkout << 8);
}
