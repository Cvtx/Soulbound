// Copyright Cvtx. All Rights Reserved.


#include "SBTraceComponent.h"
#include "LogMacros.h"
#include "Logging.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

USBTraceComponent::USBTraceComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;
	TraceMethod = ESBTraceMethod::Sockets;
	TraceChannel = ETraceTypeQuery::TraceTypeQuery4; // Visibility
	bStopOnBlock = true;
	bCheckDirection = false;
	DirectionThreshold = 0.01f;
	bCalculateVelocity = false;
	bAutoStep = true;
	SubstepMax = 5;
	SubstepPadding = 1.f;
	SubstepAngleMax = 120.f;
	InterpModifier = 1.f;

	#if SOULBOUND_DEBUG && WITH_EDITOR
	DebugType = EDrawDebugTrace::None;
	DebugDrawTime = 1.f;
	DebugPrintTime = 1.f;
	DisplayActor = true;
	DisplayComponent = true;
	DisplayCollisionType = true;
	DisplaySpeed = true;
	DisplayBone = true;
	#endif		
}

void USBTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
	OnComponentActivated.AddDynamic(this, &USBTraceComponent::Enable);
	OnComponentDeactivated.AddDynamic(this, &USBTraceComponent::Disable);	
}

void USBTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Steps = bAutoStep ? CalculateSteps() : 0;
	
	bSubdivide = Steps > 0;
	if (bSubdivide)
	{
		// Substep Trace
		switch (TraceMethod)
		{
			case ESBTraceMethod::Colliders:
				ColliderSubstepTrace();
				break;
			case ESBTraceMethod::Sockets:
				SocketSubstepTrace();
				break;
			default:				
				LOG_ERROR_PRINTF(SoulboundTrace, "Substep trace for trace method %s is not implemented", *UEnum::GetValueAsString(TraceMethod.GetValue()))
				break;
		}
	}
	else
	{
		// Trace
		switch (TraceMethod)
		{
			case ESBTraceMethod::Colliders:
				ColliderTrace();
			break;
			case ESBTraceMethod::Sockets:
				SocketTrace();
			break;
			default:
				LOG_ERROR_PRINTF(SoulboundTrace, "Trace for trace method %s is not implemented", *UEnum::GetValueAsString(TraceMethod.GetValue()))
			break;
		}		
	}
}

void USBTraceComponent::Enable(UActorComponent* Component, bool bReset)
{
	bIsEnabled = true;
	Reset();
	SetComponentTickInterval(TickInterval);	

	switch (TraceMethod)
	{
		case ESBTraceMethod::Colliders:
			GetColliders();
		break;
		case ESBTraceMethod::Sockets:
			GetSockets();
		break;
		default:
			LOG_ERROR(SoulboundTrace, "Specified TraceMethod is not implemented.")
		break;
	}

	UpdateComponentTransform();
	SetComponentTickEnabled(true);	
}

void USBTraceComponent::Disable(UActorComponent* Component)
{
	bIsEnabled = false;
	SetComponentTickEnabled(false);
}

void USBTraceComponent::Reset()
{
	Hits = 0;
	Steps = StepCount;
	
	// Ignores owning actor/instigator for trace, clears hit actors from last sweep
	IgnoreActors.Empty();
	IgnoreActors.AddUnique(GetOwner());
	IgnoreActors.AddUnique(GetOwner()->GetInstigator());
}

void USBTraceComponent::SocketTrace()
{	
	for (int i = 0; i < SocketNames.Num(); ++i)
	{
		if (ParentComponent->DoesSocketExist(SocketNames[i]) == false) continue;
				
		LineTrace(SocketTransforms[i].GetLocation(),ParentComponent->GetSocketTransform(SocketNames[i]).GetLocation());
	}
	
	UpdateComponentTransform();
	UpdateSocketTransforms();
}

void USBTraceComponent::SocketSubstepTrace()
{
	for (; CurrentStep < Steps; ++CurrentStep)
	{
		for (const FName& SocketName : SocketNames)
		{					
			if (ParentComponent->DoesSocketExist(SocketName) == false) continue;

			FVector Start;
			FVector End;
			FRotator Orientation;
			SubdivideTrace(CurrentStep, ParentComponent->GetSocketTransform(SocketName), SubstepPadding, Start, End, Orientation);					
			LineTrace(Start, End);				
		}					
	}
	CurrentStep = -1;
	UpdateComponentTransform();
	UpdateSocketTransforms();
	
}

