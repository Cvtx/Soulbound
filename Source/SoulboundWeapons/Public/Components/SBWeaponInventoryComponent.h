// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/SBItemSlot.h"
#include "SBWeaponInventoryComponent.generated.h"

class ASBWeapon;
class USBWeaponItem;
class USBInventorySlotsComponent;

UCLASS( ClassGroup=(Components), meta=(BlueprintSpawnableComponent) )
class SOULBOUNDWEAPONS_API USBWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	USBWeaponInventoryComponent();
	
	/**
	 * @brief Call this before usage.
	 * InventoryComponent may be on PlayerController or Character.
	 * Since location may be different this method is here to bound to specified inventory.
	 * @param InInventoryComponent Retrive weapons from this InventoryComponent;
	 */
	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void Initialize(USBInventorySlotsComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void EquipNextWeapon();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void EquipPreviousWeapon();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void EquipWeaponAtSlotIndex(int32 WeaponSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void UpdateCurrentWeaponSlot();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")	
	const TArray<ASBWeapon*> GetCurrentWeaponActors() const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	const USBWeaponItem* GetCurrentWeapon() const { return EquippedWeapon; };
	
protected:	
	virtual void BeginPlay() override;	
	
private:
	void AttachWeapon(ASBWeapon* Weapon, const FName &SocketName);

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<USBInventorySlotsComponent> InventorySlotsComponent;
	
	/**	Current weapon actor(s). This is an array in case when weapon consist of different parts e.g. double sword. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<ASBWeapon*> CurrentWeaponActors;
	
	UPROPERTY()
	FSBItemSlot CurrentWeaponSlot;

	UPROPERTY()
	TObjectPtr<USBWeaponItem> EquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly)
	uint8 bEquipFirstWeaponOnInitialize : 1;
};
