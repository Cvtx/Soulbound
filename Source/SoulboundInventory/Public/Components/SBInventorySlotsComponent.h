// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/SBItemSlot.h"
#include "Items/SBItemData.h"
#include "SBInventorySlotsComponent.generated.h"

class USBItem;
class USBSaveGame;
class USBInventoryComponent;

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventorySlotsLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventorySlotsLoadedNative)

/** Delegate called when the contents of an inventory slot change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FSBItemSlot, ItemSlot, USBItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChangedNative, FSBItemSlot, USBItem*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULBOUNDINVENTORY_API USBInventorySlotsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USBInventorySlotsComponent();

	///
	/// Slots
	///

	/** Sets slot to item, will remove from other slots if necessary. If passing null this will empty the slot */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SetSlottedItem(FSBItemSlot ItemSlot, USBItem* Item);

	/** Returns item in slot, or null if empty */
	UFUNCTION(BlueprintPure, Category = Inventory)
	USBItem* GetSlottedItem(FSBItemSlot ItemSlot) const;

	/** Returns all slotted items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void GetSlottedItems(TArray<USBItem*>& Items, FPrimaryAssetType ItemType);

	/** Native version of above */	
	void GetSlottedItemsNative(TArray<USBItem*>& Items, FPrimaryAssetType ItemType = FPrimaryAssetType(NAME_None));

	/** */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetSlotAmountForItemType(FPrimaryAssetType ItemType);

	int32 GetSlotAmountForItemTypeNative(FPrimaryAssetType ItemType = FPrimaryAssetType(NAME_None));
	
	/** Fills in any empty slots with items in inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void FillEmptySlots();

	/** Called after an item was equipped and we notified all delegates */
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void SlottedItemChanged(FSBItemSlot ItemSlot, USBItem* Item);

	/** Delegate called when an inventory slot has changed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnSlottedItemChanged OnSlottedItemChanged;
	
	/** Native version above, called before BP delegate */
	FOnSlottedItemChangedNative OnSlottedItemChangedNative;

	///
	/// Save / Load
	///

	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventorySlotsLoaded OnInventorySlotsLoaded;

	/** Native version above, called before BP delegate */	
	FOnInventorySlotsLoadedNative OnInventorySlotsLoadedNative;

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SaveSlots();
	
	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool LoadSlots();
	
protected:	
	virtual void BeginPlay() override;

public:		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/* Inventory components slots associate */
	TObjectPtr<USBInventoryComponent> InventoryComponent;
	
	/** Auto slots a specific item, returns true if anything changed */
	bool FillEmptySlotWithItem(USBItem* NewItem);
	
	/** Calls the inventory update callbacks */
	void NotifyInventorySlotsLoaded() const;
	void NotifySlottedItemChanged(FSBItemSlot ItemSlot, USBItem* Item);

	/** Called when a global save game as been loaded */
	void HandleSaveGameLoaded(USBSaveGame* NewSaveGame);

	/** */
	void HandleInventoryItemChanged(USBItem* ChangedItem);

	/** */
	void HandleInventoryLoaded();
	
	/** Map of slot, from type/num to item, initialized from ItemSlotsPerType on SBGameInstanceBase */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Inventory)	
	TMap<FSBItemSlot, USBItem*> SlottedItems;	
};