void USBTraceComponent::LineTrace(const FVector Start, const FVector End)
{
	if (bIsEnabled == false) return;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::LineTraceMulti
	(
		this,
		Start,
		End,
		TraceChannel,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		OutHits,
		true		
	);

	HitCheck(ParentComponent, OutHits, TraceVelocity(Start, End));

	#if SOULBOUND_DEBUG && WITH_EDITOR
	if (DebugType != EDrawDebugTrace::None)
	{
		float Duration;
		switch (DebugType)
		{
			case EDrawDebugTrace::ForDuration:
				Duration = DebugDrawTime;
				break;
			case EDrawDebugTrace::ForOneFrame:
				Duration = GetWorld()->DeltaTimeSeconds * 1.1f;
				break;
			case EDrawDebugTrace::Persistent:
				Duration = 999999.f; // TODO refactor 
				break; 
			
			// will never trigger			
			case EDrawDebugTrace::None:
				Duration = 0.f;
				break;
			default:
				Duration = 0.f;
				break;
		}

		UKismetSystemLibrary::DrawDebugLine(this, Start, End, TraceColor(), Duration, 2.f);
		UKismetSystemLibrary::DrawDebugPoint(this, End, 10.f, TraceColor(), Duration);
	}
	#endif	
}

void USBTraceComponent::ColliderTrace()
{	
	for (int32 Index = 0; Index < Colliders.Num(); ++Index)
	{
		USceneComponent* Collider = Colliders[Index];

		if (IsValid(Collider) == false) continue;
		
		const FVector Start = CollidersTransforms[Index].GetLocation();
		const FVector End = Collider->GetComponentTransform().GetLocation();
		
		if (Collider->GetClass() == UBoxComponent::StaticClass())
		{
			UBoxComponent* BoxComponent = Cast<UBoxComponent>(Collider);
			const FRotator Rotator = CollidersTransforms[Index].Rotator();
			const FVector HalfSize = BoxComponent->GetScaledBoxExtent();
			BoxTrace(BoxComponent, Start, End, HalfSize, Rotator);
		}
		else if (Collider->GetClass() == USphereComponent::StaticClass())
		{				
			USphereComponent* SphereComponent = Cast<USphereComponent>(Collider);
			const float Radius = SphereComponent->GetScaledSphereRadius();
			SphereTrace(SphereComponent, Start, End, Radius);
		}
		else
		{			
			LOG_ERROR_PRINTF(SoulboundTrace, "Unable to do collider trace since class %s has no trace implementation.", *Collider->GetClass()->GetFName().ToString());
		}		
	}
	
	UpdateComponentTransform();
	UpdateColliderTransforms();
}

void USBTraceComponent::ColliderSubstepTrace()
{
	for (; CurrentStep < Steps; ++CurrentStep)
	{
		for (USceneComponent* Collider : Colliders)
		{			
			if (IsValid(Collider) == false) continue;

			FVector Start;
			FVector End;
			FRotator Orientation;
			SubdivideTrace(CurrentStep, Collider->GetComponentTransform(), SubstepPadding, Start, End, Orientation);
			
			if (Collider->GetClass() == UBoxComponent::StaticClass())
			{
				UBoxComponent* BoxComponent = Cast<UBoxComponent>(Collider);							
				const FVector HalfSize = BoxComponent->GetScaledBoxExtent();
				BoxTrace(BoxComponent, Start, End, HalfSize, Orientation);
			}
			else if (Collider->GetClass() == USphereComponent::StaticClass())
			{				
				USphereComponent* SphereComponent = Cast<USphereComponent>(Collider);				
				const float Radius = SphereComponent->GetScaledSphereRadius();
				SphereTrace(SphereComponent, Start, End, Radius);
			}
			else
			{			
				LOG_ERROR_PRINTF(SoulboundTrace, "Unable to do collider substep trace since class %s has no trace implementation.", *Collider->GetClass()->GetFName().ToString());
			}		
		}					
	}
	CurrentStep = -1;
	
	UpdateComponentTransform();
	UpdateColliderTransforms();
}

void USBTraceComponent::BoxTrace(USceneComponent* Collider, FVector Start, FVector End, FVector HalfSize, FRotator Orientation)
{
	if (bIsEnabled == false) return;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMulti
	(
		this,
		Start,
		End,
		HalfSize,
		Orientation,
		TraceChannel,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,		
		OutHits,
		true,
		TraceColor()
	);

	HitCheck(Collider, OutHits, TraceVelocity(Start, End));
}

void USBTraceComponent::SphereTrace(USceneComponent* Collider, FVector Start, FVector End, float Radius)
{
	if (bIsEnabled == false) return;
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		Start,
		End,
		Radius,
		TraceChannel,		
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		OutHits,
		true,
		TraceColor()
	);

	HitCheck(Collider, OutHits, TraceVelocity(Start, End));
}

