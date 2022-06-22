// Copyright Cvtx. All Rights Reserved.

#include "Components/SBTargetingComponent.h"
#include "Logging.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SBTargetPointComponent.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Interfaces/SBTargetableInterface.h"
#include "Widgets/SBLockOnWidget.h"


USBTargetingComponent::USBTargetingComponent()
{
	TargetingType = ESBTargetingType::EMagneticLockYawOnly;
	bShouldDrawLockedOnWidget = true;
	TargetableCollisionChannel = ECollisionChannel::ECC_Pawn;
	bTargetOnlyVisibleTargets = true;
}

void USBTargetingComponent::ToggleTargeting()
{
	IsLockedOnTarget() ? StopTargeting() : StartTargeting();
}

void USBTargetingComponent::StartTargeting()
{
	if (ControlledPawn && CameraManager && !IsLockedOnTarget())
	{
		AActor* BestTarget = GetBestTarget();
		if (BestTarget)
		{
			LockOnTarget(BestTarget);
		}
	} 
}

void USBTargetingComponent::StopTargeting()
{
	if (IsLockedOnTarget())
	{
		// Interface message
		if (IsActorImplementsInterface(CurrentTarget)) { ISBTargetableInterface::Execute_OnTargetLosed(CurrentTarget); }
		
		// Remove lock on widget
		if (TargetLockedOnWidgetComponent) { TargetLockedOnWidgetComponent->DestroyComponent(); }

		// Invalidate timers
		if (CheckLockOffConditionsTimerHandle.IsValid()) { CheckLockOffConditionsTimerHandle.Invalidate(); }

		// Invalidate timers
		if (LineOfSightBreakTimerHandle.IsValid()) { LineOfSightBreakTimerHandle.Invalidate(); }

		// Broadcast events
		OnTargetLockedOff.Broadcast(CurrentTarget);
		OnTargetLockedOffDelegate.Broadcast(CurrentTarget);
	}

	CurrentTarget = nullptr;
	CurrentTargetPoint = nullptr;	
}

void USBTargetingComponent::LockOnNextTargetWithAxisInput(FSBUniversalTargetingAxisInput Input)
{
	// If we're not locked on, do nothing
	// If we're not allowed to switch target, do nothing
	// If we're already switching target, do nothing for a set amount of time
	if (!IsLockedOnTarget() || !ShouldSwitchTargetActor(Input.AxisValue) || bIsSwitchingTarget)
	{
		return;
	}
	
	if (Input.InputType == FSBUniversalTargetingAxisInputType::EGamepadInput)
	{
		// This 2.5 magical number should be a gamepad x-axis sensetivity
		Input.AxisValue *= 2.5;
	}
	
	// Depending on Axis Value negative / positive, set Direction to Look for (negative: left, positive: right)
	const float RangeMin = Input.AxisValue < 0 ? 0 : 180;
	const float RangeMax = Input.AxisValue < 0 ? 180 : 360;

	// Reset Closest Target Distance to Minimum Distance to Enable
	ClosestTargetDistance = MaxTargetingDistance;
	
	TArray<AActor*> PotentialTargets;
	FindPotentialTargets(PotentialTargets);

	// For each of these actors, check line trace and ignore Current Target and build the list of actors to look from
	TArray<AActor*> ActorsToLook;

	const TArray<AActor*> ActorsToIgnore { CurrentTarget };
	for (AActor* Target : PotentialTargets)
	{
		if (IsActorInSight(Target))
		{
			ActorsToLook.Add(Target);
		}
		
		continue;
		
		const bool bHit = LineTraceForActor(Target, ActorsToIgnore);
		if (bHit && IsActorInViewport(Target))
		{
			ActorsToLook.Add(Target);
		}
	}

	// Find Targets in Range (left or right, based on Character and CurrentTarget)
	TArray<AActor*> TargetsInRange = FindTargetsInRange(ActorsToLook, RangeMin, RangeMax);

	// For each of these targets in range, get the closest one to current target
	AActor* ActorToTarget = nullptr;
	for (AActor* Actor : TargetsInRange)
	{
		// and filter out any character too distant from minimum distance to enable
		const float Distance = GetDistanceFromPawn(Actor);
		if (Distance < MaxTargetingDistance)
		{
			const float RelativeActorsDistance = CurrentTarget->GetDistanceTo(Actor);
			if (RelativeActorsDistance < ClosestTargetDistance)
			{
				ClosestTargetDistance = RelativeActorsDistance;
				ActorToTarget = Actor;
			}
		}
	}

	if (ActorToTarget)
	{
		SwitchingTargetTimerHandle.IsValid() ? SwitchingTargetTimerHandle.Invalidate() : true;
		LockOnTarget(ActorToTarget);
		
		GetWorld()->GetTimerManager().SetTimer(
			SwitchingTargetTimerHandle,
			this,
			&USBTargetingComponent::ResetIsSwitchingTarget,
			bIsSwitchingTarget ? 0.25f : 0.5f // Less sticky if still switching
		);

		bIsSwitchingTarget = true;
	}	
}

