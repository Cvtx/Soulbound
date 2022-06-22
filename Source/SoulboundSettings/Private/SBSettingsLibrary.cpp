// Copyright Cvtx. All Rights Reserved.


#include "SBSettingsLibrary.h"

#include "SBDefaultSettings.h"
#include "SBSettingsNames.h"
#include "Sound/SoundClass.h"
#include "GameFramework/GameUserSettings.h"

// Available languages defined in GameSettingsWidgetBP
static TArray<FString> LanguageCodeArray;

///
/// General 
///

FString USBSettingsLibrary::GetProjectVersion()
{
	if (!GConfig) return "0";
	
	FString ProjectVersion;
	
	GConfig->GetString(
		*FSBSettingsNames::GeneralProjectSettingsSection,
		*FSBSettingsNames::ProjectVersionKey,
		ProjectVersion,
		GGameIni
	);

	return ProjectVersion;
}

bool USBSettingsLibrary::IsBenchmarkAlreadyRan()
{
	if (!GConfig) return false;

	bool BenchmarkAlreadyRan = false;

	// Did the Game User Settings Benchmark already ran?
	GConfig->GetBool(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::BenchmarkAlreadyRan,
		BenchmarkAlreadyRan,
		GGameUserSettingsIni
	);

	return BenchmarkAlreadyRan;	
}

void USBSettingsLibrary::SetBenchmarkAlreadyRan(bool BenchmarkAlreadyRan)
{
	if (!GConfig) return;
	
	// Set BenchmarkAlreadyRan Variable
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::BenchmarkAlreadyRan,
		BenchmarkAlreadyRan,
		GGameUserSettingsIni
	);	
}

///
/// Display 
///

bool USBSettingsLibrary::GetSupportedScreenResolutions(TArray<FString>& Resolutions, int32& CurrentScreenResolutionIndex)
{
	FScreenResolutionArray ResolutionsArray;

	if (RHIGetAvailableResolutions(ResolutionsArray, true))  // needs the "RHI" dependency
		{
		UGameUserSettings* Settings = GetGameUserSettings();
		if (Settings == nullptr)
		{
			return false;
		}

		FIntPoint CurrentResolution = Settings->GetScreenResolution();

		for (int32 i = 0; i < ResolutionsArray.Num(); i++)
		{
			if (ResolutionsArray[i].Width < MIN_SCREEN_WIDTH || ResolutionsArray[i].Height < MIN_SCREEN_HEIGHT)
			{
				continue;
			}

			FString Str = FString::FromInt(ResolutionsArray[i].Width) + "x" + FString::FromInt(ResolutionsArray[i].Height);
			Resolutions.AddUnique(Str);

			if (ResolutionsArray[i].Width == CurrentResolution.X && ResolutionsArray[i].Height == CurrentResolution.Y)
			{
				CurrentScreenResolutionIndex = Resolutions.Num() - 1;
			}
		}

		return true;
		}

	return false;  // failed to obtain screen resolutions	
}

FString USBSettingsLibrary::GetScreenResolution()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (Settings == nullptr)
	{
		return FString("");
	}

	FIntPoint Resolution = Settings->GetScreenResolution();
	return FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);	
}

///
/// Graphics 
///

bool USBSettingsLibrary::GetPostProcessingSettings(FSBPostProcessingSettings& PostProcessingSettings)
{
	if (!GConfig) return false;
			
	{		
		// Get Motion Blur
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::RendererSettingsSection,
			*FSBSettingsNames::MotionBlurEnabled,
			PostProcessingSettings.MotionBlur,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			PostProcessingSettings.MotionBlur = USBDefaultSettings::MotionBlur;
		}
	}
	
	{
		// Get Lens Flares
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::RendererSettingsSection,
			*FSBSettingsNames::LensFlaresEnabled,
			PostProcessingSettings.LensFlares,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			PostProcessingSettings.LensFlares = USBDefaultSettings::LensFlares;
		}
	}

	{
		// Get SSR
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::RendererSettingsSection,
			*FSBSettingsNames::SSREnabled,
			PostProcessingSettings.SSR,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			PostProcessingSettings.SSR = USBDefaultSettings::SSR;
		}
	}

	{
		// Get Bloom
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::RendererSettingsSection,
			*FSBSettingsNames::BloomEnabled,
			PostProcessingSettings.Bloom,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			PostProcessingSettings.Bloom = USBDefaultSettings::Bloom;
		}
	}

	{
		// Get Anti Aliasing
		const bool ValueRetrieved = GConfig->GetInt(
			*FSBSettingsNames::RendererSettingsSection,
			*FSBSettingsNames::AntiAliasingMethod,
			PostProcessingSettings.AntiAliasing,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			PostProcessingSettings.AntiAliasing = USBDefaultSettings::AntiAliasing;
		}
	}

	{
		// Get Ambient Occlusion
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::RendererSettingsSection,
			*FSBSettingsNames::AmbientOcclusionEnabled,
			PostProcessingSettings.AmbientOcclusion,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			PostProcessingSettings.AmbientOcclusion = USBDefaultSettings::AmbientOcclusion;
		}
	}
	
	// Execute console commands
	ApplyPostProcessingSettings(PostProcessingSettings);

	return true;
}