void USBTraceComponent::GetColliders()
{	
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);
	
	Colliders.Empty();
	for (USceneComponent* Child : Children)
	{
		if (Child->GetClass() == UBoxComponent::StaticClass() || Child->GetClass() == USphereComponent::StaticClass())
		{
			Colliders.AddUnique(Child);
		}				
	}

	UpdateColliderTransforms();
}

void USBTraceComponent::UpdateColliderTransforms()
{
	CollidersTransforms.Empty();
	CollidersTransforms.SetNum(Colliders.Num());
	for (const USceneComponent* Collider : Colliders)
	{
		CollidersTransforms.Add(Collider->GetComponentTransform());		
	}
}

void USBTraceComponent::GetSockets()
{
	TArray<USceneComponent*> Parents;
	GetParentComponents(Parents); // TODO: Use this instead? GetAttachParent();

	ParentComponent = (Parents.IsValidIndex(0))  ?  Parents[0] : nullptr;	
	
	if (IsValid(ParentComponent))
	{
		SocketNames.Append(ParentComponent->GetAllSocketNames());		
		UpdateSocketTransforms();
	}		
}

void USBTraceComponent::UpdateSocketTransforms()
{	
	SocketTransforms.Empty();
	SocketTransforms.SetNum(SocketNames.Num());
	for (int i = 0; i < SocketNames.Num(); ++i)
	{
		// Is this check redundant? Since SocketNames from GetAllSocketNames().
		if (ParentComponent->DoesSocketExist(SocketNames[i]))
		{
			SocketTransforms[i] = ParentComponent->GetSocketTransform(SocketNames[i]);
		} 
	}
}

void USBTraceComponent::SubdivideTrace(int32 Index, FTransform Transform, float Padding, FVector& Start, FVector& End, FRotator& Orientation) const
{
	// Increments step along trace
	const float Ratio = (Steps == 0) ? 0 : Index / Steps;
	const float BasePadding = (0.5f / Steps);
	const float RatioStart = FMath::Clamp(Ratio - (BasePadding * Padding), 0.0f, 1.0f);
	const float RatioEnd = FMath::Clamp(1.f - Ratio - (BasePadding * Padding), 0.0f, 1.0f);
	const bool bNotLastIndex = Index < Steps;

	// Interpolates transforms based on increments between current and last transform	
	FVector LocationTransformedStart;
	FRotator RotatonTransformedStart;
	{
		const FTransform QuatInterpLerp = UKismetMathLibrary::TLerp(ComponentTransform, GetComponentTransform(), RatioStart, ELerpInterpolationMode::QuatInterp);
		const FTransform DualQuatInterpLerp = UKismetMathLibrary::TLerp(ComponentTransform, GetComponentTransform(), RatioStart, ELerpInterpolationMode::DualQuatInterp);
		// Lerps between Quat (better for long sweeps) and Dual Quat (better for tight arcs) based on curvature
		const FTransform LastTransform = UKismetMathLibrary::TLerp(QuatInterpLerp, DualQuatInterpLerp, CurveRatio, ELerpInterpolationMode::QuatInterp);
		// Calculates location and rotation for the beginning of the step
		TransformLocationRotation(Transform.GetLocation(), Transform.Rotator(), LastTransform, GetComponentTransform(),
			LocationTransformedStart, RotatonTransformedStart);
	}
	
	FVector LocationTransformedEnd;
	FRotator RotatonTransformedEnd;
	{
		const FTransform QuatInterpLerp = UKismetMathLibrary::TLerp(GetComponentTransform(), ComponentTransform, RatioEnd, ELerpInterpolationMode::QuatInterp);
		const FTransform DualQuatInterpLerp = UKismetMathLibrary::TLerp(GetComponentTransform(), ComponentTransform,  RatioEnd, ELerpInterpolationMode::DualQuatInterp);
		// Lerps between Quat (better for long sweeps) and Dual Quat (better for tight arcs) based on curvature
		const FTransform LastTransform = UKismetMathLibrary::TLerp(QuatInterpLerp, DualQuatInterpLerp, CurveRatio, ELerpInterpolationMode::QuatInterp);
		// Calculates location and rotation for the end of the step
		TransformLocationRotation(Transform.GetLocation(), Transform.Rotator(), LastTransform, GetComponentTransform(),
			LocationTransformedEnd, RotatonTransformedEnd);		
	}
	
	Start = LocationTransformedStart;
	End = LocationTransformedEnd;										
	Orientation = (bNotLastIndex)
						// Lerps rotation through sweep based on step ratio
		              ? UKismetMathLibrary::RLerp(RotatonTransformedEnd, RotatonTransformedStart, RatioStart, true)
		              : RotatonTransformedEnd;			
}

