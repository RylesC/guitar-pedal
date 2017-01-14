/*************************************************************************
 *  File Name:      codec.h
 *  Author:         LoudNoises
 *  Date Created:   2016-10-20
 *
 *  Sub-files:
 *      codec.c
 *
 *  Brief:
 *      Provides configuration settings for display
 *
*************************************************************************/
#ifndef CODEC_H_
#define CODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx.h"

#include "stm32f4-hal/stm32f4xx_hal_i2c.h"

#define CODEC_ADDRESS 	0b0011010

void CODEC_i2c2Init( void );
void CODEC_i2cDataTransfer(uint8_t *buffer, uint8_t size);

#endif /* CODEC_H_ */
