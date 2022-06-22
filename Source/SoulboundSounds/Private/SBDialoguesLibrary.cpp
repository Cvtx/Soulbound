// Copyright Cvtx. All Rights Reserved.


#include "SBDialoguesLibrary.h"

float USBDialoguesLibrary::GetWaveLengthFromContext(UDialogueWave* Wave, const FDialogueContext& Context)
{
	if (Wave)
	{
		if (const USoundBase* SoundWave = Wave->GetWaveFromContext(Context))
		{
			return SoundWave->GetDuration();
		}	
	}
	
	return 0;
}