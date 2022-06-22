// Copyright Cvtx. All Rights Reserved.


#include "SBFootstepConfigDataAsset.h"

void USBFootstepConfigDataAsset::GetSurfaceReaction(const TEnumAsByte<EPhysicalSurface> InputParam, USoundCue*& Sound, UNiagaraSystem*& Particle)
{
	// Find sound by surface type
	USoundCue** SoundFound = Sounds.Find(Sounds.Contains(InputParam)
		? InputParam
		: static_cast<TEnumAsByte<EPhysicalSurface>>(EPhysicalSurface::SurfaceType_Default));
	Sound = SoundFound != nullptr ? *SoundFound : nullptr;

	// Find particle by surface type
	UNiagaraSystem** ParticleFound = Particles.Find(Particles.Contains(InputParam)
		? InputParam
		: static_cast<TEnumAsByte<EPhysicalSurface>>(EPhysicalSurface::SurfaceType_Default));
	Particle = ParticleFound != nullptr ? *ParticleFound : nullptr;
}

void USBFootstepConfigDataAsset::GetVelocityRange(float& MinRange, float& MaxRange) const
{
	MinRange = MinVelocity;
	MaxRange = MaxVelocity;
}

void USBFootstepConfigDataAsset::GetFootStepSettings(USoundAttenuation*& AttenuationRef, USoundConcurrency*& ConcurrencyRef) const
{
	AttenuationRef = Attenuation;
	ConcurrencyRef = Concurrency;
}
