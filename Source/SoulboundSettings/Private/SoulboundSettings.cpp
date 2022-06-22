// Copyright Cvtx. All Rights Reserved.

#include "SoulboundSettings.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSoulboundSettings"

void FSoulboundSettings::StartupModule()
{
}

void FSoulboundSettings::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoulboundSettings, SoulboundSettings);