// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "SBAssetManager.h"
#include "Abilities/GameplayAbility.h"
#include "SBItem.generated.h"


class UGameplayAbility;

USTRUCT(BlueprintType)
struct SOULBOUNDITEMS_API FSBAbilityAtLevel
{
	GENERATED_BODY()
	
	/** Ability to grant */
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	TSubclassOf<UGameplayAbility> Ability;

	/** Level of ability. <= 0 means the character level */
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	int32 AbilityLevel;
};

// TODO: Split gameplay parameters and UI (Slate icon)

/** Base class for all items, do not blueprint directly */
UCLASS(Abstract, BlueprintType)
class SOULBOUNDITEMS_API USBItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	USBItem()		
		: MaxCount(1)
		, MaxLevel(1)
		, AbilityLevel(1)
	{}

	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;
	
	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxCount;

	/** Returns if the item is consumable (MaxCount <= 0)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
	bool IsConsumable() const;

	/** Maximum level this item can be, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	/** Abilities to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TArray<FSBAbilityAtLevel> GrantedAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	int32 AbilityLevel;

	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
