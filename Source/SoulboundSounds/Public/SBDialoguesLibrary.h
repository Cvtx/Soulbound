// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/DialogueWave.h"
#include "SBDialoguesLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDSOUNDS_API USBDialoguesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Soulbound|Dialogues")
	static float GetWaveLengthFromContext(UDialogueWave* Wave, const FDialogueContext& Context);
	
};
