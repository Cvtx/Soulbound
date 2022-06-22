// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "SBItem.h"
#include "SBTokenItem.generated.h"

/** Native base class for tokens/currency, should be blueprinted */
UCLASS()
class SOULBOUNDITEMS_API USBTokenItem : public USBItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	USBTokenItem()
	{
		ItemType = USBAssetManager::TokenItemType;
		MaxCount = 0; // Infinite
	}
};
