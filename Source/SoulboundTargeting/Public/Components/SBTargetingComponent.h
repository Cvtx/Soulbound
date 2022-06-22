// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SBTargetingFilter.h"
#include "Components/SphereComponent.h"
#include "SBTargetingComponentBase.h"
 #include "SBCameraData.h"
#include "SBTargetingComponent.generated.h"

class APawn;
class USBTargetComponent;

UENUM(BlueprintType)
enum class ESBTargetingType : uint8
{
	// Dont Lock = does not affect camera
	EDontLock = 0			UMETA(DisplayName = "Don't Lock Camera"),
	// Magnetic Lock = smooth camera lock on that interpolates thought the target
	EMagneticLock			UMETA(DisplayName = "Adjust Camera Lock with Magnetism"),
	// MagneticYawOnly = Affects only yaw for the camera
	EMagneticLockYawOnly	UMETA(DisplayName = "Adjust Camera Yaw Lock On Target"),
};

UENUM(BlueprintType)
enum class ESBTargetingDirection : uint8
{
	ELeft = 0       UMETA(DisplayName = "Left"),
	ERight			UMETA(DisplayName = "Right"),
	EUp 		    UMETA(DisplayName = "Up"),
	EDown           UMETA(DisplayName = "Down"),
};

UENUM(BlueprintType)
enum class ESBTargetSelectionType : uint8
{
	ENearestTarget = 0       UMETA(DisplayName = "Choose Lower Distance"),
	EForwardTarget = 1		 UMETA(DisplayName = "Choose Lower Degrees From Forward"),
};

UENUM(BlueprintType)
enum class FSBUniversalTargetingAxisInputType : uint8
{
	EMouseInput = 0			 UMETA(DisplayName = "Mouse axis input"),
	EGamepadInput = 1		 UMETA(DisplayName = "Gamepad axis input"),
};

/**
 * @brief Structure to convert mouse and gamepad axis inputs both ways. 
 */
USTRUCT(BlueprintType)
struct FSBUniversalTargetingAxisInput
{
	GENERATED_BODY()

	FSBUniversalTargetingAxisInput() = default;
	
	FSBUniversalTargetingAxisInput(FSBUniversalTargetingAxisInputType Type, float Value)
	{
		InputType = Type;
		AxisValue = Value;
	}

	UPROPERTY(BlueprintReadWrite)
	float AxisValue;
	
	UPROPERTY(BlueprintReadWrite)
	FSBUniversalTargetingAxisInputType InputType;
};

class AActor;
class APawn;
class UWidgetComponent;
class USBTargetingFilter;
class APlayerCameraManager;

DECLARE_EVENT_OneParam(USBTargetSystemComponent, FComponentOnTargetLockedOnOff, AActor*)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentOnTargetLockedOnOffDelegate, AActor*, Target);

DECLARE_EVENT_TwoParams(USBTargetSystemComponent, FComponentSetRotation, AActor*, FRotator)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentSetRotationDeleagate, AActor*, Actor, FRotator, Rotation);

UCLASS(ClassGroup = (SBTargetingSystem), Blueprintable, meta = (BlueprintSpawnableComponent))
class SOULBOUNDTARGETING_API USBTargetingComponent : public USBTargetingComponentBase
{
	GENERATED_BODY()

public:	
	USBTargetingComponent();
	
	/** Manually target best target. Does nothing if not currently targeting. */
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting")
	void StartTargeting();
		
	/** Manually untarget. Does nothing if not currently targeting. */
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting")
	void StopTargeting();
	
	/** Call start/stop targeting depending on current targeting state. */
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting")
	void ToggleTargeting();
	
	/**
	 * @brief Lock on specified target.
	 * Will lock off from current target, if currently targeting.
	 * @param Target Target to lock on. 
	 */
	virtual	void LockOnTarget(AActor* Target) override;
	
	/**
	 * @brief Try to find a new target with input direction.
	 * @param Input AxisInput and Type of current input.
	 */
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting")
	void LockOnNextTargetWithAxisInput(FSBUniversalTargetingAxisInput Input); 