bool USBTargetingComponent::IsLockedOnTarget() const
{
	return IsValid(CurrentTarget);
}

AActor* USBTargetingComponent::GetBestTargetRightNow(ESBTargetSelectionType TargetSelection)
{
	// TODO This is a dublicate method, refactor
	AActor* BestTargetCandidate = nullptr;
	TArray<AActor*> PotentialTargets;
	FindPotentialTargets(PotentialTargets);

	FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation());
	CameraForwardVector.Z = 0;
	CameraForwardVector = CameraForwardVector.GetSafeNormal();
	float MaxDistance = BIG_NUMBER;
	float MaxDirection = -1.0f;

	const float InDistance = 2000;
	
	for (AActor* Target : PotentialTargets)
	{
		if (Target && IsValidTarget(Target) && Target->WasRecentlyRendered())
		{
			const float Distance = Target->GetDistanceTo(ControlledPawn);
			switch (TargetSelection)
			{
				case ESBTargetSelectionType::ENearestTarget:
					if (Distance < MaxDistance && Distance <= InDistance)
					{
						BestTargetCandidate = Target;
						MaxDistance = Distance;
					}
				break;
				
				case ESBTargetSelectionType::EForwardTarget:
					const FVector VDistance = (Target->GetActorLocation() - ControlledPawn->GetActorLocation()).GetUnsafeNormal();
				const float Direction = FVector::DotProduct(CameraForwardVector, VDistance);
				
				if (Direction > MaxDirection)
				{
					BestTargetCandidate = Target;
					MaxDirection = Direction;
				}
				break;
			}
		}
	}

	return BestTargetCandidate;
}

bool USBTargetingComponent::IsActorInViewport(const AActor* Actor) const
{
	if (IsValid(ControlledPawn->GetController()) == false)
	{
		return true;
	}

	FVector2D ScreenLocation;
	APlayerController* PC = Cast<APlayerController>(ControlledPawn->GetController());
	PC->ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenLocation);

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	return ScreenLocation.X > 0 && ScreenLocation.Y > 0 && ScreenLocation.X < ViewportSize.X && ScreenLocation.Y < ViewportSize.Y;	
}

float USBTargetingComponent::GetAngleUsingCameraRotation(const AActor* ActorToLook) const
{
	UCameraComponent* CameraComponent = ControlledPawn->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		// Fallback to CharacterRotation if no CameraComponent can be found
		return GetAngleUsingCharacterRotation(ActorToLook);
	}

	const FRotator CameraWorldRotation = CameraComponent->GetComponentRotation();
	const FRotator LookAtRotation = FindLookAtRotation(CameraComponent->GetComponentLocation(), ActorToLook->GetActorLocation());

	float YawAngle = CameraWorldRotation.Yaw - LookAtRotation.Yaw;
	if (YawAngle < 0)
	{
		YawAngle = YawAngle + 360;
	}

	return YawAngle;	
}

float USBTargetingComponent::GetAngleUsingCharacterRotation(const AActor* ActorToLook) const
{
	const FRotator CharacterRotation = ControlledPawn->GetActorRotation();
	const FRotator LookAtRotation = FindLookAtRotation(ControlledPawn->GetActorLocation(), ActorToLook->GetActorLocation());

	float YawAngle = CharacterRotation.Yaw - LookAtRotation.Yaw;
	if (YawAngle < 0)
	{
		YawAngle = YawAngle + 360;
	}

	return YawAngle;
}

