// Copyright Cvtx. All Rights Reserved.

#include "SoulboundTrace.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSoulboundTrace"

void FSoulboundTrace::StartupModule()
{
}

void FSoulboundTrace::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoulboundTrace, SoulboundTrace);