// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Debug/SBDebug.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SBTraceComponent.generated.h"

class UClass;
class USceneComponent;

UENUM()
enum ESBTraceMethod
{
	Colliders, // Use child box and sphere colliders for traces
	Sockets, // Use sockets for traces, add socket names to the SocketName array or set AutoDetectSockets? to true
};


DECLARE_EVENT_FourParams(USBTraceComponent, FHitEvent, FHitResult, FVector, bool, USceneComponent*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHitDelegate, FHitResult, HitResult, FVector, Velocity, bool, bBlocked, USceneComponent*, Collider);

DECLARE_EVENT_OneParam(USBTraceComponent, FDirectionFailEvent, FHitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDirectionFailDelegate, FHitResult, HitResult);

DECLARE_EVENT(USBTraceComponent, FBlockEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlockDelegate);

DECLARE_EVENT_OneParam(USBTraceComponent, FMaxHitsEvent, FHitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMaxHitsDelegate, FHitResult, HitResult);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULBOUNDTRACE_API USBTraceComponent : public USceneComponent
{
	GENERATED_BODY()

public:		
	USBTraceComponent();

	/** Called when trace hit something */
	UPROPERTY(BlueprintAssignable)
	FHitDelegate OnHit;
	
	/** Native version of above. Called before blueprint version. */	
	FHitEvent OnHitNative;

	/** Called on direction fail */
	UPROPERTY(BlueprintAssignable)
	FDirectionFailDelegate OnDirectionFail;
	
	/** Native version of above. Called before blueprint version. */
	FDirectionFailEvent OnDirectionFailNative;

	/** Called when trace is blocked */
	UPROPERTY(BlueprintAssignable)
	FBlockDelegate OnBlock;

	/** Native version of above. Called before blueprint version. */
	FBlockEvent OnBlockNative;

	/** Called when hit count gets to max count */
	UPROPERTY(BlueprintAssignable)
	FMaxHitsDelegate OnMaxHits;
		
	/** Native version of above. Called before blueprint version. */
	FMaxHitsEvent OnMaxHitsNative;

	UFUNCTION(BlueprintCallable)
	void Enable(UActorComponent* Component, bool bReset);

	UFUNCTION(BlueprintCallable)
	void Disable(UActorComponent* Component);

	void Reset();

protected:

	///
	/// Trace settings
	///

	/** Method used for traces, Colliders (box and sphere compatible) or Sockets */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	TEnumAsByte<ESBTraceMethod> TraceMethod;

	/** Channel to trace against */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	/** Collision types that will trigger a block */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	TArray<TEnumAsByte<ECollisionChannel>> BlockingCollisionTypes;
	
	/** Physical surface types that will trigger a block */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	TArray<TEnumAsByte<EPhysicalSurface>> BlockingSurfaceTypes;

	/** If true, stops all traces on a blocking hit */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	uint8 bStopOnBlock : 1;

	/** Component classes to register hits, excludes all else */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	TArray<TSubclassOf<UObject>> ComponentClassWhitelist;

	/** Component classes to ignore */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	TArray<TSubclassOf<UObject>> ComponentClassBlacklist;

	/** If true, checks swing direction against hits and ignores hits in the opposite direction */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	uint8 bCheckDirection : 1;

	/** Direction threshold from -1 (opposite direction) to 1 (matching direction) under which direction checks will fail */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings", meta = (ClampMin = "-1", ClampMax = "1", UIMin = "-1", UIMax = "1"))
	float DirectionThreshold;

	/** */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	int32 MaxHits;

	/** */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	uint8 bCalculateVelocity : 1;

	/** Component tick interval */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Trace Settings")
	float TickRate;	

	///
	/// Substep settings
	///

	/** Number of steps to use in substepping */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Substep Settings", meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	int32 StepCount;

	/** If true, automatically substeps based on transform differences per frame (recommended) */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Substep Settings")
	uint8 bAutoStep : 1;

	/** Max steps for autostepping */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Substep Settings", meta = (ClampMin = "1", ClampMax = "10", UIMin = "1", UIMax = "10"))
	int32 SubstepMax;

	/** */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Substep Settings", meta = (ClampMin = "1", ClampMax = "3", UIMin = "1", UIMax = "3"))
	float SubstepPadding;

	/** The angle difference at which the max number of substeps will be used */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Substep Settings", meta = (ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200"))
	float SubstepAngleMax;

	/** Affects the curvature calculation from 0 (max curve) to 2 (no curve) when choosing between the substep interpolation methods Quat or Dual Quat (better with extreme arcs) */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Substep Settings", meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	float InterpModifier;
	
	///
	/// Debug settings
	///
	
	#if WITH_EDITORONLY_DATA && SOULBOUND_DEBUG

	/** Sets the debug draw type */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DebugType;

	/** Debug draw time */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	float DebugDrawTime;

	/** Debug print time */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	float DebugPrintTime;

	/** If true, debug print is enabled */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 bPrint : 1;

	/** If true, debug print is enabled */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 bPrintToScreen : 1;

	/** If true, debug print is enabled */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 bPrintToLog : 1;

	/** If true, prints hit actor name */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 DisplayActor : 1;

	/** If true, prints hit component name */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 DisplayComponent : 1;

	/** If true, prints hit collision type */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 DisplayCollisionType : 1;

	/** If true, prints speed of hit */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 DisplaySpeed : 1;

	/** If true, prints hit bone (if valid) */
	UPROPERTY(EditDefaultsOnly, Category="Soulbound|Debug")
	uint8 DisplayBone : 1;
	
	#endif	
	
