// Copyright Cvtx. All Rights Reserved.

#include "SoulboundInventory.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSoulboundInventory"

void FSoulboundInventory::StartupModule()
{
}

void FSoulboundInventory::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoulboundInventory, SoulboundInventory);