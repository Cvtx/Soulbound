// Copyright Cvtx. All Rights Reserved.


#include "SBRandomActorOrbitSpawner.h"
#include "Components/ChildActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"


ASBRandomActorOrbitSpawner::ASBRandomActorOrbitSpawner()
{
	SpawnAmount = 5;
	DefaultScale = FVector::OneVector;
	bRotateRelativeToOrbit = true;
	ActorClass = AActor::StaticClass();
	
	//Initiate();
}

void ASBRandomActorOrbitSpawner::Initiate()
{
	Super::Initiate();
	
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

	for (UChildActorComponent* Actor : Actors)
	{
		if (Actor)
		{
			Actor->DestroyComponent();
		}
	}
	Actors.Reset();

	for (const FSBOrbitData& Orbit : OrbitData)
	{
		FVector NewLocation = FVector();
		FRotator NewRotation = FRotator();

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
		
 		UChildActorComponent* Actor = Cast<UChildActorComponent>(AddComponentByClass(UChildActorComponent::StaticClass(), false, FTransform(NewRotation, NewLocation - GetActorLocation(), Orbit.DefaultScale), false));
		//UChildActorComponent* Actor = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor"));
		Actor->SetupAttachment(RootComponent);
		Actor->SetChildActorClass(ActorClass);
		Actor->CreateChildActor();
		Actors.Add(Actor);
	}
}

void ASBRandomActorOrbitSpawner::Update(float NewTime)
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
		
		if (Actors.IsValidIndex(It.GetIndex()))
		{
			const FTransform NewTransform = FTransform(NewRotation, NewLocation, It->DefaultScale);
			Actors[It.GetIndex()]->SetWorldTransform(NewTransform);
		}
	}
}

void ASBRandomActorOrbitSpawner::DrawDebugOrbits()
{
	Super::DrawDebugOrbits();
	
	GetOverflowOrbitInstancedMeshes()->ClearInstances();

	for (const FSBOrbitData& Orbit : OrbitData)
	{
		AddOrbitPlaneInstance(Orbit);
	}
}
