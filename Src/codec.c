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

uint16_t buffer1[66] = {0};
uint16_t buffer2[66] = {0};

 int32_t IRR_Last1 = 0;
int32_t IRR_Last2 = 0;
int32_t IRR_Last3 = 0;

void codec_EnableDACOut			(bool enable);
void codec_EnableADCIn			(bool enable);
void codec_RegisterInit			(void);
void codec_UpdateRegister		(uint16_t codecReg);
void codec_SetInputVolume		(uint8_t volume);
void codec_SetOutputVolume		(uint8_t volume);
void codec_ZeroCrossHPOutput	(bool enable);
void codec_PowerOn				(void);
void codec_ActivateCodec		(void);
void codec_EnableBypass			(bool enable);
void codec_ResetCodec			(void);


void CODEC_Init(void)
{
	// Initialize all registers to defaults
	codec_RegisterInit();

	// Reset codec to default state
	codec_ResetCodec();

	codec_EnableADCIn(true);

	// Power on codec peripherals
	// Update to power on peripherals in correct order
	codec_PowerOn();

	codec_EnableDACOut(true);

	// Activate codec
	codec_ActivateCodec();

	// Just for testing...
	codec_SetOutputVolume(40);
	codec_SetInputVolume(5);
}

/*void CODEC_startReadWrite(void)
{
	static bool bufferSelect = true;

	if(bufferSelect)
	{
		I2S2_TransmitReceive_DMA(buffer1, buffer2, 66);
		bufferSelect = false;
	} else {
		I2S2_TransmitReceive_DMA(buffer2, buffer1, 66);
		bufferSelect = true;
	}
}*/

void CODEC_sendReceive(uint16_t* pTx, uint16_t* pRx)
{
	I2S2_TransmitReceive_DMA(pTx, pRx, BUFFER_SIZE);
}

void codec_EnableDACOut(bool enable)
{
	if(enable)
	{
		APathControl.reg.DACSEL = 1;
		APathControl.reg.BYPASS = 0;
		DPathControl.reg.DACMU	= 0;
		DPathControl.reg.DEEMP  = 0b00;
	} else {
		APathControl.reg.DACSEL = 0;
		DPathControl.reg.DACMU	= 1;
	}

	codec_UpdateRegister(CODEC_APATHCTRL_REG);
	codec_UpdateRegister(CODEC_DPATHCTRL_REG);
}

void codec_EnableADCIn(bool enable)
{

	if(enable)
	{
		// Unmute RLineIn
		RLineIn.reg.RINMUTE = 0;
	  LLineIn.reg.LINMUTE = 1;
	} else {
		// Mute RLineIn
		RLineIn.reg.RINMUTE = 1;
		LLineIn.reg.LINMUTE = 1;
	}

	codec_UpdateRegister(CODEC_RLINEIN_REG);
	codec_UpdateRegister(CODEC_LLINEIN_REG);
}

