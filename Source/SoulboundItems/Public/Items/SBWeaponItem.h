// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "SBItem.h"
#include "SBTypes.h"

#include "SBWeaponItem.generated.h"


USTRUCT(BlueprintType)
struct  FActorToSlotMapping
{
	GENERATED_BODY()

	FActorToSlotMapping()
	{
		
	}
	
	FActorToSlotMapping(TSubclassOf<AActor> ActorClass, const FName& Socket)
		: Actor(ActorClass)
		, SocketName(Socket)
	{
		
	}
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditDefaultsOnly)
	FName SocketName;
};


/** Native base class for weapons, should be blueprinted */
UCLASS()
class SOULBOUNDITEMS_API USBWeaponItem : public USBItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	USBWeaponItem()
	{
		ItemType = USBAssetManager::WeaponItemType;
	}

	/** Weapon actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TArray<FActorToSlotMapping> WeaponActors;

	// /** Abilities this weapon grants if this weapon is slotted */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	// TArray<TSubclassOf<USBGameplayAbility>> GrantedAbilities;
};