bool USBSettingsLibrary::SetPostProcessingSettings(const FSBPostProcessingSettings& PostProcessingSettings)
{
	if (!GConfig) return false;
	
	// Set Motion Blur
	GConfig->SetBool(
		*FSBSettingsNames::RendererSettingsSection,
		*FSBSettingsNames::MotionBlurEnabled,
		PostProcessingSettings.MotionBlur,
		GGameUserSettingsIni
	);

	// Set Lens Flares
	GConfig->SetBool(
		*FSBSettingsNames::RendererSettingsSection,
		*FSBSettingsNames::LensFlaresEnabled,
		PostProcessingSettings.LensFlares,
		GGameUserSettingsIni
	);

	// Set SSR
	GConfig->SetBool(
		*FSBSettingsNames::RendererSettingsSection,
		*FSBSettingsNames::SSREnabled,
		PostProcessingSettings.SSR,
		GGameUserSettingsIni
	);

	// Set Bloom
	GConfig->SetBool(
		*FSBSettingsNames::RendererSettingsSection,
		*FSBSettingsNames::BloomEnabled,
		PostProcessingSettings.Bloom,
		GGameUserSettingsIni
	);

	// Set Anti Aliasing
	GConfig->SetInt(
		*FSBSettingsNames::RendererSettingsSection,
		*FSBSettingsNames::AntiAliasingMethod,
		PostProcessingSettings.AntiAliasing,
		GGameUserSettingsIni
	);

	// Set Ambient Occlusion
	GConfig->SetBool(
		*FSBSettingsNames::RendererSettingsSection,
		*FSBSettingsNames::AmbientOcclusionEnabled,
		PostProcessingSettings.AmbientOcclusion,
		GGameUserSettingsIni
	);

	// Execute console commands
	ApplyPostProcessingSettings(PostProcessingSettings);

	return true;
}

void USBSettingsLibrary::ApplyPostProcessingSettings(const FSBPostProcessingSettings& PostProcessingSettings)
{
	{
		// Motion Blur	
		FString Command = FSBSettingsNames::MotionBlurCommand;
		Command += PostProcessingSettings.MotionBlur ? "0.5" : "0";
		GEngine->Exec(GEngine->GetWorld(), *Command);
	}

	{
		// Lens Flares
		FString Command = FSBSettingsNames::LensFlaresCommand;
		Command += PostProcessingSettings.LensFlares ? "1" : "0";
		GEngine->Exec(GEngine->GetWorld(), *Command);
	}

	{
		// SSR
		FString Command = FSBSettingsNames::SSRCommand;
		Command += PostProcessingSettings.SSR ? "1" : "0";
		GEngine->Exec(GEngine->GetWorld(), *Command);
	}

	{
		// Bloom
		FString Command = FSBSettingsNames::BloomCommand;
		Command += PostProcessingSettings.Bloom ? "1" : "0";
		GEngine->Exec(GEngine->GetWorld(), *Command);
	}

	{
		// Anti Aliasing
		FString Command = FSBSettingsNames::AntiAliasingCommand;
		Command += FString::FromInt(PostProcessingSettings.AntiAliasing);
		GEngine->Exec(GEngine->GetWorld(), *Command);
	}

	{
		// Ambient Occlusion
		FString Command = FSBSettingsNames::AmbientOcclusionCommand;
		Command += PostProcessingSettings.AmbientOcclusion ? "1" : "0";
		GEngine->Exec(GEngine->GetWorld(), *Command);
	}	
}

