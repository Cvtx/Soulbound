// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"

class SOULBOUNDSETTINGS_API FSBSettingsNames
{
	
public:

	///
	/// Settings sections
	///

	/// Engine sections
	static FString GeneralProjectSettingsSection;
	static FString GameUserSettingsSection;
	static FString RendererSettingsSection;

	/// Custom sections
	static FString DefaultInputBindingSettingsSection;

	///
	/// Settings keys
	///

	/// Graphics
	static FString ProjectVersionKey;
	static FString BenchmarkAlreadyRan;
	static FString MotionBlurEnabled;
	static FString LensFlaresEnabled;
	static FString SSREnabled;
	static FString BloomEnabled;
	static FString AntiAliasingMethod;
	static FString AmbientOcclusionEnabled;
	static FString Brightness;

	/// Audio
	static FString MasterAudioVolume;
	static FString MusicAudioVolume;
	static FString SFXAudioVolume;
	static FString VoiceAudioVolume;

	/// Game
	static FString Difficulty;
	static FString Language;
	static FString SubtitlesEnabled;

	/// Input
	static FString InvertLookX;
	static FString InvertLookY;
	static FString CameraSpeedMultiplierX;
	static FString CameraSpeedMultiplierY;
	static FString Rumble;
	
	
	///
	/// Commands
	///

	static FString MotionBlurCommand;
	static FString LensFlaresCommand;
	static FString SSRCommand;
	static FString BloomCommand;
	static FString AntiAliasingCommand;
	static FString AmbientOcclusionCommand;
		
	
};
