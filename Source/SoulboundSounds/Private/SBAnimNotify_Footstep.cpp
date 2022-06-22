// Copyright Cvtx. All Rights Reserved.


#include "SBAnimNotify_Footstep.h"

#include "Components/SBSoundMasterComponent.h"

USBAnimNotify_Footstep::USBAnimNotify_Footstep()
{
	FootKey = DefaultFootSocketName;	
}

void USBAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp->GetOwner()))
	{
		return;
	}
	
	USBSoundMasterComponent* SoundComponent = MeshComp->GetOwner()->FindComponentByClass<USBSoundMasterComponent>();
	if (!IsValid(SoundComponent))
	{
		return;
	}

	if (!SoundComponent->bAutoPlace)
	{
		if (FootKey == DefaultFootSocketName)
		{
			SoundComponent->TestAllFeetForCollision();
		}
		else
		{
			SoundComponent->TryTraceFootstep(FootKey);
		}
	}	
}
