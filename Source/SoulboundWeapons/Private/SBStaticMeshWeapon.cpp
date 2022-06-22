// Copyright Cvtx. All Rights Reserved.


#include "SBStaticMeshWeapon.h"

ASBStaticMeshWeapon::ASBStaticMeshWeapon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ASBWeapon::MeshComponentName))
{
	
}
