// Copyright Cvtx. All Rights Reserved.

#include "SoulboundLogging.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSoulboundLogging"

void FSoulboundLogging::StartupModule()
{
}

void FSoulboundLogging::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoulboundLogging, SoulboundLogging);