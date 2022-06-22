// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SBLookAtPointComponent.h"
#include "SBTargetPointComponent.generated.h"


UCLASS(ClassGroup = (SBTargetingSystem), Blueprintable, meta = (BlueprintSpawnableComponent))
class SOULBOUNDTARGETING_API USBTargetPointComponent : public USBLookAtPointComponent
{
	GENERATED_BODY()

public:	
	/* Returns desired camera offset for this point */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	FORCEINLINE	FName GetPointCameraEvent() const
	{
		return CameraEvent;
	}

protected:

	/* The camera event triggered when this point gets targeted */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName CameraEvent;
		
};
