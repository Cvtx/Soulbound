// Copyright Cvtx. All Rights Reserved.


#include "SBOrbitActor.h"
#include "Components/StaticMeshComponent.h"

ASBOrbitActor::ASBOrbitActor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	if (Mesh)
	{
		
	}
}