FRotator USBTargetingComponent::FindLookAtRotation(const FVector Start, const FVector Target)
{
	return FRotationMatrix::MakeFromX(Target - Start).Rotator();
}

bool USBTargetingComponent::TrySwitchPointOnCurrentTarget(ESBTargetingDirection direction)
{
	USBTargetPointComponent* bestpoint = nullptr;
	TArray< USBTargetPointComponent*> potentialTargets;
	if (CurrentTarget && CurrentTargetPoint)
	{
		TArray<UActorComponent*> points;
		CurrentTarget->GetComponents(USBTargetPointComponent::StaticClass(), points, true);
		if (points.Num() > 1)
		{
			for (auto point : points)
			{
				USBTargetPointComponent* targetpoint = Cast<USBTargetPointComponent>(point);
				if (targetpoint && targetpoint != CurrentTargetPoint)
				{
					FVector PotentialTargetLocation = targetpoint->GetComponentLocation();

					switch (direction)
					{
						case ESBTargetingDirection::ERight:
							if (IsRightOfCurrentTarget(PotentialTargetLocation))
							{
								potentialTargets.Add(targetpoint);
							}
						break;
						case ESBTargetingDirection::ELeft:
							if (!IsRightOfCurrentTarget(PotentialTargetLocation))
							{
								potentialTargets.Add(targetpoint);
							}
						break;
						case ESBTargetingDirection::EDown:
							if (!IsUpOfCurrentTarget(PotentialTargetLocation))
							{
								potentialTargets.Add(targetpoint);
							}
						break;
						case ESBTargetingDirection::EUp:
							if (IsUpOfCurrentTarget(PotentialTargetLocation))
							{
								potentialTargets.Add(targetpoint);
							}
						break;
						default:
							break;
					}
				}
							
			}
			
		}
	
	}

	if (potentialTargets.Num() == 0)
	{
		return false;
	}
	else
	{
		USBTargetPointComponent* finaltarget = GetNearestTargetPoint(potentialTargets);
		UpdateCurrentTargetPoint(finaltarget);
	}

	return false;	
}

bool USBTargetingComponent::IsRightOfCurrentTarget(FVector LocationToCheck)
{
	FVector CurrentTargetLocation = GetCurrentTargetPointLocation();
	FVector2D currentTargetLocationOnScreen;
	FVector2D potentialTargetLocationOnScreen;

	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

	if (playerController)
	{
		playerController->ProjectWorldLocationToScreen(CurrentTargetLocation, currentTargetLocationOnScreen);
		playerController->ProjectWorldLocationToScreen(LocationToCheck, potentialTargetLocationOnScreen);

		return (potentialTargetLocationOnScreen.X - currentTargetLocationOnScreen.X) >= 0;
	}
	return false;	
}

bool USBTargetingComponent::IsUpOfCurrentTarget(FVector LocationToCheck)
{
	FVector CurrentTargetLocation = GetCurrentTargetPointLocation();
	FVector2D currentTargetLocationOnScreen;
	FVector2D potentialTargetLocationOnScreen;

	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

	if (playerController)
	{
		playerController->ProjectWorldLocationToScreen(CurrentTargetLocation, currentTargetLocationOnScreen);
		playerController->ProjectWorldLocationToScreen(LocationToCheck, potentialTargetLocationOnScreen);

		return (potentialTargetLocationOnScreen.Y - currentTargetLocationOnScreen.Y) >= 0;
	}
	return false;	
}

FVector USBTargetingComponent::GetCurrentTargetPointLocation() const
{
	if (CurrentTargetPoint)
	{
		return CurrentTargetPoint->GetComponentLocation();
	}
	else if(CurrentTarget)
	{
		return CurrentTarget->GetActorLocation();
	}

	return FVector();	
}

