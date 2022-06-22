// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Items/SBItemSlot.h"
#include "SBWeapon.generated.h"

class USphereComponent;
class UMeshComponent;
class UCapsuleComponent;
class USBWeaponItem;
class USBTraceComponent;
class UOrientedCapsuleComponent;

UCLASS()
class SOULBOUNDWEAPONS_API ASBWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ASBWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable)
	void BeginAttack(FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USBWeaponItem> WeaponItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	FSBItemSlot ItemSlot;

public:

	/** Returns CapsuleComponent subobject  */
	UFUNCTION(BlueprintCallable, Category=Components)
	FORCEINLINE UOrientedCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

	/** Name of the CapsuleComponent component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName CapsuleComponentName;
	
	/** Returns Mesh subobject  */
	UFUNCTION(BlueprintCallable, Category=Components)
	FORCEINLINE UMeshComponent* GetMesh() const { return Mesh; }

	/** Name of the Mesh component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName MeshComponentName;

	/** Returns TraceComponent subobject  */
	UFUNCTION(BlueprintCallable, Category=Components)
	FORCEINLINE USBTraceComponent* GetTraceComponent() const { return TraceComponent; }
	
	/** Name of the Trace component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName TraceComponentName;

	 
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	void OnCollisionDetected(FName TraceName, const FHitResult& HitResult); 
	
	UPROPERTY()
	uint8 IsAttacking: 1;

	FGameplayTag AttackEventTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UOrientedCapsuleComponent> CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMeshComponent> Mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USBTraceComponent> TraceComponent;
};
