//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "stm32f4xx.h"
#include "stm32f4-hal/stm32f4xx_hal.h"

#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/StackMacros.h"
#include "FreeRTOS/croutine.h"
#include "FreeRTOS/event_groups.h"
#include "FreeRTOS/list.h"
#include "FreeRTOS/mpu_wrappers.h"
#include "FreeRTOS/portable.h"
#include "FreeRTOS/projdefs.h"
#include "FreeRTOS/queue.h"
#include "FreeRTOS/semphr.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/timers.h"

#include "codec.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void vApplicationStackOverflowHook( TaskHandle_t *pxTask, char *pcTaskName );
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

/* Called if stack overflow during execution */
extern void vApplicationStackOverflowHook(TaskHandle_t *pxTask, char *pcTaskName)
{
    /* If the parameters have been corrupted then inspect pxCurrentTCB to
     * identify which task has overflowed its stack.
     */
    for (;;)
    {
        /* Loop forever */
    }
}

/* Called if memory allocation fails */
extern void vApplicationMallocFailedHook(void)
{
    for(;;)
    {
        /* Loop forever */
    }
}

/* This function is called by FreeRTOS idle task */
extern void vApplicationIdleHook(void)
{
    /* Do nothing */
}

/* This function is called by FreeRTOS each tick */
extern void vApplicationTickHook(void)
{
    /* Do nothing */
}

int main(int argc, char* argv[])
{
	__initialize_hardware();
	CODEC_GPIOInit();
	CODEC_i2c2Init();
	CODEC_Init();

	while(1)
	{

	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