int REVERB(int16_t *DelayBuffer, int16_t *OutputBuffer, int16_t in, int16_t k, volatile int16_t i){

  volatile int32_t sig_out, sig, sig1, sig2, sig3, sig4, sig5, sig6 = 0;

  int16_t delay = 5000;
  int16_t delay1 = 2200;
  int16_t delay2 = 2360;
  int16_t delay3 = 2430;
  int16_t delay4 = 3580;
  int16_t delay5 = 3778;
  int16_t delay6 = 3000;

  //FIR implementation for early reflections
  sig = sig + FIR(DelayBuffer,k,i,delay1,0.6);
  sig = sig + FIR(DelayBuffer,k,i,delay2,0.7);
  sig = sig + FIR(DelayBuffer,k,i,delay3,0.6);
  sig = sig + FIR(DelayBuffer,k,i,delay4,0.5);
  sig = sig + FIR(DelayBuffer,k,i,delay5,0.4);
  sig = sig + FIR(DelayBuffer,k,i,delay6,0.4);
  sig = sig/16;

  //IRR implementation for late reflections
  sig1 = IIR(DelayBuffer, OutputBuffer, k+i, delay, 0, 1, 0.9, 1);
  sig2 = IIR(DelayBuffer, OutputBuffer, k+i, delay + 500, 0, 1, 0.9, 1);
  sig3 = IIR(DelayBuffer, OutputBuffer, k+i, delay + 900, 0, 1, 0.9, 1);
  sig4 = IIR(DelayBuffer, OutputBuffer, k+i, delay, 1500, 1, 0.9, 1);
  sig5 = IIR(DelayBuffer, OutputBuffer, k+i, delay + 2500, 0, 1, 0.9, 1);
  sig6 = IIR(DelayBuffer, OutputBuffer, k+i, delay + 3000, 0, 1, 0.9, 1);

  int32_t IRR_SIG = (sig1 + sig2 + sig3 +sig4 + sig5 + sig6)/6;

//Low Pass filter output
  int32_t IRR_SIG_LP = (IRR_SIG + IRR_Last1 + IRR_Last2 + IRR_Last3)/4;

  IRR_Last1 = IRR_SIG_LP;
  IRR_Last2 = IRR_Last1;
  IRR_Last3 = IRR_Last2;

  //time delay

  sig_out = (int16_t)(in + IRR_SIG_LP + sig);// + sig/2;//(in/2 + IIRBuffer[(k + i - 3500 + MAX_BUFFER)%MAX_BUFFER]/2)/4; //;
  OutputBuffer[k+i] = (in + IRR_SIG_LP);
  IIRBuffer[k+i] = IRR_SIG_LP;
  return sig_out;
}

int FIR(int16_t *DelayBuffer,int16_t k, int16_t i, int16_t delay, float amp){

  if ((i+k - delay) < 0){
      return (amp*(int16_t)DelayBuffer[k+i - delay + MAX_BUFFER]);
  }
  else{
      return (amp*(int16_t)DelayBuffer[k+i - delay]);
  }
}


int IIR(int16_t *DelayBuffer, int16_t *OutputBuffer, int16_t ki, int16_t delay, int16_t z, float amp, float a, float b){


  if ((ki - (delay) - z) < 0){
       //FIR
      return (amp*(a*(int16_t)OutputBuffer[ki - z - (delay) + MAX_BUFFER]));
  }
  else{
      return (amp*(a*(int16_t)OutputBuffer[ki - z - (delay)]));
  }
}

/*
void codec_ConfigureDFMT(void)
{
	DAInterfaceFormat.reg.IWL = 0b11; // Configure for 32-bit frame
	codec_UpdateRegister(CODEC_DFMT_REG);
}
*/

void codec_RegisterInit(void)
{
	// Setup register defaults

	// Left Line In Register
	LLineIn.reg.ADDR 			  = CODEC_LLINEIN_REG;
	LLineIn.reg.LINMUTE 		= 1;
	LLineIn.reg.LRINBOTH 		= 0;
	LLineIn.reg.LINVOL 			= CODEC_IN_DEFAULT_VOL;

	// Right Line In Register
	RLineIn.reg.ADDR 		  	= CODEC_RLINEIN_REG;
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
	DPathControl.reg.DACMU		= 0;
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
	DAInterfaceFormat.reg.IWL		= 0b00;	// 16-bit Input Audio Data
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
		I2C2_TransmitBlocking(CODEC_I2C_ADDRESS , data, 2, 1000);
	}
}

void codec_SetInputVolume(uint8_t volume)
{
	// Range from 0 to 32
	// 0 = Mute
	if(volume == 0)
	{
		RLineIn.reg.RINMUTE = 1;
		LLineIn.reg.LINMUTE = 1;
	} else {
		if( volume > 32 ) volume = 32;
		RLineIn.reg.RINVOL = (volume - 1);
		RLineIn.reg.RINMUTE = 0;
		LLineIn.reg.LINVOL = (volume - 1);
		LLineIn.reg.LINMUTE = 0;
	}

	codec_UpdateRegister(CODEC_RLINEIN_REG);
	codec_UpdateRegister(CODEC_LLINEIN_REG);
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
