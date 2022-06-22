// Copyright Cvtx. All Rights Reserved.

#include "SBTargetingFilter.h"

USBTargetingFilter::USBTargetingFilter()
{

}

bool USBTargetingFilter::IsActorTargetable_Implementation(const AActor* componentOwner, const AActor* Target) const
{
	return true;
}