USBTargetPointComponent* USBTargetingComponent::GetNearestTargetPoint(const TArray<USBTargetPointComponent*>& points)
{
	USBTargetPointComponent* localTarget = nullptr;
	if (points.Num() > 0)
	{
		float distanceFromCurrentTarget = 999999.f;

		for (int32 i = 0; i < points.Num(); ++i)
		{
			if (!points.IsValidIndex(i))
			{
				continue;
			}
			
			float distance = FVector::Distance(points[i]->GetComponentLocation(), CurrentTargetPoint->GetComponentLocation());
			if (distance < distanceFromCurrentTarget)
			{
				distanceFromCurrentTarget = distance;
				localTarget = points[i];
			}
		}
	}
	return localTarget;	
}

TArray<AActor*> USBTargetingComponent::GetAllTargetsByDirection(ESBTargetingDirection direction)
{
	TArray<AActor*> Targets;
	FindPotentialTargets(Targets);
	
	TArray<AActor*> ActorFilter;
	for (AActor* Target : Targets)
	{
		if (Target != CurrentTarget && IsValidTarget(Target))
		{
			FVector PotentialTargetLocation = Target->GetActorLocation();

			if (!IsInFrontOfOwner(Target))
			{
				continue;
			}
			switch (direction)
			{
				case ESBTargetingDirection::ERight:
					if (IsRightOfCurrentTarget(PotentialTargetLocation))
					{
						ActorFilter.AddUnique(Target);
					}
				break;
				case ESBTargetingDirection::ELeft:
					if (!IsRightOfCurrentTarget(PotentialTargetLocation))
					{
						ActorFilter.AddUnique(Target);
					}
				break;
				case ESBTargetingDirection::EDown:
					if (!IsUpOfCurrentTarget(PotentialTargetLocation))
					{
						ActorFilter.AddUnique(Target);
					}
				break;
				case ESBTargetingDirection::EUp:
					if (IsUpOfCurrentTarget(PotentialTargetLocation))
					{
						ActorFilter.AddUnique(Target);
					}
				break;
				default:
					break;
			}
		}
	}

	return ActorFilter;	
}

bool USBTargetingComponent::IsInFrontOfOwner(AActor* target)
{
	
	FVector forward = CameraManager->GetCameraRotation().Vector();
	FVector distance = target->GetActorLocation() - ControlledPawn->GetActorLocation();
	distance.Normalize();
	float AimAtAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(forward, distance)));
	if (AimAtAngle <  MaxAngularDistanceDegree)
	{
		return true;
	}
	return false;	
}

AActor* USBTargetingComponent::GetNearestTarget(TArray<AActor*>& actorFilter)
{
	AActor* localTarget = nullptr;
	if (actorFilter.Num() > 0)
	{		
		float distanceFromCurrentTarget = 999999.f;

		for (int32 i = 0; i < actorFilter.Num(); ++i)
		{
			if (!actorFilter[i])
			{
				continue;
			}
			float distance = actorFilter[i]->GetDistanceTo(ControlledPawn);
			if (distance < distanceFromCurrentTarget)
			{
				distanceFromCurrentTarget = distance;
				localTarget = actorFilter[i];
			}
		}
	}
	return localTarget;	
}

void USBTargetingComponent::LockOnTarget(AActor* Target)
{
	StopTargeting();
	
	if (Target)
	{
		CurrentTarget = Target;
		UpdateCurrentTargetPoint(GetBestTargetPointForTarget(CurrentTarget));

		if (CurrentTargetPoint)
		{
			CreateLockedOnWidgetOnTargetPoint();
		}
		else
		{
			CreateLockedOnWidgetOnTarget();
		}

		GetWorld()->GetTimerManager().SetTimer(
			CheckLockOffConditionsTimerHandle,
			this,
			&USBTargetingComponent::CheckLockOffConditions,
			CheckLockOffConditionsRate,
			true
		);
		
		OnTargetLockedOn.Broadcast(Target);
		OnTargetLockedOnDelegate.Broadcast(Target);
	}
}

void USBTargetingComponent::SwitchTargetByDirection(ESBTargetingDirection SwitchDirection)
{
	if (TrySwitchPointOnCurrentTarget(SwitchDirection))
	{
		return ;
	}

	TArray<AActor*> Targets;
	FindPotentialTargets(Targets);
	TArray<AActor*> actorFilter = GetAllTargetsByDirection(SwitchDirection);

	AActor* localCurrentTarget = GetNearestTarget(actorFilter);

	if (localCurrentTarget && localCurrentTarget != CurrentTarget)
	{
		LockOnTarget(localCurrentTarget);

		return ;
	}	
}

