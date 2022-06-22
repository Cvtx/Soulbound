// Copyright Cvtx. All Rights Reserved.


#include "SBMeshOrbit.h"

#include "Components/InstancedStaticMeshComponent.h"

ASBMeshOrbit::ASBMeshOrbit()
{
	Meshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Meshes");
	if (Meshes)
	{
		Meshes->SetupAttachment(RootComponent);
	}

	Initiate();
}

void ASBMeshOrbit::Initiate()
{
	Super::Initiate();
	
	Meshes->SetCollisionEnabled(CollisionType);
	Meshes->SetStaticMesh(StaticMesh);
	Meshes->ClearInstances();
	
	for (const FSBOrbitData& Orbit : OrbitData)
	{
		FVector NewLocation;
		FRotator NewRotation;

		CalculateObjectTransform
		(
			Orbit.Rotation,
			Orbit.Radius,
			Orbit.Offset,
			Orbit.StartAngle,
			Orbit.MovementSpeed,
			Orbit.DefaultRotation,
			Orbit.RotationSpeed,
			Orbit.bRotateRelativeToOrbit,
			0.f,
			NewLocation,
			NewRotation
		);

		Meshes->AddInstanceWorldSpace(FTransform(NewRotation, NewLocation, Orbit.DefaultScale));
	}
}

void ASBMeshOrbit::DrawDebugOrbits()
{
	Super::DrawDebugOrbits();
	
	GetOverflowOrbitInstancedMeshes()->ClearInstances();
	for (const FSBOrbitData& Orbit : OrbitData)
	{
		AddOrbitPlaneInstance(Orbit);
	}
}

void ASBMeshOrbit::Update(float NewTime)
{
	Super::Update(NewTime);
	
	for (auto It = OrbitData.CreateConstIterator(); It; ++It)
	{
		FVector NewLocation;
		FRotator NewRotation;
		
		CalculateObjectTransform
		(
			It->Rotation,
			It->Radius,
			It->Offset,
			It->StartAngle,
			It->MovementSpeed,
			It->DefaultRotation,
			It->RotationSpeed,
			It->bRotateRelativeToOrbit,
			NewTime,
			NewLocation,
			NewRotation
		);

		const FTransform NewInstanceTransform = FTransform(NewRotation, NewLocation, It->DefaultScale);
		const bool MarkRenderStateDirty = It.GetIndex() == OrbitData.Num() - 1; // True if on last index

		Meshes->UpdateInstanceTransform(It.GetIndex(), NewInstanceTransform, true, MarkRenderStateDirty, false);
	}
}
