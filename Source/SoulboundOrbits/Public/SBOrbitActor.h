// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "SBOrbitActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class SOULBOUNDORBITS_API ASBOrbitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASBOrbitActor();

private:
	/** */
	UPROPERTY(EditDefaultsOnly, Category=Components)
	TObjectPtr<UStaticMeshComponent> Mesh;
};