void USBTargetingComponent::AddFilter(TSubclassOf<USBTargetingFilter> FilterClass)
{
	USBTargetingFilter* NewFilter = NewObject<USBTargetingFilter>(FilterClass);
	TargetFilters.AddUnique(NewFilter);
}

bool USBTargetingComponent::RemoveFilter(TSubclassOf<USBTargetingFilter> FilterClass)
{
	int32 Index = -1;
	for (USBTargetingFilter* Filter : TargetFilters)
	{
		if (Filter->IsA(FilterClass))
		{
			Index = TargetFilters.IndexOfByKey(Filter);
			break;
		}

	}
	if (TargetFilters.IsValidIndex(Index))
	{
		TargetFilters.RemoveAt(Index);
		return true;
	}
	return false;	
}

void USBTargetingComponent::AddObjectType(TEnumAsByte<EObjectTypeQuery> InTraceChannel)
{
	if (!ObjectsToQuery.Contains(InTraceChannel))
	{
		ObjectsToQuery.Add(InTraceChannel);
	}
}

void USBTargetingComponent::RemoveObjectType(TEnumAsByte<EObjectTypeQuery> InTraceChannel)
{
	if (ObjectsToQuery.Contains(InTraceChannel))
	{
		ObjectsToQuery.Remove(InTraceChannel);
	}	
}

ESBLockType USBTargetingComponent::GetLockTypeByTargetingType(ESBTargetingType TargetType)
{
	switch (TargetType)
	{
		case ESBTargetingType::EDontLock:
			break;
		case ESBTargetingType::EMagneticLock:
			return ESBLockType::EAllAxis;
			break;
		case ESBTargetingType::EMagneticLockYawOnly:
			return ESBLockType::EYawOnly;
			break;
		default:
			break;
	}
	
	return ESBLockType::EAllAxis;	
}

void USBTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	SetOwnerReferences();
}

TArray<AActor*> USBTargetingComponent::FindTargetsInRange(TArray<AActor*>& PotentialTargets, float RangeMax, float RangeMin) const
{
	TArray<AActor*> ActorsInRange;

	for (AActor* Actor : PotentialTargets)
	{
		const float Angle = GetAngleUsingCameraRotation(Actor);
		if (Angle > RangeMin && Angle < RangeMax)
		{
			ActorsInRange.Add(Actor);
		}
	}

	return ActorsInRange;	
}

void USBTargetingComponent::FindPotentialTargets(TArray<AActor*>& OutTargets) const
{
	if (ControlledPawn)
	{
		OutTargets.Empty();
		const TArray<AActor*> IgnoredActors { ControlledPawn };
		UKismetSystemLibrary::SphereOverlapActors
		(
			this,
			ControlledPawn->GetActorLocation(),
			MaxTargetingDistance,
			ObjectsToQuery,
			AActor::StaticClass(),
			IgnoredActors,
			OutTargets
		);
	}
}

bool USBTargetingComponent::LineTrace(FHitResult& HitResult, AActor* OtherActor, TArray<AActor*> ActorsToIgnore) const
{
	FCollisionQueryParams Params = FCollisionQueryParams(FName("LineTraceSingle"));

	TArray<AActor*> IgnoredActors {ControlledPawn};
	IgnoredActors += ActorsToIgnore;

	Params.AddIgnoredActors(IgnoredActors);

	return GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ControlledPawn->GetActorLocation(),
		OtherActor->GetActorLocation(),
		TargetableCollisionChannel,
		Params
	);	
}

bool USBTargetingComponent::LineTraceForActor(AActor* OtherActor, TArray<AActor*> ActorsToIgnore) const
{
	FHitResult HitResult;
	const bool bHit = LineTrace(HitResult, OtherActor, ActorsToIgnore);
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor == OtherActor)
		{
			return true;
		}
	}

	return false;	
}

void USBTargetingComponent::BreakLineOfSight()
{
	if (ShouldBreakLineOfSight())
	{
		StopTargeting();
	}
	
	bIsBreakingLineOfSight = false;
}