void USBTraceComponent::HitCheck(USceneComponent* Collider, const TArray<FHitResult> HitResults, FVector Velocity)
{
	for (FHitResult HitResult : HitResults)
	{
		if (IsValid(HitResult.GetActor()) == false) continue;
		
		if (CheckComponentClassInWhitelistBlackList(Collider->GetClass()) && CheckDirection(HitResult, Velocity))
		{
			if (IgnoreHitActor(HitResult))
			{
				const bool bIsCollisionBlocked = IsBlockingCollision(HitResult);
				UpdateHitCounter(HitResult);
				
				if (IsValid(Collider))
				{
					if (OnHitNative.IsBound()) OnHitNative.Broadcast(HitResult, Velocity, bIsCollisionBlocked, Collider);
					if (OnHit.IsBound()) OnHit.Broadcast(HitResult, Velocity, bIsCollisionBlocked, Collider);
				}
				else
				{
					if (OnHitNative.IsBound()) OnHitNative.Broadcast(HitResult, Velocity, bIsCollisionBlocked, ParentComponent);
					if (OnHit.IsBound()) OnHit.Broadcast(HitResult, Velocity, bIsCollisionBlocked, ParentComponent);
				}

				#if SOULBOUND_DEBUG && WITH_EDITOR
				if (DebugType != EDrawDebugTrace::None && bPrint)
				{
					// TODO Refactor colors as define
					if (DisplayActor)
					{
						UKismetSystemLibrary::PrintString(this,
							"Hit Actor: " + UKismetSystemLibrary::GetDisplayName(HitResult.GetActor()), bPrintToScreen, bPrintToLog,
							FLinearColor(0, 0.66, 1), DebugPrintTime);							
					}

					if (DisplayComponent)
					{
						UKismetSystemLibrary::PrintString(this,
						"Hit component: " + UKismetSystemLibrary::GetDisplayName(HitResult.GetComponent()), bPrintToScreen, bPrintToLog,
						FLinearColor(0, 0.66, 1), DebugPrintTime);
					}

					if (DisplayCollisionType)
					{
						UKismetSystemLibrary::PrintString(this,
						"Collision Type: " + UEnum::GetValueAsString(HitResult.GetComponent()->GetCollisionObjectType()) , bPrintToScreen, bPrintToLog,
						FLinearColor(0, 0.66, 1), DebugPrintTime);
					}

					if (DisplaySpeed)
					{							
						UKismetSystemLibrary::PrintString(this,
						"Speed : " + FString::SanitizeFloat(Velocity.Size()), bPrintToScreen, bPrintToLog,
						FLinearColor(0, 0.66, 1), DebugPrintTime);
					}

					if (DisplayBone && HitResult.BoneName.IsNone() == false)
					{
						UKismetSystemLibrary::PrintString(this,
						"Bone : " + HitResult.BoneName.ToString(), bPrintToScreen, bPrintToLog,
						FLinearColor(0, 0.66, 1), DebugPrintTime);
					}
				}
				#endif
			}
		}		
	}
}

FVector USBTraceComponent::TraceVelocity(const FVector& Start, const FVector& End) const
{
	return (bCalculateVelocity) ? (End - Start) / FMath::Max(GetWorld()->GetDeltaSeconds(), GetComponentTickInterval()) * (Steps + 1) : FVector();
}

FLinearColor USBTraceComponent::TraceColor() const
{
												// purple										 // red
	return (CurrentStep < Steps && bSubdivide) ? FLinearColor(1.f, 0.f, 0.85f, 1) : FLinearColor(1.f, 0.f, 0.f, 1);							
}

bool USBTraceComponent::CheckComponentClassInWhitelistBlackList(UClass* ComponentClass)
{	
	SolveWhitelistBlacklistCollisions();
	
	const bool bIsWhitelistEmpty =  !ComponentClassWhitelist.IsValidIndex(0);
	const bool bIsComponentWhiteListed = ComponentClassWhitelist.Contains(ComponentClass);
	const bool bIsComponentBlackListed = ComponentClassBlacklist.Contains(ComponentClass);
		
	return (bIsWhitelistEmpty || bIsComponentWhiteListed) && !bIsComponentBlackListed;
}

void USBTraceComponent::SolveWhitelistBlacklistCollisions()
{
	TArray<TSubclassOf<UObject>> ClassesToRemoveFromBlacklist;
	
	for (TSubclassOf<UObject> ComponentClass : ComponentClassWhitelist)
	{
		if (ComponentClassBlacklist.Contains(ComponentClass))
		{
			ClassesToRemoveFromBlacklist.Add(ComponentClass);
		}
	}

	for (TSubclassOf<UObject> ComponentClass : ClassesToRemoveFromBlacklist)
	{
		ComponentClassBlacklist.Remove(ComponentClass);
	}
}

