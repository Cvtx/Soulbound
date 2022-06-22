// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SBSettingsTypes.generated.h"

class USoundClass;

// I opted to bool instead of uint8 in some fields so they can be used in GetBool methods directly

USTRUCT(BlueprintType)
struct SOULBOUNDSETTINGS_API FSBPostProcessingSettings
{
	GENERATED_BODY()

	FSBPostProcessingSettings() {};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	int32 AntiAliasing;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool MotionBlur;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool LensFlares;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool SSR;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool Bloom;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool AmbientOcclusion;
};

USTRUCT(BlueprintType)
struct SOULBOUNDSETTINGS_API FSBDisplaySettings
{
	GENERATED_BODY()

	FSBDisplaySettings() {};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float Brightness;	
};

USTRUCT(BlueprintType)
struct SOULBOUNDSETTINGS_API FSBAudioSettings
{
	GENERATED_BODY()

	FSBAudioSettings() {};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float MasterAudioVolume;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float MusicAudioVolume;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float SFXAudioVolume;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float VoiceAudioVolume;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	USoundClass* MasterSoundClass = NULL;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	USoundClass* MusicSoundClass = NULL;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	USoundClass* SFXSoundClass = NULL;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	USoundClass* VoiceSoundClass = NULL;
};

USTRUCT(BlueprintType)
struct SOULBOUNDSETTINGS_API FSBGameSettings
{
	GENERATED_BODY()

	FSBGameSettings() {};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	int32 Difficulty;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	int32 Language = 0;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool SubtitlesEnabled;
	
	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	TArray<FString> Languages;
};

USTRUCT(BlueprintType)
struct SOULBOUNDSETTINGS_API FSBControlsSettings
{
	GENERATED_BODY()

	FSBControlsSettings() {};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool bInvertLookX;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool bInvertLookY;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float CameraSpeedX;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	float CameraSpeedY;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	bool bRumble;
};

USTRUCT(BlueprintType)
struct SOULBOUNDSETTINGS_API FSBSettings
{
	GENERATED_BODY()

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	FSBGameSettings GameSettings;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	FSBPostProcessingSettings PostProcessingSettings;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	FSBDisplaySettings DisplaySettings;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	FSBAudioSettings AudioSettings;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Soulbound|Settings")
	FSBControlsSettings ControlsSettings;
};