AActor* USBTargetingComponent::GetBestTarget()
{
	AActor* BestTargetCandidate = nullptr;
	TArray<AActor*> PotentialTargets;
	FindPotentialTargets(PotentialTargets);

	FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation());
	CameraForwardVector.Z = 0;
	CameraForwardVector = CameraForwardVector.GetSafeNormal();
	float MaxDistance = BIG_NUMBER;
	float MaxDirection = -1.0f;
	
	for (AActor* Target : PotentialTargets)
	{
		if (Target && IsValidTarget(Target) && Target->WasRecentlyRendered())
		{
			const float Distance = Target->GetDistanceTo(ControlledPawn);
			switch (TargetSelectionType)
			{
				case ESBTargetSelectionType::ENearestTarget:
					if (Distance < MaxDistance)
					{
						BestTargetCandidate = Target;
						MaxDistance = Distance;
					}
				break;
				
				case ESBTargetSelectionType::EForwardTarget:
					const FVector VDistance = (Target->GetActorLocation() - ControlledPawn->GetActorLocation()).GetUnsafeNormal();
					const float Direction = FVector::DotProduct(CameraForwardVector, VDistance);
				
					if (Direction > MaxDirection)
					{
						BestTargetCandidate = Target;
						MaxDirection = Direction;
					}
				break;
			}
		}
	}

	return BestTargetCandidate;
}

float USBTargetingComponent::GetDistanceToActorFromPawn(AActor* Actor) const
{
	return ControlledPawn->GetDistanceTo(Actor);
}

void USBTargetingComponent::CreateLockedOnWidgetOnTarget()
{
	if (!LockedOnWidgetClass)
	{
		LOG_ERROR(SoulboundTargeting, "Can't get LockedOnWidgetClass, please ensure it is a valid reference in the Component Properties.");
		return;
	}

	if (CurrentTarget == nullptr)
	{
		LOG_WARNING(SoulboundTargeting, "Can't get draw locked on widget when there is no target.")
		return;
	}

	TargetLockedOnWidgetComponent = NewObject<UWidgetComponent>(CurrentTarget, MakeUniqueObjectName(CurrentTarget, UWidgetComponent::StaticClass(), FName("LockOnTarget")));
	TargetLockedOnWidgetComponent->SetWidgetClass(LockedOnWidgetClass);
	
	UMeshComponent* MeshComponent = CurrentTarget->FindComponentByClass<UMeshComponent>();
	USceneComponent* ParentComponent = (MeshComponent && LockedOnWidgetParentSocket != NAME_None)
										   ? MeshComponent
										   : CurrentTarget->GetRootComponent(); // TargetComponent->GetAttachmentRoot(); 

	TargetLockedOnWidgetComponent->ComponentTags.Add(FName("TargetSystem.LockOnWidget"));
	TargetLockedOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TargetLockedOnWidgetComponent->SetupAttachment(ParentComponent, LockedOnWidgetParentSocket);
	TargetLockedOnWidgetComponent->SetRelativeLocation(LockedOnWidgetRelativeLocation);
	TargetLockedOnWidgetComponent->SetDrawSize(FVector2D(LockedOnWidgetDrawSize, LockedOnWidgetDrawSize));
	TargetLockedOnWidgetComponent->SetVisibility(true);
	TargetLockedOnWidgetComponent->RegisterComponent();
	
	// TODO Remake
	USBLockOnWidget* Widget = Cast<USBLockOnWidget>(TargetLockedOnWidgetComponent->GetWidget());
	if (Widget)
	{
		Widget->SetTarget(CurrentTarget);
		if (Widget->GetOuter() == nullptr)
		{
			LOG_SCREENMSG("Outer is null");
		}
		
	} else
	{
		LOG_SCREENMSG("Can't cast widget");
	}
}

