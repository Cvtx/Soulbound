// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "Sound/SoundCue.h"
#include "SBFootstepConfigDataAsset.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SOULBOUNDSOUNDS_API USBFootstepConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Soulboun|Setup")
	float MinVelocity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Soulboun|Setup")
	float MaxVelocity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Soulboun|Setup")
	TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> Sounds = {
		{EPhysicalSurface::SurfaceType_Default, nullptr},
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Soulboun|Setup")
	TMap<TEnumAsByte<EPhysicalSurface>, UNiagaraSystem*> Particles = {
		{EPhysicalSurface::SurfaceType_Default, nullptr},
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Soulboun|Setup")
	USoundAttenuation* Attenuation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Soulboun|Setup")
	USoundConcurrency* Concurrency;

public:
	// Get sound cues relative to the physical surfaces called
	UFUNCTION(BlueprintCallable, Category = "Soulboun Footsteps")
	void GetSurfaceReaction(TEnumAsByte<EPhysicalSurface> InputParam, USoundCue*& Sound, UNiagaraSystem*& Particle);

	UFUNCTION(BlueprintCallable, Category = "Soulboun Footsteps")
	void GetVelocityRange(float& MinRange , float& MaxRange) const;

	UFUNCTION(BlueprintCallable, Category = "Soulboun Footsteps")
	void GetFootStepSettings(USoundAttenuation*& AttenuationRef, USoundConcurrency*& ConcurrencyRef) const;	
	
};
