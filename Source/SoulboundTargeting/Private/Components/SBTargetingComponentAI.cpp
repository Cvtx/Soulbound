// Copyright Cvtx. All Rights Reserved.


#include "Components/SBTargetingComponentAI.h"
#include <AIController.h>
#include "Components/SBTargetPointComponent.h"
#include <Engine/TargetPoint.h>

void USBTargetingComponentAI::LockOnTarget(class AActor* Target)
{
	Super::LockOnTarget(Target);

	AAIController* ControllerOwner = Cast<AAIController>(GetOwner());
	if (Target)
	{
		USBTargetPointComponent* PointToLookAt = Target->FindComponentByClass<USBTargetPointComponent>();

		if (PointToLookAt)
		{		
			ControllerOwner->SetFocus(PointToLookAt->GetTargetPoint(), EAIFocusPriority::Gameplay);
			CurrentTargetPoint = PointToLookAt;
		}
		else
		{
			ControllerOwner->SetFocus(Target, EAIFocusPriority::Gameplay);
		}
	}
	else {

		ControllerOwner->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
