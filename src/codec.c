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
HAL_LockTypeDef a;

void CODEC_i2c2Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	__GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();

	GPIO_InitStruct.Pin 	= GPIO_PIN_10 | GPIO_PIN_11;
	GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull	= GPIO_PULLUP;
	GPIO_InitStruct.Speed	= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate	= GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	I2cHandle.Instance = I2C2;
	I2cHandle.Init.AddressingMode 	= I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.ClockSpeed 		= 400000;
	I2cHandle.Init.DualAddressMode 	= I2C_DUALADDRESS_DISABLED;
	I2cHandle.Init.DutyCycle		= I2C_DUTYCYCLE_16_9;
	I2cHandle.Init.GeneralCallMode	= I2C_GENERALCALL_DISABLED;
	I2cHandle.Init.NoStretchMode	= I2C_NOSTRETCH_DISABLED;
	I2cHandle.Init.OwnAddress1		= CODEC_ADDRESS;
	I2cHandle.Init.OwnAddress2		= 0x00;

	HAL_I2C_Init(&I2cHandle);
}

void CODEC_i2cDataTransfer(uint8_t *buffer, uint8_t size)
{
	while(HAL_I2C_Master_Transmit(&I2cHandle, CODEC_ADDRESS << 1, (uint8_t *)buffer, size, 5000) != HAL_OK);
}