bool USBSettingsLibrary::SetDisplaySettings(const FSBDisplaySettings& DisplaySettings)
{
	if (!GConfig) return false;
	
	// Brightness / Gamma
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::Brightness,
		DisplaySettings.Brightness,
		GGameUserSettingsIni
	);

	return true;	
}

bool USBSettingsLibrary::GetDisplaySettings(FSBDisplaySettings& DisplaySettings)
{
	if (!GConfig) return false;
		
	// Brightness / Gamma
	const bool ValueRetrieved = GConfig->GetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::Brightness,
		DisplaySettings.Brightness,
		GGameUserSettingsIni
	);

	// Set to default if failed to read
	if (ValueRetrieved == false)
	{
		DisplaySettings.Brightness = USBDefaultSettings::Brightness;
	}

	return true;	
}

void USBSettingsLibrary::GetVRAM(float& UsedVRAM, float& TotalVRAM)
{
	FTextureMemoryStats TextureMemoryStats;
	RHIGetTextureMemoryStats(TextureMemoryStats);
	UsedVRAM = (float)TextureMemoryStats.AllocatedMemorySize / 1024 / 1024 / 1024;
	TotalVRAM = (float)TextureMemoryStats.TotalGraphicsMemory / 1024 / 1024 / 1024;	
}

///
/// Audio 
///

bool USBSettingsLibrary::GetAudioSettings(FSBAudioSettings& AudioSettings)
{
	//UE_LOG(LogTemp, Warning, TEXT("GET MasterAudioVolume: %f"), MasterAudioVolume);
	if (!GConfig) return false;

	{
		// Master Audio Volume
		const bool ValueRetrieved = GConfig->GetFloat(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::MasterAudioVolume,
			AudioSettings.MasterAudioVolume,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			AudioSettings.MasterAudioVolume = USBDefaultSettings::MasterAudioVolume;
		}
	}

	{
		// Music Audio Volume
		const bool ValueRetrieved = GConfig->GetFloat(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::MusicAudioVolume,
			AudioSettings.MusicAudioVolume,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			AudioSettings.MusicAudioVolume = USBDefaultSettings::MusicAudioVolume;
		}
	}

	{
		// SFX Audio Volume
		const bool ValueRetrieved = GConfig->GetFloat(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::SFXAudioVolume,
			AudioSettings.SFXAudioVolume,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			AudioSettings.SFXAudioVolume = USBDefaultSettings::SFXAudioVolume;
		}
	}

	{
		// Voice Audio Volume
		const bool ValueRetrieved = GConfig->GetFloat(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::VoiceAudioVolume,
			AudioSettings.VoiceAudioVolume,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			AudioSettings.VoiceAudioVolume = USBDefaultSettings::VoiceAudioVolume;
		}
	}

	return true;	
}

bool USBSettingsLibrary::SetAudioSettings(const FSBAudioSettings& AudioSettings)
{	
	if (!GConfig) return false;
	
	// Config for Master Audio Volume
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::MasterAudioVolume,
		AudioSettings.MasterAudioVolume,
		GGameUserSettingsIni
	);

	// Adjust Master Audio Volume
	if (AudioSettings.MasterSoundClass)
	{
		AudioSettings.MasterSoundClass->Properties.Volume = USBDefaultSettings::MasterAudioVolume;
	}

	// Config for Music Audio Volume
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::MusicAudioVolume,
		AudioSettings.MusicAudioVolume,
		GGameUserSettingsIni
	);

	// Adjust Music Audio Volume
	if (AudioSettings.MusicSoundClass)
	{
		AudioSettings.MusicSoundClass->Properties.Volume = USBDefaultSettings::MusicAudioVolume;
	}

	// Config for SFX Audio Volume
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::SFXAudioVolume,
		AudioSettings.SFXAudioVolume,
		GGameUserSettingsIni
	);

	// Adjust SFX Audio Volume
	if (AudioSettings.SFXSoundClass)
	{
		AudioSettings.SFXSoundClass->Properties.Volume = USBDefaultSettings::SFXAudioVolume;
	}

	// Config for Voice Audio Volume
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::VoiceAudioVolume,
		AudioSettings.VoiceAudioVolume,
		GGameUserSettingsIni
	);

	// Adjust Voice Audio Volume
	if (AudioSettings.VoiceSoundClass)
	{
		AudioSettings.VoiceSoundClass->Properties.Volume = USBDefaultSettings::VoiceAudioVolume;
	}

	//GConfig->Flush(false, GGameUserSettingsIni);

	return true;	
}

