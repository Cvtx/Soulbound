// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SBTargetableInterface.generated.h"


UINTERFACE(Blueprintable)
class USBTargetableInterface : public UInterface
{
	GENERATED_BODY()
};

class USBTargetPointComponent;

class SOULBOUNDTARGETING_API ISBTargetableInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulbound|Targeting")
	bool IsTargetable(USBTargetPointComponent* Point);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulbound|Targeting")
	void OnTargetAcquired(USBTargetPointComponent* Point);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulbound|Targeting")
	void OnTargetLosed();
};
