// Copyright Cvtx. All Rights Reserved.

#include "SBSettingsNames.h"

///
/// Settings sections
///

FString FSBSettingsNames::GeneralProjectSettingsSection = "/Script/EngineSettings.GeneralProjectSettings";
FString FSBSettingsNames::GameUserSettingsSection = "/Script/Engine.GameUserSettings";
FString FSBSettingsNames::RendererSettingsSection = "/Script/Engine.RendererSettings";

FString FSBSettingsNames::DefaultInputBindingSettingsSection = "DefaultInputBindingSettings";

///
/// Settings keys
///

FString FSBSettingsNames::ProjectVersionKey = "ProjectVersionKey";
FString FSBSettingsNames::BenchmarkAlreadyRan = "BenchmarkAlreadyRan";
FString FSBSettingsNames::MotionBlurEnabled = "MotionBlurEnabled";
FString FSBSettingsNames::LensFlaresEnabled = "LensFlaresEnabled";
FString FSBSettingsNames::SSREnabled = "SSREnabled";
FString FSBSettingsNames::BloomEnabled = "BloomEnabled";
FString FSBSettingsNames::AntiAliasingMethod = "AntiAliasingMethod";
FString FSBSettingsNames::AmbientOcclusionEnabled = "AmbientOcclusionEnabled";
FString FSBSettingsNames::Brightness = "Brightness";

FString FSBSettingsNames::MasterAudioVolume = "MasterAudioVolume";
FString FSBSettingsNames::MusicAudioVolume = "MusicAudioVolume";
FString FSBSettingsNames::SFXAudioVolume = "SFXAudioVolume";
FString FSBSettingsNames::VoiceAudioVolume = "VoiceAudioVolume";

FString FSBSettingsNames::Difficulty = "Difficulty";
FString FSBSettingsNames::Language = "Language";
FString FSBSettingsNames::SubtitlesEnabled = "SubtitlesEnabled";

FString FSBSettingsNames::InvertLookX = "InvertLookX";
FString FSBSettingsNames::InvertLookY = "InvertLookY";
FString FSBSettingsNames::CameraSpeedMultiplierX = "CameraSpeedMultiplierX";
FString FSBSettingsNames::CameraSpeedMultiplierY = "CameraSpeedMultiplierY";
FString FSBSettingsNames::Rumble = "Rumble";

///
/// Console commands
///

FString FSBSettingsNames::MotionBlurCommand = "r.MotionBlur.Amount ";
FString FSBSettingsNames::LensFlaresCommand = "r.DefaultFeature.LensFlare ";
FString FSBSettingsNames::SSRCommand = "ShowFlag.ScreenSpaceReflections ";
FString FSBSettingsNames::BloomCommand = "ShowFlag.Bloom ";
FString FSBSettingsNames::AntiAliasingCommand = "r.DefaultFeature.AntiAliasing ";
FString FSBSettingsNames::AmbientOcclusionCommand = "r.DefaultFeature.AmbientOcclusion ";
