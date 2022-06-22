// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/SBItemSlot.h"
#include "Items/SBItemData.h"
#include "SBInventoryComponent.generated.h"

class USBItem;
class USBSaveGame;

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemChanged, USBItem*, Item);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryItemChangedNative, USBItem*);

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULBOUNDINVENTORY_API USBInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	USBInventoryComponent();

protected:	
	virtual void BeginPlay() override;

public:			

	///
	/// Items
	///
	
	/** Adds a new inventory item, will add it to an empty slot if possible. If the item supports count you can add more than one count. It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddInventoryItem(USBItem* NewItem, int32 ItemCount = 1, int32 ItemLevel = 1);

	/** Remove an inventory item, will also remove from slots. A remove count of <= 0 does nothing. */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool RemoveInventoryItem(USBItem* RemovedItem, int32 RemoveCount = 1);

	/** Returns all inventory items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void GetInventoryItems(TArray<USBItem*>& Items, FPrimaryAssetType ItemType);

	/** Native version of above. */
	void GetInventoryItemsNative(TArray<USBItem*>& Items, FPrimaryAssetType ItemType = FPrimaryAssetType(NAME_None));

	/** Returns number of instances of this item found in the inventory. This uses count from GetItemData */
	UFUNCTION(BlueprintPure, Category = Inventory)
	int32 GetInventoryItemCount(USBItem* Item) const;

	/** Returns the item data associated with an item. Returns false if none found */
	UFUNCTION(BlueprintPure, Category = Inventory)
	bool GetInventoryItemData(USBItem* Item, FSBItemData& ItemData) const;
	
	///
	/// Inventory changed
	///
	
	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryItemChanged OnInventoryItemChanged;
	
	/** Native version above, called before BP delegate */
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;
	
	/** Called after the inventory was changed and we notified all delegates */
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void InventoryItemChanged(USBItem* Item);

	///
	/// Save / Load
	///
	
	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SaveInventory();

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool LoadInventory();
	
	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryLoaded OnInventoryLoaded;

	/** Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;

protected:
		
	/** Calls the inventory update callbacks */
	void NotifyInventoryItemChanged(USBItem* Item);	
	void NotifyInventoryLoaded() const;

	/** Called when a global save game as been loaded */
	void HandleSaveGameLoaded(USBSaveGame* NewSaveGame);
	
	/** Map of all items owned by this component, from definition to data */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Inventory)	
	TMap<USBItem*, FSBItemData> InventoryData;
};
