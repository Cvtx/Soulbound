// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SBSettingsTypes.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SBSettingsLibrary.generated.h"

#define MIN_SCREEN_WIDTH 1024
#define MIN_SCREEN_HEIGHT 768

/**
 * @brief Library dedicated to change in-game settings and gather useful info.
 */
UCLASS()
class SOULBOUNDSETTINGS_API USBSettingsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	///
	/// General 
	///
	
	// Get current version of the project
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|General")
	static FString GetProjectVersion();

	// Check if the game user settings benchmark already ran
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|General")
	static bool IsBenchmarkAlreadyRan();

	// Sets benchmark already ran variable to remember next start
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|General")
	static void SetBenchmarkAlreadyRan(bool BenchmarkAlreadyRan = false);

	///
	/// Display 
	///
	
	// Get a list of screen resolutions supported on this machine
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Display")
	static bool GetSupportedScreenResolutions(TArray<FString>& Resolutions, int32& CurrentScreenResolutionIndex);

	// Get currently set screen resolution
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Display")
	static FString GetScreenResolution();

	///
	/// Graphics 
	///
	
	// Load Post Processing Settings and execute Commands
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Graphics")
	static bool GetPostProcessingSettings(FSBPostProcessingSettings& PostProcessingSettings);

	// Set the post processing settings (not applied nor saved yet)
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Graphics")
	static bool SetPostProcessingSettings(const FSBPostProcessingSettings &PostProcessingSettings);

	// Apply Post Process Settings trough console commands
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Graphics")
	static void ApplyPostProcessingSettings(const FSBPostProcessingSettings &PostProcessingSettings);

	// Set the new display settings 
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Graphics")
	static bool SetDisplaySettings(const FSBDisplaySettings &DisplaySettings);

	// Get the current display settings 
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Graphics")
	static bool GetDisplaySettings(FSBDisplaySettings &DisplaySettings);

	// Get the used and total VRAM
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Graphics")
	static void GetVRAM(float& UsedVRAM, float& TotalVRAM);

	///
	/// Audio 
	///
	
	// Get the current audio settings (Default values are only applied if there is nothing saved yet)
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Audio")
	static bool GetAudioSettings(FSBAudioSettings& AudioSettings);

	// Set the audio settings
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Audio")
	static bool SetAudioSettings(const FSBAudioSettings & AudioSettings);

	///
	/// Game 
	///
	
	// Get the game settings
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Game")
	static bool GetGameSettings(FSBGameSettings& GameSettings);

	// Set the audio settings
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Game")
	static bool SetGameSettings(const FSBGameSettings& GameSettings);

	///
	/// Controls 
	///
	
	// Get the current controls settings (Default values are only applied if there is nothing saved yet)
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Controls")
	static bool GetControlsSettings(FSBControlsSettings& ControlsSettings);

	// Set the new controls settings
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Controls")
	static bool SetControlsSettings(const FSBControlsSettings &ControlsSettings);

	// Get the default input bindings version
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static FString GetDefaultInputBindingsVersion();

	// Set the default input bindings version
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Controls")
	static void SetDefaultInputBindingsVersion(const FString &DefaultInputBindingsVersion);

	// Convert Action Mapping UStruct to String
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static FString ActionKeyMappingToString(const FInputActionKeyMapping &InputActionKeyMapping);

	// Convert String to Action Mapping UStruct 
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static FInputActionKeyMapping StringToActionKeyMapping(const FString &ActionKeyMappingString);

	// Get Default Action Mappings
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static TArray<FInputActionKeyMapping> GetDefaultActionMappings();

	// Set Default Action Mappings
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Controls")
	static bool SetDefaultActionMappings(const TArray<FInputActionKeyMapping> &ActionKeyMappings);

	// Convert Axis Mapping UStruct to String
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static FString AxisKeyMappingToString(const FInputAxisKeyMapping &InputAxisKeyMapping);

	// Convert String to Axis Mapping UStruct 
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static FInputAxisKeyMapping StringToAxisKeyMapping(const FString &AxisKeyMappingString);

	// Get Default Axis Mappings
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static TArray<FInputAxisKeyMapping> GetDefaultAxisMappings();

	// Set Default Axis Mappings
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Controls")
	static bool SetDefaultAxisMappings(const TArray<FInputAxisKeyMapping> &AxisKeyMappings);

	///
	/// Other 
	///
	
	// Save Persistent Variable (Custom fields)
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Custom Variable")
	static bool SetPersistentVariable(const FString SettingsSection = "/Script/Engine.GameUserSettings", const FString CustomVariableName = "Name", const FString CustomVariableValue = "Value");

	// Load Persistent Variable (Custom fields)
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Custom Variable")
	static bool GetPersistentVariable(FString& CustomVariableValue, FString SettingsSection = "/Script/Engine.GameUserSettings", FString CustomVariableName = "Name", FString CustomVariableValueDefault = "");

	// Get Current View Mode of Player Controller
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Controls")
	static int GetCurrentViewMode(const APlayerController* PlayerController);

	// Set Language Code Array
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Settings|Localization")
	static void SetLanguageCodeArray(const TArray<FString> &InLanguageCodeArray);

	// Get Language Code Array
	UFUNCTION(BlueprintPure, Category = "Soulbound|Settings|Localization")
	static TArray<FString> GetLanguageCodeArray();

private:
	
	// Try to get the GameUserSettings object from the engine
	static UGameUserSettings* GetGameUserSettings();
	
};
