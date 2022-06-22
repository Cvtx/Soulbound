// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBOrbitManagerBase.h"
#include "SBActorOrbit.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDORBITS_API ASBActorOrbit final : public ASBOrbitManagerBase
{
	GENERATED_BODY()

public:
	ASBActorOrbit();
	
	/** Each entry adds a new Orbit for an Actor in a Level */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Orbit)
	TArray<FSBOrbitActorData> Actors;
	
protected:
	//~Begin ASBOrbitManagerBase interface
	virtual void Initiate() override;
	virtual void Update(float NewTime) override;
	virtual void DrawDebugOrbits() override;
	//~End ASBOrbitManagerBase interface
};
