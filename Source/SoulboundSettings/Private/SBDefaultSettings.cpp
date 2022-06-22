// Copyright Cvtx. All Rights Reserved.


#include "SBDefaultSettings.h"

// PostProcessing settings
int32 USBDefaultSettings::AntiAliasing = 2;
bool USBDefaultSettings::MotionBlur = true;
bool USBDefaultSettings::LensFlares = true;
bool USBDefaultSettings::SSR = true;
bool USBDefaultSettings::Bloom = true;	
bool USBDefaultSettings::AmbientOcclusion = true;

// Display settings
float USBDefaultSettings::Brightness = 2.2f;

// Audio settings
float USBDefaultSettings::MasterAudioVolume = 1.f;
float USBDefaultSettings::MusicAudioVolume = 1.f;
float USBDefaultSettings::SFXAudioVolume = 1.f;
float USBDefaultSettings::VoiceAudioVolume = 1.f;

// Game settings
int32 USBDefaultSettings::Difficulty = 1;
int32 USBDefaultSettings::Language = 0;
bool USBDefaultSettings::SubtitlesEnabled = true;
TArray<FString> USBDefaultSettings::Languages;

// Controls settings
bool USBDefaultSettings::bInvertLookX = false;
bool USBDefaultSettings::bInvertLookY = false;
float USBDefaultSettings::CameraSpeedX = 1.f;
float USBDefaultSettings::CameraSpeedY =  1.f;
bool USBDefaultSettings::bRumble = false;
