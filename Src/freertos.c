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
#include "stm32f4xx_hal_i2s_ex.h"
#include "codec.h"


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
  osThreadDef(codecTask, CodecTask, osPriorityRealtime, 0, 128);
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
  memset(codecTxBuffer, 0x00000000, BUFFER_SIZE * sizeof(uint16_t));
  memset(codecRxBuffer, 0x00000000, BUFFER_SIZE * sizeof(uint16_t));
 // memset(codecTxBuffer_Long, 0x00000000, BUFFER_SIZE * sizeof(uint16_t));

 // memset(EMPTY, 0x0000, BUFFER_SIZE * sizeof(int32_t));
  volatile int16_t i,k,j= 0;
  volatile int16_t in,out= 0;

  //volatile uint32_t INPUT[BUFFER_SIZE/2] =  {1638556, 1573021, 1376411, 1507484, 1638556, 1638553, 1966242, 1966242, 1835167, 1310874, 1376410, 1376408, 1441945, 1441945, 1507482, 1441946};
  //volatile int16_t INPUT2[BUFFER_SIZE] =  {0, 65535, 0, 65535, 0, 65535, 0,65535};
  //volatile int32_t INPUT[BUFFER_SIZE/2] =  {0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100};

  CODEC_Init();
  // Start codec transfer
  while(hdma_spi2_tx.State != HAL_DMA_STATE_READY);
  CODEC_sendReceive((uint16_t*)codecTxBuffer,(uint16_t*)codecRxBuffer);
  //codec_EnableBypass(1);
  //I2SDMARxCompleted = 1;

  for(;;)
  {
    if(((j*BUFFER_SIZE)%MAX_BUFFER) ==0){
        j =0;
    }

    k = (j*BUFFER_SIZE)%MAX_BUFFER;
    // Wait to receive data
    if( I2SDMARxCompleted)
    {
      // Update flags
      I2SDMARxCompleted = 0;
      I2SDMATxCompleted = 0;

     // Apply audio effect

      for(i = 0; i<(BUFFER_SIZE/2); i++){
          in = (int16_t)codecRxBuffer[i];
          out = REVERB(DelayBuffer, OutputBuffer, in,k,i);
          //Fill buffers
          DelayBuffer[k+i] = in;
          codecTxBuffer[i] = out;
      }
          //codecTxBuffer_Long = Data_Full;
          CODEC_sendReceive((uint16_t*)codecTxBuffer,(uint16_t*)codecRxBuffer);
          j++;
    }
//      for(i = 0; i<(BUFFER_SIZE/2); i++){
//      out = ((codecRxBuffer[2*i]<<8) & 0xff00) | (codecRxBuffer[2*i+1] & 0x00ff);
//      EMPTY[i*2+1] = out;
//      }

//     while(hdma_spi2_tx.State != HAL_DMA_STATE_READY);
//      codecTxBuffer[0] = codecRxBuffer[0]; //R

//      codecTxBuffer[1] = 0;//1000;
//      codecTxBuffer[2] = 0; //L
//      codecTxBuffer[3] = 0;
//      codecTxBuffer[4] = 0; //R
//      codecTxBuffer[5] = 0;//1000;
//      codecTxBuffer[6] = 0; //L
//      codecTxBuffer[7] = 0;
      //while(hdma_spi2_tx.State != HAL_DMA_STATE_READY);

    }

      // Re-package audio data to 24-bit packets
      //memcpy(codecTxBuffer, codecRxBuffer, sizeof(codecRxBuffer));

     // Transfer to codec


}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
