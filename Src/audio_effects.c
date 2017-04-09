#include "audio_effects.h"

uint32_t AUDIO_receive32BitRightCH(uint16_t *pInput, uint32_t *pOutput, uint32_t length)
{
	uint32_t outputLength = 0;
	uint32_t i = 0;

	pOutput[0] = 0;

	for(i = 0; i < length; i++)
	{
		if(i == 0)
		{
			// Do nothing
		}else if(((i - 1) % 3) == 0){
			pOutput[outputLength] |= ((uint32_t)pInput[i] & 0xFF) << 16; // Take the left 8 bits
		}else if (((i - 2) % 3) == 0){
			pOutput[outputLength] |= pInput[i]; // Take the right 16 bits
			outputLength++;
			pOutput[outputLength] = 0;
		}else{
			// Do nothing
		}
	}

	return outputLength;
}
