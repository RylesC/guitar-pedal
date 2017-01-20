/*************************************************************************
 *  File Name:      io.c
 *  Author:         LoudNoises
 *  Date Created:   2016-10-20
 *
 *  Brief:
 *      Contains configuration functions for the IO devices
 *
*************************************************************************/
#include "io.h"

/*initializes the encoder*/
  void EncoderInit( void )
  {

    TIM_Encoder_InitTypeDef encoder;
    GPIO_InitTypeDef  GPIO_InitStruct;


    __GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin   = GPIO_PIN_5 | GPIO_PIN_6;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  encoder.EncoderMode = TIM_ENCODERMODE_TI12;
  encoder.IC1Filter = 0x0F;
  encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  encoder.IC1Prescaler = TIM_ICPSC_DIV4;
  encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;

  encoder.IC2Filter = 0x0F;
  encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  encoder.IC2Prescaler = TIM_ICPSC_DIV4;
  encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  }


void TimerInit(){

  __TIM3_CLK_ENABLE();
  TIM_HandleTypeDef timer;

  timer.Instance = TIM3;
  timer.Init.Period = 0xFFFF;
  timer.Init.CounterMode = TIM_COUNTERMODE_UP;
  timer.Init.Prescaler = 0;
  timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }

void ADC_Init(void)
{
  ADC_InitTypeDef  ADC_InitStructure;
  /* PCLK2 is the APB2 clock */
  /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
  ADC_InitStructure.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV6;
  ADC_InitStructure.Resolution = ADC_RESOLUTION_10B;
  ADC_InitStructure.DataAlign = ADC_DATAALIGN_RIGHT;
  ADC_InitStructure.ScanConvMode = DISABLE;
  ADC_InitStructure.EOCSelection = DISABLE;
  ADC_InitStructure.ContinuousConvMode = ENABLE;
  ADC_InitStructure.NbrOfConversion = 1;
  ADC_InitStructure.DiscontinuousConvMode = DISABLE;
  ADC_InitStructure.NbrOfDiscConversion = 0;
  ADC_InitStructure.ExternalTrigConv = ADC_EXTERNALTRIGCONVEDGE_NONE;
  ADC_InitStructure.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  ADC_InitStructure.DMAContinuousRequests = DISABLE;

  HAL_ADC_Init(&ADC_InitStructure);
}

//set flag with interrupt

void GetInputValues();
//pot1
//pot2
//encoder3
