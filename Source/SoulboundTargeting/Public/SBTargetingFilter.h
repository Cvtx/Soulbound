// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <GameFramework/Actor.h>
#include "SBTargetingFilter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, abstract, EditInlineNew, HideCategories = ("DoNotShow"), CollapseCategories, AutoExpandCategories = ("Default"))
class SOULBOUNDTARGETING_API USBTargetingFilter : public UObject
{
	GENERATED_BODY()
	
	USBTargetingFilter();

public:

	UFUNCTION(BlueprintNativeEvent, Category = ATS)
	bool IsActorTargetable(const AActor* componentOwner, const AActor* Target) const;
	virtual bool IsActorTargetable_Implementation(const AActor* componentOwner, const  AActor* Target) const;
};