	/* Returns true / false whether the system is targeting an target */
	virtual bool IsLockedOnTarget() const override;

	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting")
	AActor* GetBestTargetRightNow(ESBTargetSelectionType TargetSelection = ESBTargetSelectionType::ENearestTarget);
	
	/**
	* @brief Switch to new target which is in SwitchDirection from current target.
	* @param SwitchDirection Direction to look for new target
	*/
	void SwitchTargetByDirection(ESBTargetingDirection SwitchDirection);
	
	///
	/// Targeting Filters
	///
	
	/* Used to add Filter for potential targets at runtime*/
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting|Filters")
	void AddFilter(TSubclassOf<USBTargetingFilter> FilterClass);

	/* Used to remove a Filter for potential targets at runtime*/
	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting|Filters")
	bool RemoveFilter(TSubclassOf<USBTargetingFilter> FilterClass);

	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting|Filters")
	void AddObjectType(TEnumAsByte<EObjectTypeQuery> InTraceChannel);

	UFUNCTION(BlueprintCallable, Category = "Soulbound|Targeting|Filters")
	void RemoveObjectType(TEnumAsByte<EObjectTypeQuery> InTraceChannel);

	///
	/// Events
	///
	
	/** Called when a target is locked off, either if it is untarget condition or manual untarget. */
	FComponentOnTargetLockedOnOff OnTargetLockedOff;
	UPROPERTY(BlueprintAssignable)
	FComponentOnTargetLockedOnOffDelegate OnTargetLockedOffDelegate;

	/** Called when locked on a new target */
	FComponentOnTargetLockedOnOff OnTargetLockedOn;
	UPROPERTY(BlueprintAssignable)
	FComponentOnTargetLockedOnOffDelegate OnTargetLockedOnDelegate;

	/**
	* Setup the control rotation on Tick when a target is locked on.
	* If not implemented, will fallback to default implementation.
	* If this event is implemented, it lets you control the rotation of the character.
	*/
	FComponentSetRotation OnTargetSetRotation;
	
protected:
	virtual void BeginPlay() override;

	///
	/// Targeting Settings
	///
	
	/* Maximum distance for target detection */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Targeting Settings")
	float MaxTargetingDistance = 3500.f;

	/*MaxAngular distance from OwnerActor for target detection*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ATS)
	float MaxAngularDistanceDegree = 60.f;
	
	/* Target selection type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Targeting Settings")
	ESBTargetSelectionType TargetSelectionType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Soulbound|Targeting Settings")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToQuery;

	/* Filters to avoid an actor from being targeted */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Soulbound|Targeting Settings")
	TArray<USBTargetingFilter*> TargetFilters;
	
	/**
	 * Whether or not the character rotation should be controlled when Target is locked on.
	 * If true, it'll set the value of bUseControllerRotationYaw and bOrientationToMovement variables on Target locked on / off.
	 * Set it to true if you want the character to rotate around the locked on target to enable you to setup strafe animations.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Pawn")
	uint8 bRotatePawnToTarget : 1;
	
	/** Whether to accept pitch input when bAdjustPitchBasedOnDistanceToTarget is disabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings")
	uint8 bIgnoreLookInput : 1;

	/** If target sight to the target is interrupted by something, stop targeting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Targeting Settings")
	bool bStopTargetingIfOutOfSight = true;
	
	/** The amount of time to break line of sight when actor gets behind an Object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings")
	float BreakLineOfSightDelay = 2.0f;
	
	/** Timer rate to check lock off conditions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings")
	float CheckLockOffConditionsRate = 2.0f;

	/** Lower this value is, easier it will be to switch new target on right or left. Must be < 1.0f if controlling with gamepad stick.
	 * When using Sticky Feeling feature, it has no effect (see StickyRotationThreshold) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings")
	float StartRotatingThreshold = 0.85f;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings")
	TEnumAsByte<ECollisionChannel> TargetableCollisionChannel;

	///
	/// Sticky Feeling
	///
	
	/* Set it to true / false whether you want a sticky feeling when switching target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings|Sticky Feeling on Target Switch")
	uint8 bEnableStickyTarget : 1;
	
	/*  This value gets multiplied to the AxisValue to check against StickyRotationThreshold.
	 *  Only used when Sticky Target is enabled.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings|Sticky Feeling on Target Switch")
	float AxisMultiplier = 1.0f;

	/* Lower this value is, easier it will be to switch new target on right or left.
	 * This is similar to StartRotatingThreshold, but you should set this to a much higher value.
	 * Only used when Sticky Target is enabled.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targeting Settings|Sticky Feeling on Target Switch")
	float StickyRotationThreshold = 30.0f;
	
	///
	/// Camera Settings
	///
	
	/* Targeting type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Camera")
	ESBTargetingType TargetingType = ESBTargetingType::EMagneticLock;

	/* Strength of the magnetism when using TargetingType = EMagneticLock*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Camera")
	float LockMagnetism = 15.f;

	/* Limit on camera pitch to avoid gimbal lock */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Camera")
	float BottomPitchLimitDegree = -15.f;

