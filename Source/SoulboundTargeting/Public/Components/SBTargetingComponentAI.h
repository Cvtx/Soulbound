// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBTargetingComponentBase.h"
#include "SBTargetingComponentAI.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (SBTargetingSystem), Blueprintable, meta = (BlueprintSpawnableComponent))
class SOULBOUNDTARGETING_API USBTargetingComponentAI : public USBTargetingComponentBase
{
	GENERATED_BODY()
	
public: 
	virtual	void LockOnTarget(class AActor* Target) override;

};
