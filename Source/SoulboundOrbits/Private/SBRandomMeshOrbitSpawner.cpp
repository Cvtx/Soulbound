// Copyright Cvtx. All Rights Reserved.


#include "SBRandomMeshOrbitSpawner.h"
#include "Components/InstancedStaticMeshComponent.h"

ASBRandomMeshOrbitSpawner::ASBRandomMeshOrbitSpawner()
{
	SpawnAmount = 5;
	DefaultScale = FVector::OneVector;
	bRotateRelativeToOrbit = true;

	Meshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Meshes");
	if (Meshes)
	{
		Meshes->SetupAttachment(RootComponent);
	}
}

void ASBRandomMeshOrbitSpawner::Initiate()
{
	Super::Initiate();
	
	Meshes->SetCollisionEnabled(CollisionType);
	Meshes->SetStaticMesh(StaticMesh);
	Meshes->ClearInstances();
	
	GenerationStream = FRandomStream(Seed);
	GenerationStream.Reset();
	OrbitData.Reset();
	
	OrbitData = GenerateRandomData
	(
		GenerationStream,
		SpawnAmount,
		Randomization,
		DefaultOrbitRotation,
		DefaultRotation,
		DefaultScale,
		bRotateRelativeToOrbit
	);
	
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

		Meshes->AddInstanceWorldSpace(FTransform(NewRotation, NewLocation - GetActorLocation(), Orbit.DefaultScale));
	}
}

void ASBRandomMeshOrbitSpawner::Update(float NewTime)
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

void ASBRandomMeshOrbitSpawner::DrawDebugOrbits()
{
	Super::DrawDebugOrbits();
	
	GetOverflowOrbitInstancedMeshes()->ClearInstances();

	for (const FSBOrbitData& Orbit : OrbitData)
	{
		AddOrbitPlaneInstance(Orbit);
	}
}