	/* Limit on camera pitch to avoid gimbal lock */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soulbound|Camera")
	float UpperPitchLimitDegree = 75.f;

	///
	/// Camera Pitch Settings
	///
	
	/**
	* Setting this to true will tell the Target System to adjust the Pitch Offset (the Y axis) when locked on,
	* depending on the distance to the target actor.
	* It will ensure that the Camera will be moved up vertically the closer this Actor gets to its target.
	*
	* Formula:
	* (DistanceToTarget * PitchDistanceCoefficient + PitchDistanceOffset) * -1.0f
	* Then Clamped by PitchMin / PitchMax	
	*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Pitch Offset")
	bool bAdjustPitchBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Pitch Offset")
	float PitchDistanceCoefficient = -0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Pitch Offset")
	float PitchDistanceOffset = 60.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Pitch Offset")
	float PitchMin = -50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Pitch Offset")
	float PitchMax = -20.0f;	

	//
	// UI
	//

	/**
	 * Whether or not the Target LockOn Widget indicator should be drawn and attached automatically.
	 * When set to false, this allow you to manually draw the widget for further control on where you'd like it to appear.
	 * OnTargetLockedOn and OnTargetLockedOff events can be used for this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targering|Widget")
	uint8 bShouldDrawLockedOnWidget : 1;

	/**
	 * The Widget Class to use when locked on Target.
	 * If not defined, will fallback to a Text-rendered widget with a single O character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targering|Widget")
	TSubclassOf<UUserWidget> LockedOnWidgetClass;

	/** The Widget Draw Size for the Widget class to use when locked on Target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targering|Widget")
	float LockedOnWidgetDrawSize = 32.0f;

	/**
	 * @brief The Socket name to attach the LockedOn Widget.
	 * We should use this to configure the Bone or Socket name the widget should be attached to, and allow
	 * the widget to move with target character's animation (Ex: spine_03)
	 * Set it to None to attach the Widget Component to the Root Component instead of the Mesh.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targering|Widget")
	FName LockedOnWidgetParentSocket = FName("spine_03");

	/** The Relative Location to apply on Target LockedOn Widget when attached to a target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soulbound|Targering|Widget")
	FVector LockedOnWidgetRelativeLocation = FVector(0.0f, 0.0f, 0.0f);

private:
	UPROPERTY()
	TObjectPtr<APawn> ControlledPawn;
	
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;
	
	FTimerHandle CheckLockOffConditionsTimerHandle;
	FTimerHandle LineOfSightBreakTimerHandle;
	
	uint8 bTargetOnlyVisibleTargets : 1;
	uint8 bIsLockedOnTarget : 1;
	float ClosestTargetDistance = 0.0f;
	float StartRotatingStack = 0.0f;
	FName CurrentCameraEvent = NAME_None;

	/* True when line of sight to the target is broken. */
	uint8 bIsBreakingLineOfSight : 1;

	///
	/// Target Switching
	/// 
	
	void ResetIsSwitchingTarget();
	bool ShouldSwitchTargetActor(float AxisValue);
	