protected:
	virtual void BeginPlay() override;
	
public:		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void SocketTrace();
	void SocketSubstepTrace();
	void LineTrace(const FVector Start, const FVector End);

	void ColliderTrace();
	void ColliderSubstepTrace();
	
	void BoxTrace(USceneComponent* Collider, FVector Start, FVector End, FVector HalfSize, FRotator Orientation);
	void SphereTrace(USceneComponent* Collider, FVector Start, FVector End, float Radius);
	
	void GetColliders();	
	void UpdateColliderTransforms();
	
	void GetSockets();
	void UpdateSocketTransforms();
	
	void SubdivideTrace(int32 Index, FTransform Transform, float Padding, FVector& Start, FVector& End, FRotator& Orientation) const;

	void HitCheck(USceneComponent* Collider, const TArray<FHitResult> HitResults, FVector Velocity);

	FVector TraceVelocity(const FVector &Start, const FVector &End) const;
	
	FLinearColor TraceColor() const;
	
	UPROPERTY()
	TArray<USceneComponent*> Colliders;	
	TArray<FTransform> CollidersTransforms;
	
	TArray<FName> SocketNames;
	TArray<FTransform> SocketTransforms;

	UPROPERTY()
	TArray<AActor*> IgnoreActors;		

	UPROPERTY()
	USceneComponent* ParentComponent;
	
	uint8 bIsEnabled : 1;
	uint8 bSubdivide : 1;
	float TickInterval;
	float CurveRatio;
	FTransform ComponentTransform;
	int32 Hits;		
	
	UPROPERTY(meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	int32 Steps;

	int32 CurrentStep;
	
	bool CheckComponentClassInWhitelistBlackList(UClass* ComponentClass);
	void SolveWhitelistBlacklistCollisions();
	bool CheckDirection(FHitResult HitResult, FVector Velocity) const;
	bool IgnoreHitActor(FHitResult HitResult);
	bool IsBlockingCollision(FHitResult HitResult);
	void UpdateHitCounter(FHitResult HitResult);
	void UpdateComponentTransform();
	int32 CalculateSteps();
	
	void TransformLocationRotation(FVector Location, FRotator Rotation, FTransform LastTransform,
	FTransform CurrentTransform, FVector& LocationTransformed, FRotator& RotationTransformed) const;
	
};
