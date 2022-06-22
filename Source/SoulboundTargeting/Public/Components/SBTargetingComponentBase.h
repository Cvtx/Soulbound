// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SBTargetingComponentBase.generated.h"

class AActor;
class USBTargetPointComponent;

UCLASS(ClassGroup = (SBTargetingSystem), Blueprintable, meta = (BlueprintSpawnableComponent))
class SOULBOUNDTARGETING_API USBTargetingComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	/* Forces to target the selected actor */
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting")
	virtual	void LockOnTarget(AActor* Target) { CurrentTarget = Target; }
	
	/* Return the currently focused target */
	UFUNCTION(BlueprintPure, Category = "Soulbound|Targeting")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	/* Return the TargetPointComponent focused of the current target */
	UFUNCTION(BlueprintPure, Category = "Soulbound|Targeting")
	FORCEINLINE	USBTargetPointComponent* GetCurrentTargetPoint() const { return CurrentTargetPoint; }

	/* Returns true if the target is in sight with the actual pawn owned by the controller */
	UFUNCTION(BlueprintPure, Category = "Soulbound|Targeting")
	bool IsTargetInSight() const;

	UFUNCTION(BlueprintPure, Category = "Soulbound|Targeting")	
	virtual bool IsLockedOnTarget() const;
	
protected:

	UFUNCTION(BlueprintPure, Category = "Soulbound|Targeting")
	bool IsActorInSight(const AActor* Actor) const;
	
	/* Current target actor */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> CurrentTarget;

	/* Current target point */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USBTargetPointComponent> CurrentTargetPoint;
};
