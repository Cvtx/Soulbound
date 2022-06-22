// Copyright Cvtx. All Rights Reserved.

#include "SoulboundWeapons.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSoulboundWeapons"

void FSoulboundWeapons::StartupModule()
{
}

void FSoulboundWeapons::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoulboundWeapons, SoulboundWeapons);