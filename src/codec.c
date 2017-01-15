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

I2C_HandleTypeDef I2cHandle;

void CODEC_GPIOInit(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	__GPIOB_CLK_ENABLE();

	// Configure GPIO pins for I2C2
	GPIO_InitStruct.Pin 		= CODEC_I2C_SCL_PIN | CODEC_I2C_SDA_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL; 		// External 4k7 pullups used
	GPIO_InitStruct.Speed		= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate	= GPIO_AF4_I2C2;
	HAL_GPIO_Init(CODEC_I2C_GPIO, &GPIO_InitStruct);

	// Configure GPIO pins for I2S
	GPIO_InitStruct.Pin 		= CODEC_I2S_WS_PIN | CODEC_I2S_extSD_PIN |
								  CODEC_I2S_CK_PIN | CODEC_I2S_SD_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL; 		// External 4k7 pullups used
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

void CODEC_WriteRegister(uint8_t addr, uint8_t value)
{
	// 7-bit address & first data bit
	uint8_t initAddr = ((addr << 1) & 0xFE) | ((value >> 8) & 0x01);

	// Ensure device can be communicated with
	while(HAL_I2C_IsDeviceReady(&I2cHandle, CODEC_I2C_ADDRESS, 1, 1000) != HAL_OK);

	// Write to register in codec format
	while(HAL_I2C_Mem_Write(&I2cHandle, CODEC_I2C_ADDRESS, initAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *) value, (uint16_t) 1, 1000) != HAL_OK);
}
