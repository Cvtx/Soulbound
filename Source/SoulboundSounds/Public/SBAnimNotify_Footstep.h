// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SBAnimNotify_Footstep.generated.h"

/**
 * 
 */
UCLASS()
class SOULBOUNDSOUNDS_API USBAnimNotify_Footstep : public UAnimNotify
{
	GENERATED_BODY()

public:
	USBAnimNotify_Footstep();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Soulbound")
	FName FootKey = DefaultFootSocketName;	

private:
	const FName DefaultFootSocketName = "DefaultTestAll";
	
};
