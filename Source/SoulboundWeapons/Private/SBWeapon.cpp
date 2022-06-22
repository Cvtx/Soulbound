// Copyright Cvtx. All Rights Reserved.


#include "SBWeapon.h"
#include "SBAssetManager.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "SBTraceComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/OrientedCapsuleComponent.h"

FName ASBWeapon::MeshComponentName(TEXT("WeaponMesh0"));
FName ASBWeapon::CapsuleComponentName(TEXT("CollisionCylinder"));
FName ASBWeapon::TraceComponentName(TEXT("TraceComponent"));

ASBWeapon::ASBWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{	
	CapsuleComponent = CreateDefaultSubobject<UOrientedCapsuleComponent>(ASBWeapon::CapsuleComponentName);
	
	if (CapsuleComponent)
	{
		SetRootComponent(CapsuleComponent);
		CapsuleComponent->CapsuleRotation = FRotator(90, 90, 0);
		CapsuleComponent->SetCapsuleSize(5, 50);
	}
	
	Mesh = CreateDefaultSubobject<UMeshComponent>(ASBWeapon::MeshComponentName);
	if (Mesh)
	{
		Mesh->SetupAttachment(CapsuleComponent);
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		//Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;		
		static FName MeshCollisionProfileName(TEXT("NoCollision"));
		Mesh->SetCollisionProfileName(MeshCollisionProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
	}
	
	TraceComponent = CreateDefaultSubobject<USBTraceComponent>(ASBWeapon::TraceComponentName);
	if (TraceComponent)
	{
		TraceComponent->SetupAttachment(Mesh);
		//TraceComponent->OnHit.AddDynamic(&ASBWeapon::OnCollisionDetected);
	}
}

void ASBWeapon::OnCollisionDetected(FName TraceName, const FHitResult& HitResult)
{
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator = GetInstigator();
	GameplayEventData.Target = HitResult.GetActor();

	// Pass hit information back to ability
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), AttackEventTag, GameplayEventData);	
}

void ASBWeapon::BeginAttack(FGameplayTag EventTag)
{
	IsAttacking = true;
	//CollisionManager->StartAllTraces();
	TraceComponent->Enable(Mesh, false);
	AttackEventTag = EventTag;
}

void ASBWeapon::EndAttack()
{
	IsAttacking = false;
	//CollisionManager->StopAllTraces();
	TraceComponent->Disable(Mesh);
}

void ASBWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Setup item slot as weapon slot
	ItemSlot.ItemType = USBAssetManager::Get().WeaponItemType;
	ItemSlot.SlotNumber = -1;
	
}

void ASBWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
