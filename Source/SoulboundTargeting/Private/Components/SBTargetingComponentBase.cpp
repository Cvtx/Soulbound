// Copyright Cvtx. All Rights Reserved.


#include "Components/SBTargetingComponentBase.h"
#include <GameFramework/Controller.h>


bool USBTargetingComponentBase::IsTargetInSight() const
{
	return IsActorInSight(CurrentTarget);
}

bool USBTargetingComponentBase::IsLockedOnTarget() const
{
	return IsValid(CurrentTarget);
}

bool USBTargetingComponentBase::IsActorInSight(const AActor* Actor) const
{
	AController* Controller = Cast<AController>(GetOwner());
	return Controller ? Controller->LineOfSightTo(Actor) : false; 
}