void USBTargetingComponent::CreateLockedOnWidgetOnTargetPoint()
{
	// TODO Delete duplicate code
	
	if (!LockedOnWidgetClass)
	{
		LOG_ERROR(SoulboundTargeting, "Can't get LockedOnWidgetClass, please ensure it is a valid reference in the Component Properties.");
		return;
	}

	if (CurrentTarget == nullptr)
	{
		LOG_WARNING(SoulboundTargeting, "Can't get draw locked on widget when there is no target.")
		return;
	}
	
	//UObject* WidgetOuter = Cast<UObject>(CurrentTargetPoint ? CurrentTargetPoint : CurrentTarget);
	TargetLockedOnWidgetComponent = NewObject<UWidgetComponent>(CurrentTargetPoint, MakeUniqueObjectName(CurrentTarget, UWidgetComponent::StaticClass(), FName("LockOnTarget")));
	TargetLockedOnWidgetComponent->SetWidgetClass(LockedOnWidgetClass);
	
	UMeshComponent* MeshComponent = CurrentTarget->FindComponentByClass<UMeshComponent>();
	USceneComponent* ParentComponent = MeshComponent; // CurrentTargetPoint->GetAttachmentRoot(); 

	TargetLockedOnWidgetComponent->ComponentTags.Add(FName("TargetSystem.LockOnWidget"));
	TargetLockedOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TargetLockedOnWidgetComponent->SetupAttachment(ParentComponent, LockedOnWidgetParentSocket);
	TargetLockedOnWidgetComponent->SetRelativeLocation(LockedOnWidgetRelativeLocation);
	TargetLockedOnWidgetComponent->SetDrawSize(FVector2D(LockedOnWidgetDrawSize, LockedOnWidgetDrawSize));
	TargetLockedOnWidgetComponent->SetVisibility(true);
	TargetLockedOnWidgetComponent->RegisterComponent();		
}

void USBTargetingComponent::CheckLockOffConditions()
{
	if (!IsValidTarget(CurrentTarget))
	{
		StopTargeting();
	}
	
	// Condition based on max targeting distance
	if (GetDistanceToActorFromPawn(CurrentTarget) > MaxTargetingDistance)
	{
		StopTargeting();
	}

	// Condition based on line of sight
	if (!bIsBreakingLineOfSight && ShouldBreakLineOfSight())
	{
		if (BreakLineOfSightDelay <= 0)
		{
			StopTargeting();
		}
		else
		{
			bIsBreakingLineOfSight = true;
			GetWorld()->GetTimerManager().SetTimer(
				LineOfSightBreakTimerHandle,
				this,
				&USBTargetingComponent::BreakLineOfSight,
				BreakLineOfSightDelay
			);
		}
	}
}

void USBTargetingComponent::ResetIsSwitchingTarget()
{
	bIsSwitchingTarget = false;
	bDesireToSwitch = false;	
}

bool USBTargetingComponent::ShouldSwitchTargetActor(float AxisValue)
{
	// Sticky feeling computation
	if (bEnableStickyTarget)
	{
		StartRotatingStack += (AxisValue != 0) ? AxisValue * AxisMultiplier : (StartRotatingStack > 0 ? -AxisMultiplier : AxisMultiplier);

		if (AxisValue == 0 && FMath::Abs(StartRotatingStack) <= AxisMultiplier)
		{
			StartRotatingStack = 0.0f;
		}
		
		// Sticky when switching target.
		if (StartRotatingStack * AxisValue > 0)
		{
			StartRotatingStack = StartRotatingStack > 0 ? StickyRotationThreshold : -StickyRotationThreshold;
		}
		else if (StartRotatingStack * AxisValue < 0)
		{
			StartRotatingStack = StartRotatingStack * -1.0f;
		}

		// If Axis value does not exceeds configured threshold, do nothing
		return bDesireToSwitch = (FMath::Abs(StartRotatingStack) < StickyRotationThreshold) ? false : true;
		
		if (FMath::Abs(StartRotatingStack) < StickyRotationThreshold)
		{
			bDesireToSwitch = false;
			return false;
		} else
		{
			bDesireToSwitch = true;
		}
		
		return true;
	}

	// Non Sticky feeling, check Axis value exceeds threshold
	return FMath::Abs(AxisValue) > StartRotatingThreshold;	
}

bool USBTargetingComponent::ShouldBreakLineOfSight() const
{
	return !IsTargetInSight();
}