	uint8 bIsSwitchingTarget : 1;
	uint8 bDesireToSwitch : 1;
	FTimerHandle SwitchingTargetTimerHandle;
	
	///
	///  Targets search / trace
	///

	AActor* FindNearestTarget(const TArray<AActor*> &PotentialTargets) const;
	TArray<AActor*> FindTargetsInRange(TArray<AActor*> &PotentialTargets, float RangeMax, float RangeMin = 0.f) const;	
	TArray<AActor*> FindVisibleTargets(const TArray<AActor*> &PotentialTargets) const;

	void FindPotentialTargets(TArray<AActor*> &OutTargets) const;

	bool LineTrace(FHitResult& HitResult, AActor* OtherActor, TArray<AActor*> ActorsToIgnore = TArray<AActor*>()) const;
	bool LineTraceForActor(AActor* OtherActor, TArray<AActor*> ActorsToIgnore = TArray<AActor*>()) const;
	
	float GetDistanceToActorFromPawn(AActor* Actor) const;
	
	/* ~Begin UI/Widget methods */
	void CreateLockedOnWidgetOnTarget();
	void CreateLockedOnWidgetOnTargetPoint();
	/* ~End UI/Widget methods */
	
	/**	Lock On widget component on target */
	UPROPERTY()
	TObjectPtr<UWidgetComponent> TargetLockedOnWidgetComponent;
	
	/**
	 * @brief Check if line of sight to the target is currently broken.
	 * @return True if line of sight is broken.
	 */
	bool ShouldBreakLineOfSight() const;
	
	/**
	 * @brief Stop targeting if line of sight to the target is broken.
	 * Used in LineOfSightBreakTimerHandle.
	 */
	void BreakLineOfSight();
	
	///
	/// Targeting
	///
	
	AActor* GetBestTarget();
	void CheckLockOffConditions();
	void SetOwnerReferences();
	
	/**
	 * @brief Handle new pawn possession in controller.
	 * @param NewPawn New possessed pawn.
	 */
	UFUNCTION()
	void HandlePawnChanged(APawn* NewPawn);

	/**
	 * @brief Check if an actor can be a target.
	 * @param Target Target to check.
	 * @return True if actor is a valid target.
	 */
	UFUNCTION()
	bool IsValidTarget(AActor* Target);

	UFUNCTION()		
	void UpdateCurrentTargetPoint(USBTargetPointComponent * InCurrentTargetPoint);

	/**
	 * @brief Determine closest target point to the camera forward vector.
	 * @param Target Actor to search target points on.
	 * @return Best target point on target actor or nullptr if none is found.
	 */
	UFUNCTION()
	USBTargetPointComponent* GetBestTargetPointForTarget(AActor* Target) const;

	static ESBLockType GetLockTypeByTargetingType(ESBTargetingType TargetType);

	static bool IsActorImplementsInterface(AActor* Actor);

	float GetDistanceFromPawn(const AActor* OtherActor) const;
	bool IsActorInViewport(const AActor* Actor) const;

	float GetAngleUsingCameraRotation(const AActor* ActorToLook) const;
	float GetAngleUsingCharacterRotation(const AActor* ActorToLook) const;
	static FRotator FindLookAtRotation(const FVector Start, const FVector Target);

	UFUNCTION()
	bool TrySwitchPointOnCurrentTarget(ESBTargetingDirection direction);

	UFUNCTION()
	bool IsRightOfCurrentTarget(FVector locationToCheck);

	UFUNCTION()
	bool IsUpOfCurrentTarget(FVector locationToCheck);

	/*Get the world location of current target point*/
	UFUNCTION(BlueprintCallable, Category = ATS)
	FVector GetCurrentTargetPointLocation() const;

	USBTargetPointComponent* GetNearestTargetPoint(const TArray<USBTargetPointComponent*> &points);

	UFUNCTION()
	TArray<AActor*> GetAllTargetsByDirection(ESBTargetingDirection direction);

	bool IsInFrontOfOwner(AActor* target);

	UFUNCTION()
	class AActor* GetNearestTarget(TArray<AActor *> &actorFilter);
};

