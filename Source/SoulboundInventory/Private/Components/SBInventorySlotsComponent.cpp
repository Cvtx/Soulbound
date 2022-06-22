// Copyright Cvtx. All Rights Reserved.


#include "Components/SBInventorySlotsComponent.h"
#include "Logging.h"
#include "LogMacros.h"
#include "Items/SBItem.h"
#include "SBAssetManager.h"
#include "SBGameInstanceBase.h"
#include "Components/SBInventoryComponent.h"

USBInventorySlotsComponent::USBInventorySlotsComponent()
{
		
}

void USBInventorySlotsComponent::BeginPlay()
{
	// Load inventory slots off save game before starting play	
	Super::BeginPlay();
	
	InventoryComponent = GetOwner()->FindComponentByClass<USBInventoryComponent>();
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryLoadedNative.AddUObject(this, &USBInventorySlotsComponent::HandleInventoryLoaded);
		InventoryComponent->OnInventoryItemChangedNative.AddUObject(this, &USBInventorySlotsComponent::HandleInventoryItemChanged);
		LoadSlots();
	}
	else
	{
		LOG_FATAL(SoulboundInventory, "Can't find USBInventoryComponent on Owner.");
	}	
}

void USBInventorySlotsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}


bool USBInventorySlotsComponent::SetSlottedItem(FSBItemSlot ItemSlot, USBItem* Item)
{
	// Iterate entire inventory because we need to remove from old slot
	bool bFound = false;
	for (TPair<FSBItemSlot, USBItem*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			// Add to new slot
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			// If this item was found in another slot, remove it
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	return bFound;
	
	if (bFound)
	{
		SaveSlots();		
	}	
}

USBItem* USBInventorySlotsComponent::GetSlottedItem(FSBItemSlot ItemSlot) const
{
	USBItem* const* FoundItem = SlottedItems.Find(ItemSlot);

	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;	
}

void USBInventorySlotsComponent::GetSlottedItems(TArray<USBItem*>& Items, FPrimaryAssetType ItemType)
{
	GetSlottedItemsNative(Items, ItemType);
}

void USBInventorySlotsComponent::GetSlottedItemsNative(TArray<USBItem*>& Items, FPrimaryAssetType ItemType)
{
	for (TPair<FSBItemSlot, USBItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == ItemType || !ItemType.IsValid())
		{
			Items.Add(Pair.Value);
		}
	}	
}

int32 USBInventorySlotsComponent::GetSlotAmountForItemType(FPrimaryAssetType ItemType)
{
	return GetSlotAmountForItemTypeNative(ItemType);
}

int32 USBInventorySlotsComponent::GetSlotAmountForItemTypeNative(FPrimaryAssetType ItemType)
{
	TArray<USBItem*> Items;
	GetSlottedItemsNative(Items, ItemType);
	return Items.Num();
}

void USBInventorySlotsComponent::FillEmptySlots()
{		
	TArray<USBItem*> Items;
	InventoryComponent->GetInventoryItemsNative(Items);

	LOG_WARNING_PRINTF(SoulboundInventory, "Вещей в инвентаре: %d", Items.Num());
	
	bool bShouldSave = false;
	for (USBItem* Item : Items)
	{
		bShouldSave |= FillEmptySlotWithItem(Item);
	}

	if (bShouldSave)
	{
		//SaveSlots();
	}	
}

bool USBInventorySlotsComponent::FillEmptySlotWithItem(USBItem* NewItem)
{
	// Look for an empty item slot to fill with this item
	const FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FSBItemSlot EmptySlot;
	for (TPair<FSBItemSlot, USBItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// Item is already slotted
				return false;
			}
			
			if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > Pair.Key.SlotNumber))
			{
				// We found an empty slot worth filling
				EmptySlot = Pair.Key;
			}
		}
	}

	if (EmptySlot.IsValid())
	{
		SlottedItems[EmptySlot] = NewItem;
		NotifySlottedItemChanged(EmptySlot, NewItem);
		return true;
	}

	return false;
}

void USBInventorySlotsComponent::NotifyInventorySlotsLoaded() const
{
	// Notify native before blueprint
	if (OnInventorySlotsLoadedNative.IsBound())
	{
		OnInventorySlotsLoadedNative.Broadcast();
	}

	if (OnInventorySlotsLoaded.IsBound())
	{
		OnInventorySlotsLoaded.Broadcast();		
	}	
}

void USBInventorySlotsComponent::NotifySlottedItemChanged(FSBItemSlot ItemSlot, USBItem* Item)
{
	// Notify native before blueprint
	if (OnSlottedItemChangedNative.IsBound())
	{
		OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);	
	}

	if (OnSlottedItemChanged.IsBound())
	{
		OnSlottedItemChanged.Broadcast(ItemSlot, Item);	
	}	

	// Call BP update event
	SlottedItemChanged(ItemSlot, Item);	
}

void USBInventorySlotsComponent::HandleSaveGameLoaded(USBSaveGame* NewSaveGame)
{
	LoadSlots();	
}

void USBInventorySlotsComponent::HandleInventoryLoaded()
{
	LoadSlots();
}

void USBInventorySlotsComponent::HandleInventoryItemChanged(USBItem* ChangedItem)
{
	const int32 NewItemCount = InventoryComponent->GetInventoryItemCount(ChangedItem);

	if (NewItemCount == 0)
	{
		// If item removed, make sure it is unslotted
		for (TPair<FSBItemSlot, USBItem*>& Pair : SlottedItems)
		{
			if (Pair.Value == ChangedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}
	}
}

bool USBInventorySlotsComponent::SaveSlots()
{
	UWorld* World = GetWorld();
	USBGameInstanceBase* GameInstance = World ? World->GetGameInstance<USBGameInstanceBase>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	USBSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		// Reset cached data in save game before writing to it
		CurrentSaveGame->SlottedItems.Reset();

		for (const TPair<FSBItemSlot, USBItem*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlottedItems.Add(SlotPair.Key, AssetId);
		}
		
		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	
	return false;
}

bool USBInventorySlotsComponent::LoadSlots()
{
	SlottedItems.Reset();
	
	// Fill in slots from game instance
	UWorld* World = GetWorld();
	USBGameInstanceBase* GameInstance = World ? World->GetGameInstance<USBGameInstanceBase>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	// Bind to loaded callback if not already bound
	if (!GameInstance->OnSaveGameLoadedNative.IsBoundToObject(this))
	{
		GameInstance->OnSaveGameLoadedNative.AddUObject(this, &USBInventorySlotsComponent::HandleSaveGameLoaded);
	}

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
		{
			SlottedItems.Add(FSBItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	USBSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	USBAssetManager& AssetManager = USBAssetManager::Get();
	if (CurrentSaveGame)
	{
		bool bFoundAnySlots = false;
		for (const TPair<FSBItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				USBItem* LoadedItem = Cast<USBItem>(AssetManager.ForceLoadAsset(SlotPair.Value));
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}
			
		if (!bFoundAnySlots)
		{
			// Auto slot items as no slots were saved
			FillEmptySlots();
		}				
	}

	NotifyInventorySlotsLoaded();
	return false;
}