void USBTargetingComponent::SetOwnerReferences()
{
	//APlayerController* Controller = Cast<APlayerController>();

	
	ControlledPawn = UGameplayStatics::GetPlayerPawn(this, 0); // TODO: Remake

	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	FPawnChangedSignature EventNewPawn = Controller->GetOnNewPawnNotifier();
	EventNewPawn.AddLambda([this](APawn* newPawn) {
		HandlePawnChanged(newPawn);
		}
	);

	CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	CurrentTarget = nullptr;
}

void USBTargetingComponent::HandlePawnChanged(APawn* NewPawn)
{
	ControlledPawn = NewPawn;
}

bool USBTargetingComponent::IsValidTarget(AActor* Target)
{
	// Nullptr check
	if (Target == nullptr)
	{
		return  false;
	}

	// Can't target our controlled pawn
	if (ControlledPawn && Target == ControlledPawn)
	{
		return false;
	}

	// Check targeting interface implementation
	if (!IsActorImplementsInterface(Target) || !ISBTargetableInterface::Execute_IsTargetable(Target, nullptr))
	{
		return false;
	}
	
	// Check targeting filters
	for (const USBTargetingFilter* Filter : TargetFilters)
	{
		if (Filter && !Filter->IsActorTargetable(GetOwner(), Target))
		{
			return false;
		}
	}

	// Check if target is visible
	if (bTargetOnlyVisibleTargets && !IsActorInSight(Target))
	{
		return false;
	}
	
	return true;	
}

void USBTargetingComponent::UpdateCurrentTargetPoint(USBTargetPointComponent* InCurrentTargetPoint)
{
	const bool bImplements = IsActorImplementsInterface(CurrentTarget);
	if (bImplements == false)
	{
		LOG_ERROR(SoulboundTargeting, "Current Target doesn't implement targeting interface.");
		return;
	}
	
	if (CurrentTargetPoint)
	{
		if (InCurrentTargetPoint->GetOwner() != CurrentTarget)
		{
			CurrentTargetPoint = nullptr;
			ISBTargetableInterface::Execute_OnTargetAcquired(CurrentTarget, CurrentTargetPoint);
			LOG_ERROR(SoulboundTargeting, "Current Target don't own passed Target Point.");
			return;
		}
		else
		{
			CurrentTargetPoint = InCurrentTargetPoint;
			ISBTargetableInterface::Execute_OnTargetAcquired(CurrentTarget, CurrentTargetPoint);	
		}
		
		CurrentCameraEvent = CurrentTargetPoint->GetPointCameraEvent();
	}
	else
	{
		LOG_WARNING(SoulboundTargeting, "Passed Target Point is null. Possibly Target don't have Target Points at all.");
	}
}

bool USBTargetingComponent::IsActorImplementsInterface(AActor* Actor)
{
	return Actor ? Actor->GetClass()->ImplementsInterface(USBTargetableInterface::StaticClass()) : false;
}

float USBTargetingComponent::GetDistanceFromPawn(const AActor* OtherActor) const
{
	return ControlledPawn->GetDistanceTo(OtherActor);
}

USBTargetPointComponent* USBTargetingComponent::GetBestTargetPointForTarget(AActor* Target) const
{
	if (Target == nullptr)
	{
		LOG_ERROR(SoulboundTargeting, "Passed Target is null.")
		return nullptr;
	}

	USBTargetPointComponent* BestPoint = nullptr;
	float MaxDirection = -1.f;
	
	TArray<UActorComponent*> Points;
	Target->GetComponents(USBTargetPointComponent::StaticClass(), Points, true);
		
	FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation());
	CameraForwardVector.Z = 0;
	CameraForwardVector = CameraForwardVector.GetSafeNormal();
		
	for (UActorComponent* Point : Points)
	{
		USBTargetPointComponent* TargetPoint = Cast<USBTargetPointComponent>(Point);
		if (TargetPoint)
		{
			const FVector Distance = (TargetPoint->GetComponentLocation() - ControlledPawn->GetActorLocation()).GetUnsafeNormal();;
			const float Direction = FVector::DotProduct(CameraForwardVector, Distance);
			if (Direction > MaxDirection)
			{
				BestPoint = TargetPoint;
				MaxDirection = Direction;
			}
		}
	}
	
	return BestPoint;	
}

