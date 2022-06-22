// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "SBFootstepConfigDataAsset.h"
#include "Components/ActorComponent.h"
#include "SBSoundMasterComponent.generated.h"

class ACharacter;
class USBFootstepConfigDataAsset;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCharacterMadeFootstepSound, FHitResult, HitEvent,float, VolumeMulti,UAudioComponent*, Sound, UNiagaraSystem* , Particle,FName, Key);

UCLASS(Blueprintable, ClassGroup=("Soulbound|Sound"), meta=(BlueprintSpawnableComponent) )
class SOULBOUNDSOUNDS_API USBSoundMasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Soulbound|Footstep")
	FCharacterMadeFootstepSound CharacterMadeFootstepSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep Setup")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep Setup")
	float SurfaceTraceLength = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep Setup")
	TObjectPtr<USBFootstepConfigDataAsset> FootstepSetup;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep Setup")
	TMap<FName, FName> FeetDefinitions = { { "L", "foot_l"}, {"R", "foot_r" } };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep AutoSetup")
	bool bAutoPlace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep AutoSetup")
	float MinTimeCycle = 0.25f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Footstep AutoSetup")
	bool bAutoVolume;

	UPROPERTY(BlueprintReadOnly, Category = "Soulbound|Internal")
	TObjectPtr<ACharacter> OwnerAsCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Soulbound|Internal")
	TArray<FName> AutoFootstepsArray;

	UPROPERTY(BlueprintReadOnly, Category = "Soulbound|Internal")
	FName PreviousFoot;

	#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Global Setup")
	bool bDebug = false;
	#endif

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soulbound|Global Setup")
	bool bTraceComplex = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Soulbound|Global Setup")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

private:
	FTimerHandle AutoFootStepGateTimerHandle;
	bool bAutoFootStepGateOpen = true;

public:	
	USBSoundMasterComponent();

	virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Trace by foot name
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Footstep")
	bool TryTraceFootstep(const FName Key);

	UFUNCTION(BlueprintCallable, Category = "Soulbound|Footstep")
	void FootstepEffect(const FHitResult& Hit, UAudioComponent*& Sound, UNiagaraSystem*& Particle);

	// Auto footstep placement
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Footstep")
	void TestAllFeetForCollision();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Soulbound|Footstep")
	float OverwriteCalcVolume() const;
	virtual float OverwriteCalcVolume_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Soulbound|Footstep")
	FVector OverwriteParticleScale() const;
	virtual  FVector OverwriteParticleScale_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Soulbound|Footstep")
	void OverwritePickSurface(const TEnumAsByte<EPhysicalSurface>& InSurface, TEnumAsByte<EPhysicalSurface>& OutSurface);
	virtual void OverwritePickSurface_Implementation(const TEnumAsByte<EPhysicalSurface>& InSurface, TEnumAsByte<EPhysicalSurface>& OutSurface);

private:
	void RecastOwner();

	void FootstepSoundEffect(USoundCue* SoundToPlay, const FVector& Location ,UAudioComponent*& FootStepSound) const;
	void FootstepParticleEffect(UNiagaraSystem* ParticleToSpawn, const FVector& Location, const FVector& Normal, UNiagaraSystem*& NiagaraSystem) const;

	void OpenAutoFootStepGate();
	void CloseAutoFootStepGate();

		
};
