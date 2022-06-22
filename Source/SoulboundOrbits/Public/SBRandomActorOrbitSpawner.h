// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "SBOrbitManagerBase.h"
#include "SBRandomActorOrbitSpawner.generated.h"

class UChildActorComponent;

/**
 * 
 */
UCLASS()
class SOULBOUNDORBITS_API ASBRandomActorOrbitSpawner : public ASBOrbitManagerBase
{
	GENERATED_BODY()

public:
	/** The amount of orbits to generate */
	UPROPERTY(EditAnywhere, Category="Spawn Settings", meta = (ClampMin = "0", UIMin = "0"))
	int32 SpawnAmount;

	/** Specify a seed that is used for random generation */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	int32 Seed;

	/** The actor class that will be spawned on the orbit */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	TSubclassOf<AActor> ActorClass;

	/** The default rotation of the actor */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	FRotator DefaultRotation;
	
	/** The default scale of the actor */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	FVector DefaultScale;

	/** Specify the default rotation for the generated Orbits */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	FRotator DefaultOrbitRotation;
	
	/** Rotate actor objects relative to the orbit */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	uint8 bRotateRelativeToOrbit : 1;

	/** Contains all randomization settings */
	UPROPERTY(EditAnywhere, Category="Spawn Settings")
	FSBOrbitRandomization Randomization;
	
	ASBRandomActorOrbitSpawner();

protected:	
	//~Begin ASBOrbitManagerBase interface
	virtual void Initiate() override;
	virtual void Update(float NewTime) override;
	virtual void DrawDebugOrbits() override;
	//~End ASBOrbitManagerBase interface

private:
	/** */
	UPROPERTY(VisibleInstanceOnly, Category=Orbit)
	TArray<FSBOrbitData> OrbitData;

	/** */
	UPROPERTY(VisibleInstanceOnly, Category=Orbit)
	TArray<UChildActorComponent*> Actors;

	/** */
	FRandomStream GenerationStream;
};


