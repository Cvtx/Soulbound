// Copyright Cvtx. All Rights Reserved.


#include "SBOrbitActorConnector.h"

ASBOrbitActorConnector::ASBOrbitActorConnector()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASBOrbitActorConnector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePositions();
}

void ASBOrbitActorConnector::SetAttachActor(AActor* ActorToAttach)
{
	AttachActor = ActorToAttach;
}

void ASBOrbitActorConnector::SetFollowActor(AActor* ActorToFollow)
{
	FollowActor = ActorToFollow;
}

void ASBOrbitActorConnector::UpdatePositions()
{
	if (IsValid(FollowActor))
	{
		SetActorLocation(FollowActor->GetActorLocation());
	}

	if (IsValid(AttachActor))
	{
		AttachActor->SetActorLocation(GetActorLocation());
	}
}

