// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBWeapon.h"
#include "SBSkeletalMeshWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDWEAPONS_API ASBSkeletalMeshWeapon : public ASBWeapon
{
	GENERATED_BODY()
	
public:
	
	/** Default UObject constructor. */	
	ASBSkeletalMeshWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());	
};
