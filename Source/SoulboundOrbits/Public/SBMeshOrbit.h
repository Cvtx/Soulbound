// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBOrbitManagerBase.h"
#include "SBMeshOrbit.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDORBITS_API ASBMeshOrbit final : public ASBOrbitManagerBase
{
	GENERATED_BODY()

public:
	ASBMeshOrbit();
	
	/** The Static Mesh that will be spawned on the orbit */
	UPROPERTY(EditAnywhere, Category=Orbit)
	TObjectPtr<UStaticMesh> StaticMesh;

	/** Sets the collision type for the spawned Instanced Static Meshes */
	UPROPERTY(EditAnywhere, Category=Orbit)
	TEnumAsByte<ECollisionEnabled::Type> CollisionType;

	/** Each entry represents its own orbit */
	UPROPERTY(EditAnywhere, Category=Orbit)
	TArray<FSBOrbitData> OrbitData;
	
protected:
	//~Begin ASBOrbitManagerBase interface
	virtual void Initiate() override;
	virtual void Update(float NewTime) override;
	virtual void DrawDebugOrbits() override;
	//~End ASBOrbitManagerBase interface

private:
	/** */
	UPROPERTY(EditDefaultsOnly, Category=Components)
	TObjectPtr<UInstancedStaticMeshComponent> Meshes;
	
};
