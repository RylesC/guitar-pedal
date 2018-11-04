/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "codec.h"
#include "audio_effects.h"
#include "stm32f4xx_it.h"


/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId codecTaskHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void CodecTask(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(codecTask, CodecTask, osPriorityNormal, 0, 128);
  codecTaskHandle = osThreadCreate(osThread(codecTask), NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN StartDefaultTask */

  /* Infinite loop */

  for(;;)
  {
	  taskYIELD();
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
void CodecTask(void const * argument)
{
	//uint32_t formattedData[66] = {0};
	memset(codecTxBuffer, 0x00, BUFFER_SIZE * sizeof(uint16_t));
	memset(codecRxBuffer, 0x00, BUFFER_SIZE * sizeof(uint16_t));
	volatile uint16_t k,i = 0;
	float inputGain = 5;


	CODEC_Init();

	// Start codec transfer
	CODEC_sendReceive((uint32_t *)codecTxBuffer, (uint32_t *)codecRxBuffer);
	//codec_EnableBypass(true);

	for(;;)
	{
		// Wait to receive data
		if( I2SDMARxCompleted )
		{
			// Update flags
			I2SDMARxCompleted = 0;
			I2SDMATxCompleted = 0;

			// Re-package audio data to 32-bit packets
			//AUDIO_receive32BitRightCH(codecTxBuffer, formattedData, 66);

			// Apply audio effect

			for(i = 0; i < BUFFER_SIZE; i++){
			    const float in = codecRxBuffer[i] * inputGain;
			    float out;

			    float uthreshold = 8388608 + 1500000;
			    float lthreshold = 8388608 - 1500000;

			    if(in > uthreshold)
			        out = uthreshold;
			    else if(in < lthreshold)
			          out = lthreshold;
			        else
			          out = in;
		      codecRxBuffer[i] = out;
			  }



			// Re-package audio data to 24-bit packets
			//memcpy(codecTxBuffer, codecRxBuffer, sizeof(codecRxBuffer));

			// Transfer to codec
			CODEC_sendReceive((uint32_t *)codecRxBuffer, (uint32_t *)codecRxBuffer);
		}
	}
}
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
