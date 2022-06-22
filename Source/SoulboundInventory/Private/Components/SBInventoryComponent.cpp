        // Copyright Cvtx. All Rights Reserved.


#include "Components/SBInventoryComponent.h"
#include "Logging.h"
#include "LogMacros.h"
#include "Items/SBItem.h"
#include "SBAssetManager.h"
#include "SBGameInstanceBase.h"

USBInventoryComponent::USBInventoryComponent()
{	

}

void USBInventoryComponent::BeginPlay()
{
	// Load inventory off save game before starting play
	LoadInventory();
	Super::BeginPlay();
}

bool USBInventoryComponent::AddInventoryItem(USBItem* NewItem, int32 ItemCount, int32 ItemLevel)
{
	bool bChanged = false;
	if (!NewItem)
	{
		LOG_WARNING(SoulboundInventory, "Failed trying to add null item!");
		LOG_SCREENMSG("Failed trying to add null item!");
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		LOG_WARNING_PRINTF(SoulboundInventory, "Failed trying to add item %s with negative count or level!", *NewItem->GetName());
		LOG_SCREENMSG_PRINTF("Failed trying to add item %s with negative count or level!", *NewItem->GetName());
		return false;
	}

	// Find current item data, which may be empty
	FSBItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	// Find modified data
	FSBItemData NewData = OldData;
	NewData.UpdateItemData(FSBItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);

	if (OldData != NewData)
	{
		// If data changed, need to update storage and call callback
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(NewItem);
		bChanged = true;
	}

	if (bChanged)
	{
		// If anything changed, write to save game
		SaveInventory();
		return true;
	}
	return false;	
}

bool USBInventoryComponent::RemoveInventoryItem(USBItem* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem)
	{
		LOG_WARNING(SoulboundInventory, "Failed trying to remove null item!");
		return false;
	}

	// Find current item data, which may be empty
	FSBItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
	}

	// Update item count
	NewData.ItemCount -= RemoveCount;

	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// Remove item entirely
		InventoryData.Remove(RemovedItem);
	}

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(RemovedItem);
	SaveInventory();
	
	return true;
}

void USBInventoryComponent::GetInventoryItems(TArray<USBItem*>& Items, FPrimaryAssetType ItemType)
{
	GetInventoryItemsNative(Items, ItemType);
}

void USBInventoryComponent::GetInventoryItemsNative(TArray<USBItem*>& Items, FPrimaryAssetType ItemType)
{
	for (const TPair<USBItem*, FSBItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId = Pair.Key->GetPrimaryAssetId();

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}	
	}	
}

int32 USBInventoryComponent::GetInventoryItemCount(USBItem* Item) const
{
	const FSBItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}
	return 0;	
}

bool USBInventoryComponent::GetInventoryItemData(USBItem* Item, FSBItemData& ItemData) const
{
	const FSBItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}
	ItemData = FSBItemData(0, 0);
	return false;	
}

bool USBInventoryComponent::SaveInventory()
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
		CurrentSaveGame->InventoryData.Reset();		

		for (const TPair<USBItem*, FSBItemData>& ItemPair : InventoryData)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->InventoryData.Add(AssetId, ItemPair.Value);
			}
		}

		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool USBInventoryComponent::LoadInventory()
{
	InventoryData.Reset();

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
		GameInstance->OnSaveGameLoadedNative.AddUObject(this, &USBInventoryComponent::HandleSaveGameLoaded);
	}
	
	USBSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	USBAssetManager& AssetManager = USBAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into data		
		for (const TPair<FPrimaryAssetId, FSBItemData>& ItemPair : CurrentSaveGame->InventoryData)
		{
			USBItem* LoadedItem = Cast<USBItem>(AssetManager.ForceLoadAsset(ItemPair.Key));

			if (LoadedItem != nullptr)
			{
				InventoryData.Add(LoadedItem, ItemPair.Value);
			}
		}
	
		NotifyInventoryLoaded();
		return true;
	}

	// Load failed but we reset inventory, so need to notify UI
	NotifyInventoryLoaded();

	return false;	
}

void USBInventoryComponent::NotifyInventoryItemChanged(USBItem* Item)
{
	// Notify native before blueprint	
	if (OnInventoryItemChangedNative.IsBound())
	{
		OnInventoryItemChangedNative.Broadcast(Item);
	}

	if (OnInventoryItemChanged.IsBound())
	{
		OnInventoryItemChanged.Broadcast(Item);		
	}		

	// Call BP update event
	InventoryItemChanged(Item);	
}

void USBInventoryComponent::NotifyInventoryLoaded() const
{
	// Notify native before blueprint
	if (OnInventoryLoadedNative.IsBound())
	{
		OnInventoryLoadedNative.Broadcast();
	}

	if (OnInventoryLoaded.IsBound())
	{
		OnInventoryLoaded.Broadcast();		
	}		
}


void USBInventoryComponent::HandleSaveGameLoaded(USBSaveGame* NewSaveGame)
{
	LoadInventory();	
}