///
/// Game 
///

bool USBSettingsLibrary::GetGameSettings(FSBGameSettings& GameSettings)
{
	if (!GConfig) return false;

	{
		// Set Difficulty
		const bool ValueRetrieved = GConfig->GetInt(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::Difficulty,
			GameSettings.Difficulty,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			GameSettings.Difficulty = USBDefaultSettings::Difficulty;
		}
	}

	{
		// Set Language
		const bool ValueRetrieved = GConfig->GetInt(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::Language,
			GameSettings.Language,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			GameSettings.Language = USBDefaultSettings::Language;
		}
	}

	{
		// Subtitles 
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::SubtitlesEnabled,
			GameSettings.SubtitlesEnabled,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			GameSettings.SubtitlesEnabled = USBDefaultSettings::SubtitlesEnabled;
		}
	}

	// Return available languages set in GameSettingsWidgetBP
	GameSettings.Languages = LanguageCodeArray;

	return true;	
}

bool USBSettingsLibrary::SetGameSettings(const FSBGameSettings& GameSettings)
{
	if (!GConfig) return false;
	
	// Set Difficulty
	GConfig->SetInt(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::Difficulty,
		GameSettings.Difficulty,
		GGameUserSettingsIni
	);

	// Set Language
	GConfig->SetInt(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::Language,
		GameSettings.Language,
		GGameUserSettingsIni
	);

	// Subtitles
	GConfig->SetBool(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::SubtitlesEnabled,
		GameSettings.SubtitlesEnabled,
		GGameUserSettingsIni
	);

	return true;	
}

///
/// Controls 
///

bool USBSettingsLibrary::GetControlsSettings(FSBControlsSettings& ControlsSettings)
{
	if (!GConfig) return false;
	
	{
		// Get Invert Look X
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::InvertLookX,
			ControlsSettings.bInvertLookX,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			ControlsSettings.bInvertLookX = USBDefaultSettings::bInvertLookX;
		}
	}

	{
		// Get Invert Look Y
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::InvertLookY,
			ControlsSettings.bInvertLookY,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			ControlsSettings.bInvertLookY = USBDefaultSettings::bInvertLookY;
		}
	}

	{
		// Get Camera Speed Multiplier X
		const bool ValueRetrieved = GConfig->GetFloat(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::CameraSpeedMultiplierX,
			ControlsSettings.CameraSpeedX,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			ControlsSettings.CameraSpeedX = USBDefaultSettings::CameraSpeedX;
		}
	}

	{
		// Get Camera Speed Multiplier Y
		const bool ValueRetrieved = GConfig->GetFloat(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::CameraSpeedMultiplierY,
			ControlsSettings.CameraSpeedY,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			ControlsSettings.CameraSpeedY = USBDefaultSettings::CameraSpeedY;
		}
	}

	{
		// Get Rumble On/Off
		const bool ValueRetrieved = GConfig->GetBool(
			*FSBSettingsNames::GameUserSettingsSection,
			*FSBSettingsNames::Rumble,
			ControlsSettings.bRumble,
			GGameUserSettingsIni
		);

		if (ValueRetrieved == false)
		{
			ControlsSettings.bRumble = USBDefaultSettings::bRumble;
		}
	}

	return true;	
}

bool USBSettingsLibrary::SetControlsSettings(const FSBControlsSettings& ControlsSettings)
{
	if (!GConfig) return false;
	
	// Set Invert Look X
	GConfig->SetBool(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::InvertLookX,
		ControlsSettings.bInvertLookX,
		GGameUserSettingsIni
	);

	// Set Invert Look Y
	GConfig->SetBool(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::InvertLookY,
		ControlsSettings.bInvertLookY,
		GGameUserSettingsIni
	);

	// Set Camera Speed Multiplier X
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::CameraSpeedMultiplierX,
		ControlsSettings.CameraSpeedX,
		GGameUserSettingsIni
	);

	// Set Camera Speed Multiplier Y
	GConfig->SetFloat(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::CameraSpeedMultiplierY,
		ControlsSettings.CameraSpeedY,
		GGameUserSettingsIni
	);

	// Set Rumble 
	GConfig->SetBool(
		*FSBSettingsNames::GameUserSettingsSection,
		*FSBSettingsNames::Rumble,
		ControlsSettings.bRumble,
		GGameUserSettingsIni
	);

	return true;	
}

