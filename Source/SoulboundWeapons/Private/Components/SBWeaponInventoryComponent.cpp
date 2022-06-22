// Copyright Cvtx. All Rights Reserved.


#include "Components/SBWeaponInventoryComponent.h"

#include "LogMacros.h"
#include "Logging.h"
#include "SBAssetManager.h"
#include "SBWeapon.h"
#include "GameFramework/Character.h"
#include "Components/SBInventorySlotsComponent.h"
#include "Components/SBInventoryComponent.h"
#include "Items/SBWeaponItem.h"

// Sets default values for this component's properties
USBWeaponInventoryComponent::USBWeaponInventoryComponent()
{

}

void USBWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Store owner ref as character
	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void USBWeaponInventoryComponent::Initialize(USBInventorySlotsComponent* InInventoryComponent)
{
	if (IsValid(InInventoryComponent))
	{
		InventorySlotsComponent = InInventoryComponent;
		if (bEquipFirstWeaponOnInitialize)
		{
			EquipWeaponAtSlotIndex(0);
		}
	}
	else
	{
		LOG_ERROR(SoulboundWeapons, "Passed InInventoryComponent is null.")	
	}
}

void USBWeaponInventoryComponent::EquipWeaponAtSlotIndex(int32 WeaponSlotIndex)
{
	if (InventorySlotsComponent == nullptr)
	{
		LOG_ERROR(SoulboundWeapons, "InventorySlotsComponent is null. Call Initialize() before usage.")
		return;
	}
					
	// Retrieve weapon in slot
	const FSBItemSlot NewWeaponSlot = FSBItemSlot(USBAssetManager::Get().WeaponItemType, WeaponSlotIndex);
	USBWeaponItem* SlottedWeapon = Cast<USBWeaponItem>(InventorySlotsComponent->GetSlottedItem(NewWeaponSlot));

	// Check retrieval
	if (SlottedWeapon == nullptr)
	{
		LOG_WARNING(SoulboundWeapons, "Invalid WeaponSlotIndex, can not find item at specified slot number.");
		return;
	}

	// Destroy current weapon actors
	for (AActor* Weapon : CurrentWeaponActors)
	{		
		Weapon->Destroy();
	}
	CurrentWeaponActors.Empty();		
	CurrentWeaponSlot = NewWeaponSlot;
	EquippedWeapon = SlottedWeapon;
	
	for (int32 Index = 0; Index < SlottedWeapon->WeaponActors.Num(); Index++)
	{
		const FActorToSlotMapping WeaponMapping = SlottedWeapon->WeaponActors[Index];				

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();			

		// Spawn weapon actor
		ASBWeapon* Weapon = GetWorld()->SpawnActor<ASBWeapon>(WeaponMapping.Actor.Get(), SpawnParameters);

		// If spawning failed
		if (Weapon == nullptr)
		{
			LOG_ERROR(SoulboundWeapons, "Unable to spawn weapon actor.");
			continue;
		}

		// Set current slot for safety checks later
		Weapon->ItemSlot.ItemType = USBAssetManager::Get().WeaponItemType;
		Weapon->ItemSlot.SlotNumber = WeaponSlotIndex;

		// Attach weapon to owner character
		AttachWeapon(Weapon, WeaponMapping.SocketName);
		
		CurrentWeaponActors.Add(Weapon);		
	}	
}

void USBWeaponInventoryComponent::AttachWeapon(ASBWeapon* Weapon, const FName &SocketName)
{
	const FDetachmentTransformRules DetachmentTransformRules =
		FDetachmentTransformRules(
			EDetachmentRule::KeepRelative,
			EDetachmentRule::KeepWorld,
			EDetachmentRule::KeepWorld,
			true);
		
	Weapon->DetachFromActor(DetachmentTransformRules);	
		
	USceneComponent* MeshSceneComponent = OwnerCharacter->GetMesh();	
	const FAttachmentTransformRules AttachmentRules = 
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
			);
	
	if (MeshSceneComponent->DoesSocketExist(SocketName) == false)
	{
		LOG_WARNING(SoulboundWeapons, "Attach socket name don't exist on owner's mesh.")
		return;
	}
	
	// Attach weapon to character mesh
	Weapon->AttachToComponent(MeshSceneComponent, AttachmentRules, SocketName);	
}

void USBWeaponInventoryComponent::EquipNextWeapon()
{
	const int32 NextWeaponSlotNumber = CurrentWeaponSlot.SlotNumber + 1;
	const FSBItemSlot NextWeaponSlot = FSBItemSlot(USBAssetManager::Get().WeaponItemType, NextWeaponSlotNumber);
	if (InventorySlotsComponent->GetSlottedItem(NextWeaponSlot))
	{
		EquipWeaponAtSlotIndex(NextWeaponSlotNumber);
	}
	else
	{
		EquipWeaponAtSlotIndex(0);
	}	
}

void USBWeaponInventoryComponent::EquipPreviousWeapon()
{
	const int32 PreviousWeaponSlotNumber = CurrentWeaponSlot.SlotNumber - 1;
	const FSBItemSlot NextWeaponSlot = FSBItemSlot(USBAssetManager::Get().WeaponItemType, PreviousWeaponSlotNumber);
	if (InventorySlotsComponent->GetSlottedItem(NextWeaponSlot))
	{
		EquipWeaponAtSlotIndex(PreviousWeaponSlotNumber);
	}
	else
	{
		const int32 LastWeaponSlotIndex = InventorySlotsComponent->GetSlotAmountForItemTypeNative(USBAssetManager::Get().WeaponItemType);
		EquipWeaponAtSlotIndex(LastWeaponSlotIndex);
	}
}

void USBWeaponInventoryComponent::UpdateCurrentWeaponSlot()
{

}

const TArray<ASBWeapon*> USBWeaponInventoryComponent::GetCurrentWeaponActors() const
{
	return CurrentWeaponActors;
}
