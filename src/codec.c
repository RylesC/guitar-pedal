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
	__GPIOC_CLK_ENABLE();

	// Configure GPIO pins for I2C2
	GPIO_InitStruct.Pin 		= CODEC_I2C_SCL_PIN | CODEC_I2C_SDA_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull		= GPIO_NOPULL; 		// External 4k7 pullups used
	GPIO_InitStruct.Speed		= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate	= GPIO_AF4_I2C2;
	HAL_GPIO_Init(CODEC_I2C_GPIO, &GPIO_InitStruct);

	// Configure GPIO pins for I2S
	GPIO_InitStruct.Pin 		= CODEC_I2S_WS_PIN | CODEC_I2S_CK_PIN | CODEC_I2S_SD_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate	= GPIO_AF5_SPI2;
	HAL_GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin			= CODEC_I2S_extSD_PIN;
	GPIO_InitStruct.Alternate	= GPIO_AF6_I2S2ext;
	HAL_GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStruct);

	// Configure I2S MCK
	GPIO_InitStruct.Pin			= CODEC_I2S_MCK_PIN;
	HAL_GPIO_Init(CODEC_I2S_MCK_GPIO, &GPIO_InitStruct);
}

HAL_StatusTypeDef CODEC_i2c2Init(void)
{
	HAL_StatusTypeDef	initSuccess;
	RCC_PeriphCLKInitTypeDef i2sClkInit;

	i2sClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2S;

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

	// Enable I2S PLL
	__HAL_RCC_PLLI2S_ENABLE();

	// Enable DMA Controller Clock
	//__HAL_RCC_DMA1_CLK_ENABLE();

	// Configure DMA stream
	//HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

	//HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

	// Enable I2S peripheral clocks (same clock as SPI2)
	__HAL_RCC_SPI2_CLK_ENABLE();

	// Configure I2S settings
	I2sHandle.Instance				= SPI2;
	I2sHandle.Init.AudioFreq 		= I2S_AUDIOFREQ_48K;
	I2sHandle.Init.CPOL 			= I2S_CPOL_LOW;
	I2sHandle.Init.DataFormat 		= I2S_DATAFORMAT_24B;
	I2sHandle.Init.Standard 		= I2S_STANDARD_PHILIPS;
	I2sHandle.Init.FullDuplexMode 	= I2S_FULLDUPLEXMODE_ENABLE;
	I2sHandle.Init.MCLKOutput 		= I2S_MCLKOUTPUT_ENABLE;
	I2sHandle.Init.Mode 			= I2S_MODE_MASTER_TX;
	I2sHandle.Init.ClockSource		= I2S_CLOCK_PLL;

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

void CODEC_Init(void)
{
	// Initialize all registers to defaults
	codec_RegisterInit();

	// Reset codec to default state
	codec_ResetCodec();

	// Power on codec peripherals
	codec_PowerOn();

	// Enable bypass
	codec_EnableBypass(true);

	// Activate codec
	codec_ActivateCodec();
}

void codec_RegisterInit(void)
{
	// Setup register defaults

	// Left Line In Register
	LLineIn.reg.ADDR 			= CODEC_LLINEIN_REG;
	LLineIn.reg.LINMUTE 		= 1;
	LLineIn.reg.LRINBOTH 		= 0;
	LLineIn.reg.LINVOL 			= CODEC_IN_DEFAULT_VOL;

	// Right Line In Register
	RLineIn.reg.ADDR 			= CODEC_RLINEIN_REG;
	RLineIn.reg.RINMUTE 		= 1;
	RLineIn.reg.RLINBOTH 		= 0;
	RLineIn.reg.RINVOL 			= CODEC_IN_DEFAULT_VOL;

	// Left Headphone Out Register
	LHPOut.reg.ADDR 			= CODEC_LHPOUT_REG;
	LHPOut.reg.LRHPBOTH			= 0;
	LHPOut.reg.LZCEN			= 0;
	LHPOut.reg.LHPVOL			= CODEC_HP_DEFAULT_VOL;

	// Right Headphone Out Register
	RHPOut.reg.ADDR 			= CODEC_RHPOUT_REG;
	RHPOut.reg.RLHPBOTH			= 0;
	RHPOut.reg.RZCEN			= 0;
	RHPOut.reg.RHPVOL			= CODEC_HP_DEFAULT_VOL;

	// Analogue Audio Path Control Register
	APathControl.reg.ADDR 		= CODEC_APATHCTRL_REG;
	APathControl.reg.SIDEATT	= 0b00;
	APathControl.reg.SIDETONE	= 0;
	APathControl.reg.DACSEL 	= 0;
	APathControl.reg.BYPASS 	= 1;
	APathControl.reg.INSEL		= 0;
	APathControl.reg.MUTEMIC	= 1;
	APathControl.reg.MICBOOST	= 0;

	// Digital Audio Path Control Register
	DPathControl.reg.ADDR 		= CODEC_DPATHCTRL_REG;
	DPathControl.reg.HPOR		= 0;
	DPathControl.reg.DACMU		= 1;
	DPathControl.reg.DEEMP		= 0b00;
	DPathControl.reg.ADCHPD		= 0;

	// Power Down Control Register
	PDownControl.reg.ADDR 		= CODEC_PDOWN_REG;
	PDownControl.reg.POWEROFF	= 1;
	PDownControl.reg.CLKOUTPD	= 0;
	PDownControl.reg.OSCPD		= 0;
	PDownControl.reg.OUTPD		= 1;
	PDownControl.reg.DACPD		= 1;
	PDownControl.reg.ADCPD		= 1;
	PDownControl.reg.MICPD		= 1;
	PDownControl.reg.LINEINPD	= 1;

	// Digital Audio Interface Format
	DAInterfaceFormat.reg.ADDR 		= CODEC_DFMT_REG;
	DAInterfaceFormat.reg.BCLKINV 	= 0;
	DAInterfaceFormat.reg.MS		= 0;
	DAInterfaceFormat.reg.LRSWAP 	= 0;
	DAInterfaceFormat.reg.LRP 		= 0;
	DAInterfaceFormat.reg.IWL		= 0b10;	// 24-bit Input Audio Data
	DAInterfaceFormat.reg.FORMAT 	= 0b10; // I2S Format, MSB First, left justified

	// Sampling Control Register
	SamplingControl.reg.ADDR 		= CODEC_SAMPCTRL_REG;
	SamplingControl.reg.CLKODIV2	= 0;
	SamplingControl.reg.CLKIDIV2	= 0;
	SamplingControl.reg.SR			= 0b0000;
	SamplingControl.reg.BOSR		= 0;	// Normal Mode: 256fs
	SamplingControl.reg.USBNORMAL 	= 0;	// 256/384fs

	// Active Control Register
	ActiveControl.reg.ADDR 		= CODEC_ACTIVE_REG;
	ActiveControl.reg.ACTIVE	= 0;

	// Reset Register
	ResetReg.reg.ADDR 			= CODEC_RESET_REG;
}

void codec_UpdateRegister(uint16_t codecReg)
{
	uint8_t *data = NULL;

	switch(codecReg)
	{
		case CODEC_LLINEIN_REG:
			data = LLineIn.data;
		break;

		case CODEC_RLINEIN_REG:
			data = RLineIn.data;
		break;

		case CODEC_LHPOUT_REG:
			data = LHPOut.data;
		break;

		case CODEC_RHPOUT_REG:
			data = RHPOut.data;
		break;

		case CODEC_APATHCTRL_REG:
			data = APathControl.data;
		break;

		case CODEC_DPATHCTRL_REG:
			data = DPathControl.data;
		break;

		case CODEC_PDOWN_REG:
			data = PDownControl.data;
		break;

		case CODEC_DFMT_REG:
			data = DAInterfaceFormat.data;
		break;

		case CODEC_SAMPCTRL_REG:
			data = SamplingControl.data;
		break;

		case CODEC_ACTIVE_REG:
			data = ActiveControl.data;
		break;

		case CODEC_RESET_REG:
			data = ResetReg.data;
		break;

		default:
		break;
	}

	if(data != NULL)
	{
		while(HAL_I2C_Master_Transmit(&I2cHandle, CODEC_I2C_ADDRESS, data, 2, 1000) != HAL_OK);
	}
}

void codec_SetInputVolume(uint8_t volume)
{
	// Range from 0 to 32
	// 0 = Mute
	if(volume == 0)
	{
		RLineIn.reg.RINMUTE = 1;

	} else {
		if( volume > 32 ) volume = 32;
		RLineIn.reg.RINVOL = (volume - 1);
		RLineIn.reg.RINMUTE = 0;
	}

	codec_UpdateRegister(CODEC_RLINEIN_REG);
}

void codec_SetOutputVolume(uint8_t volume)
{
	// Range from 0 to 80
	// 0 = Mute
	if( volume > 80 ) volume = 80;
	RHPOut.reg.RHPVOL = volume + 47;

	codec_UpdateRegister(CODEC_RHPOUT_REG);
}

void codec_ZeroCrossHPOutput(bool enable)
{
	if(enable)
	{
		RHPOut.reg.RZCEN = 1;
	} else {
		RHPOut.reg.RZCEN = 0;
	}

	codec_UpdateRegister(CODEC_RHPOUT_REG);
}

void codec_ActivateCodec(void)
{
	// Activate codec
	ActiveControl.reg.ACTIVE = 1;

	// Update activate register
	codec_UpdateRegister(CODEC_ACTIVE_REG);
}

void codec_PowerOn(void)
{
	// Enable Line Input
	PDownControl.reg.LINEINPD 	= 0;

	// Disable mic input
	PDownControl.reg.MICPD 		= 1;

	// Enable ADC
	PDownControl.reg.ADCPD 		= 0;

	// Enable DAC
	PDownControl.reg.DACPD 		= 0;

	// Enable outputs
	PDownControl.reg.OUTPD 		= 0;

	// Enable oscillator
	PDownControl.reg.OSCPD 		= 0;

	// Disable clock output
	PDownControl.reg.CLKOUTPD 	= 1;

	// Disable poweroff
	PDownControl.reg.POWEROFF 	= 0;

	// Update powerdown register
	codec_UpdateRegister(CODEC_PDOWN_REG);
}

void codec_EnableBypass(bool enable)
{
	// Mute microphone and enable bypass
	APathControl.reg.BYPASS = enable;
	codec_UpdateRegister(CODEC_APATHCTRL_REG);
}

void codec_ResetCodec(void)
{
	ResetReg.reg.RESET1 = 0;
	ResetReg.reg.RESET2 = 0;
	codec_UpdateRegister(CODEC_RESET_REG);
}
