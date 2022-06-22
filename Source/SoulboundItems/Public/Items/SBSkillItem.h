// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "SBItem.h"
#include "SBSkillItem.generated.h"

/** Native base class for skills, should be blueprinted */
UCLASS()
class SOULBOUNDITEMS_API USBSkillItem : public USBItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	USBSkillItem()
	{
		ItemType = USBAssetManager::SkillItemType;
	}
};
