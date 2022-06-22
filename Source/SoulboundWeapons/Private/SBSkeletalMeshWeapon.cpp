// Copyright Cvtx. All Rights Reserved.


#include "SBSkeletalMeshWeapon.h"

ASBSkeletalMeshWeapon::ASBSkeletalMeshWeapon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletalMeshComponent>(ASBWeapon::MeshComponentName))
{
}
