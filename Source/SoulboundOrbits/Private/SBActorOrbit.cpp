// Copyright Cvtx. All Rights Reserved.


#include "SBActorOrbit.h"
#include "Components/InstancedStaticMeshComponent.h"


ASBActorOrbit::ASBActorOrbit()
{
	Initiate();
}

void ASBActorOrbit::Initiate()
{
	Super::Initiate();

	Update(0.f);
}

void ASBActorOrbit::Update(float NewTime)
{
	Super::Update(NewTime);
	
	for (const FSBOrbitActorData& Actor : Actors)
	{
		UpdateActorPosition(Actor, NewTime);
	}
}

void ASBActorOrbit::DrawDebugOrbits()
{
	Super::DrawDebugOrbits();
	
	GetOverflowOrbitInstancedMeshes()->ClearInstances();
	for (const FSBOrbitActorData& Actor : Actors)
	{
		AddOrbitPlaneInstance(ConvertActorDataToOrbitData(Actor));
	}
}
