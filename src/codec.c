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

void CODEC_WriteRegister(uint8_t addr, uint16_t value)
{
	uint8_t data[2];

	// 7-bit address & first data bit
	data[0] = (uint8_t) (((addr << 1) & 0xFE) | ((value >> 8) & 0x01));

	// Bottom 8 bits
	data[1] = (uint8_t) (value & 0xFF);

	// Write to register in codec format
	// This command should send the codec's I2C address followed by B15-8 and B7-0 of the register
	while(HAL_I2C_Master_Transmit(&hi2c2, CODEC_I2C_ADDRESS, data, 2, 1000) != HAL_OK);
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
		while(HAL_I2C_Master_Transmit(&hi2c2, CODEC_I2C_ADDRESS, data, 2, 1000) != HAL_OK);
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