FString USBSettingsLibrary::GetDefaultInputBindingsVersion()
{
	if (!GConfig) return "";
	
	FString DefaultInputBindingsVersion;

	GConfig->GetString(
		*FSBSettingsNames::DefaultInputBindingSettingsSection,
		TEXT("DefaultInputBindingsVersion"),
		DefaultInputBindingsVersion,
		GGameUserSettingsIni
	);

	return DefaultInputBindingsVersion;	
}

void USBSettingsLibrary::SetDefaultInputBindingsVersion(const FString& DefaultInputBindingsVersion)
{
	if (!GConfig) return;
	
	// Set BenchmarkAlreadyRan Variable
	GConfig->SetString(
		*FSBSettingsNames::DefaultInputBindingSettingsSection,
		TEXT("DefaultInputBindingsVersion"),
		*DefaultInputBindingsVersion,
		GGameUserSettingsIni
	);	
}

FString USBSettingsLibrary::ActionKeyMappingToString(const FInputActionKeyMapping& InputActionKeyMapping)
{
	UScriptStruct* Struct = InputActionKeyMapping.StaticStruct();
	FString Output = TEXT("");
	Struct->ExportText(Output, &InputActionKeyMapping, nullptr, USBSettingsLibrary::StaticClass(), (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
	return Output;	
}

FInputActionKeyMapping USBSettingsLibrary::StringToActionKeyMapping(const FString& ActionKeyMappingString)
{
	FInputActionKeyMapping ActionKeyMapping;
	UScriptStruct* Struct = ActionKeyMapping.StaticStruct();
	Struct->ImportText(*ActionKeyMappingString, &ActionKeyMapping, USBSettingsLibrary::StaticClass(), PPF_None, GWarn, Struct->GetName());
	return ActionKeyMapping;	
}

TArray<FInputActionKeyMapping> USBSettingsLibrary::GetDefaultActionMappings()
{
	TArray<FInputActionKeyMapping> ActionKeyMappings;
	TArray<FString> ActionKeyMappingsString;

	if (!GConfig) return ActionKeyMappings;

	GConfig->GetArray(
		*FSBSettingsNames::DefaultInputBindingSettingsSection,
		TEXT("ActionMappings"),
		ActionKeyMappingsString,
		GGameUserSettingsIni
	);

	FInputActionKeyMapping ActionMapping;
	for (int i = 0; i < ActionKeyMappingsString.Num(); i++) {
		ActionMapping = StringToActionKeyMapping(ActionKeyMappingsString[i]);
		ActionKeyMappings.Add(ActionMapping);
	}

	return ActionKeyMappings;	
}

bool USBSettingsLibrary::SetDefaultActionMappings(const TArray<FInputActionKeyMapping>& ActionKeyMappings)
{
	if (!GConfig) return false;

	TArray<FString> ActionKeyMappingsString;
	FString ActionMapping;
	for (int i = 0; i < ActionKeyMappings.Num(); i++) {
		ActionMapping = ActionKeyMappingToString(ActionKeyMappings[i]);
		ActionKeyMappingsString.Add(ActionMapping);
	}

	GConfig->SetArray(
		*FSBSettingsNames::DefaultInputBindingSettingsSection,
		TEXT("ActionMappings"),
		ActionKeyMappingsString,
		GGameUserSettingsIni
	);

	return true;	
}

FString USBSettingsLibrary::AxisKeyMappingToString(const FInputAxisKeyMapping& InputAxisKeyMapping)
{
	UScriptStruct* Struct = InputAxisKeyMapping.StaticStruct();
	FString Output = TEXT("");
	Struct->ExportText(Output, &InputAxisKeyMapping, nullptr, USBSettingsLibrary::StaticClass(), (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
	return Output;	
}

FInputAxisKeyMapping USBSettingsLibrary::StringToAxisKeyMapping(const FString& AxisKeyMappingString)
{
	FInputAxisKeyMapping AxisKeyMapping;
	UScriptStruct* Struct = AxisKeyMapping.StaticStruct();
	Struct->ImportText(*AxisKeyMappingString, &AxisKeyMapping, USBSettingsLibrary::StaticClass(), PPF_None, GWarn, Struct->GetName());
	return AxisKeyMapping;	
}

TArray<FInputAxisKeyMapping> USBSettingsLibrary::GetDefaultAxisMappings()
{
	TArray<FInputAxisKeyMapping> AxisKeyMappings;
	TArray<FString> AxisKeyMappingsString;

	if (!GConfig) return AxisKeyMappings;

	GConfig->GetArray(
		*FSBSettingsNames::DefaultInputBindingSettingsSection,
		TEXT("AxisMappings"),
		AxisKeyMappingsString,
		GGameUserSettingsIni
	);

	FInputAxisKeyMapping AxisMapping;
	for (int i = 0; i < AxisKeyMappingsString.Num(); i++) {
		AxisMapping = StringToAxisKeyMapping(AxisKeyMappingsString[i]);
		AxisKeyMappings.Add(AxisMapping);
	}

	return AxisKeyMappings;	
}

bool USBSettingsLibrary::SetDefaultAxisMappings(const TArray<FInputAxisKeyMapping>& AxisKeyMappings)
{
	if (!GConfig) return false;

	TArray<FString> AxisKeyMappingsString;
	FString AxisMapping;
	for (int i = 0; i < AxisKeyMappings.Num(); i++) {
		AxisMapping = AxisKeyMappingToString(AxisKeyMappings[i]);
		AxisKeyMappingsString.Add(AxisMapping);
	}

	GConfig->SetArray(
		*FSBSettingsNames::DefaultInputBindingSettingsSection,
		TEXT("AxisMappings"),
		AxisKeyMappingsString,
		GGameUserSettingsIni
	);

	return true;	
}

///
/// Other 
///

bool USBSettingsLibrary::SetPersistentVariable(const FString SettingsSection, const FString CustomVariableName,
	const FString CustomVariableValue)
{
	if (!GConfig) return false;

	// Set Motion Blur
	GConfig->SetString(
		*SettingsSection,
		*CustomVariableName,
		*CustomVariableValue,
		GGameUserSettingsIni
	);

	return true;	
}

bool USBSettingsLibrary::GetPersistentVariable(FString& CustomVariableValue, FString SettingsSection, FString CustomVariableName,
	FString CustomVariableValueDefault)
{
	if (!GConfig) return false;

	bool success;

	// Set Motion Blur
	success = GConfig->GetString(
		*SettingsSection,
		*CustomVariableName,
		CustomVariableValue,
		GGameUserSettingsIni
	);

	if (!success)
	{
		CustomVariableValue = CustomVariableValueDefault;
	}

	return true;	
}

int USBSettingsLibrary::GetCurrentViewMode(const APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		UGameViewportClient* GameViewportClient = PlayerController->GetWorld()->GetGameViewport();
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

		const bool IsInputIgnored = GameViewportClient->IgnoreInput();
		const EMouseCaptureMode MouseCaptureMode = GameViewportClient->GetMouseCaptureMode();

		if (IsInputIgnored == false && MouseCaptureMode == EMouseCaptureMode::CaptureDuringMouseDown)
		{
			return 0;  // Game And UI
		}
		else if (IsInputIgnored == true && MouseCaptureMode == EMouseCaptureMode::NoCapture)
		{
			return 1;  // UI Only
		}
		else
		{
			return 2;  // Game Only
		}
	}

	return -1;	
}

void USBSettingsLibrary::SetLanguageCodeArray(const TArray<FString>& InLanguageCodeArray)
{
	LanguageCodeArray = InLanguageCodeArray;
}

TArray<FString> USBSettingsLibrary::GetLanguageCodeArray()
{
	return LanguageCodeArray;
}

UGameUserSettings* USBSettingsLibrary::GetGameUserSettings()
{
	if (GEngine != nullptr)
	{
		return GEngine->GameUserSettings;
	}

	return nullptr;
}
