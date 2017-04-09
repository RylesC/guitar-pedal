/*************************************************************************
 *  File Name:      audio_effects.h
 *  Author:         LoudNoises
 *  Date Created:   2017-04-09
 *
 *  Sub-files:
 *      audio_effects.c
 *
 *  Brief:
 *      Audio effects
 *
*************************************************************************/

#ifndef AUDIO_EFFECTS_H_
#define AUDIO_EFFECTS_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"

uint32_t AUDIO_receive32BitRightCH(uint16_t *pInput, uint32_t *pOutput, uint32_t length);


#endif /* AUDIO_EFFECTS_H_ */
