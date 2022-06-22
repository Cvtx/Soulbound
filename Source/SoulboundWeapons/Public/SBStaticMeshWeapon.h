// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBWeapon.h"
#include "SBStaticMeshWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDWEAPONS_API ASBStaticMeshWeapon : public ASBWeapon
{
	GENERATED_BODY()

public:
	
	/** Default UObject constructor. */	
	ASBStaticMeshWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
