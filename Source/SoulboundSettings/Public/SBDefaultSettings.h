// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SBDefaultSettings.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDSETTINGS_API USBDefaultSettings : public UObject
{
	GENERATED_BODY()

public:
	
	// PostProcessing settings
	static int32 AntiAliasing;
	static bool MotionBlur;
	static bool LensFlares;
	static bool SSR;
	static bool Bloom;	
	static bool AmbientOcclusion;

	// Display settings
	static float Brightness;

	// Audio settings
	static float MasterAudioVolume;
	static float MusicAudioVolume;
	static float SFXAudioVolume;
	static float VoiceAudioVolume;

	// Game settings
	static int32 Difficulty;
	static int32 Language;
	static bool SubtitlesEnabled;
	static TArray<FString> Languages;

	// Controls settings
	static bool bInvertLookX;
	static bool bInvertLookY;
	static float CameraSpeedX;
	static float CameraSpeedY;
	static bool bRumble;
};
