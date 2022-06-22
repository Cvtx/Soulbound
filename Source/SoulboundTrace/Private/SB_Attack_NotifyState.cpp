// Copyright Cvtx. All Rights Reserved.

#include "SB_Attack_NotifyState.h"
#include "SBTraceComponent.h"

void USB_Attack_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (MeshComp)
	{
		if (const AActor* Owner = MeshComp->GetOwner())
		{
			USBTraceComponent* CombatManager = Owner->FindComponentByClass<USBTraceComponent>();
			if(!IsValid(CombatManager))
			{
				// 	Note: We might also want to check if Owner is not a AAnimationEditorPreviewActor actor (notifiers are triggered in persona preview viewport)
				return;
			}

			//CombatManager->Enable();
		}
	}
}

void USB_Attack_NotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (MeshComp)
	{
		if (const AActor* Owner = MeshComp->GetOwner())
		{
			USBTraceComponent* CombatManager = Owner->FindComponentByClass<USBTraceComponent>();
			if(!IsValid(CombatManager))
			{
				// 	Note: We might also want to check if Owner is not a AAnimationEditorPreviewActor actor (notifiers are triggered in persona preview viewport)
				return;
			}

			//CombatManager->EndTrace();
		}
	}
}