bool USBTraceComponent::CheckDirection(FHitResult HitResult, FVector Velocity) const
{
	if (bCheckDirection && bCalculateVelocity)
	{
		Velocity.Normalize(0.0001);
		const float DotProduct = FVector::DotProduct(HitResult.ImpactNormal * 1.f, Velocity);
		
		// If inverse impact normal and velocity are not similarly oriented, fail check
		if (DotProduct < DirectionThreshold)
		{
			if (OnDirectionFail.IsBound()) OnDirectionFail.Broadcast(HitResult);						
			if (OnDirectionFailNative.IsBound()) OnDirectionFailNative.Broadcast(HitResult);			
			
			return false;
		}	
	}

	return true;
}

bool USBTraceComponent::IgnoreHitActor(FHitResult HitResult)
{
	if (IgnoreActors.Contains(HitResult.GetActor())) return false;
	
	IgnoreActors.AddUnique(HitResult.GetActor());

	return true;
}

bool USBTraceComponent::IsBlockingCollision(FHitResult HitResult)
{
	const bool bIsCollisionBlocked = BlockingSurfaceTypes.Contains(HitResult.PhysMaterial.Get()->SurfaceType)
	|| BlockingSurfaceTypes.Contains(HitResult.GetComponent()->GetCollisionObjectType()); 
	if (bIsCollisionBlocked)
	{
		if (OnBlockNative.IsBound()) OnBlockNative.Broadcast();
		if (OnBlock.IsBound()) OnBlock.Broadcast();

		if (bStopOnBlock)
		{
			Deactivate();		
		}
	}
	
	return bIsCollisionBlocked;
}

void USBTraceComponent::UpdateHitCounter(FHitResult HitResult)
{
	Hits++;
	
	// If hit count is equal to (or greater than) max hits, deactivate trace
	if (MaxHits > 0 && Hits >= MaxHits)
	{
		if (OnMaxHitsNative.IsBound()) OnMaxHitsNative.Broadcast(HitResult);
		if (OnMaxHits.IsBound()) OnMaxHits.Broadcast(HitResult);
		Deactivate();
	}
}

void USBTraceComponent::TransformLocationRotation(FVector Location, FRotator Rotation, FTransform LastTransform,
	FTransform CurrentTransform, FVector& LocationTransformed, FRotator& RotationTransformed) const
{
	LocationTransformed = UKismetMathLibrary::TransformLocation(LastTransform, UKismetMathLibrary::InverseTransformLocation(CurrentTransform, Location));
	RotationTransformed = UKismetMathLibrary::TransformRotation(LastTransform, UKismetMathLibrary::InverseTransformRotation(CurrentTransform, Rotation));	
}

void USBTraceComponent::UpdateComponentTransform()
{	
	ComponentTransform = GetComponentTransform();
}

int32 USBTraceComponent::CalculateSteps()
{		
	const float TimeBetweenFrames = FMath::Max(GetComponentTickInterval(), GetWorld()->GetDeltaSeconds() / UGameplayStatics::GetGlobalTimeDilation(this));
	const float Dot = FVector::DotProduct(ComponentTransform.Rotator().Vector(), GetComponentTransform().Rotator().Vector());	
	const float AngularDifference = FMath::Abs(UKismetMathLibrary::DegAcos(Dot));
	const float AngularSpeed = AngularDifference / TimeBetweenFrames;	
	const float LocationDifference = FVector::Distance(ComponentTransform.GetLocation(), GetComponentTransform().GetLocation());
	const float LinearSpeed =  (LocationDifference / TimeBetweenFrames) * InterpModifier; // Also adjust linear speed for threshold	
	CurveRatio = UKismetMathLibrary::MapRangeClamped((AngularSpeed / LinearSpeed), 0.6f, 1.f, 0.f, 1.f); 
	
	// Maps rotation difference to step values
	const float StepValue = UKismetMathLibrary::MapRangeClamped(AngularDifference, 10.f, SubstepAngleMax, 1.f, 10.f);	
	// Maps location difference to step multiplier
	const float StepMultiplier = UKismetMathLibrary::MapRangeClamped(LocationDifference, 50.f, 100.f, 1.f, 4.f);
	
	return FMath::TruncToInt(FMath::Clamp(StepValue * StepMultiplier, StepCount + 1.f, SubstepMax + 1.f)) - 1;
}
