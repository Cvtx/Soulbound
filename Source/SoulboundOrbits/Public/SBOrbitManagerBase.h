// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "SBOrbitsTypes.h"
#include "SBOrbitManagerBase.generated.h"

class USceneComponent;
class UBillboardComponent;
class UInstancedStaticMeshComponent;

UCLASS()
class SOULBOUNDORBITS_API ASBOrbitManagerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASBOrbitManagerBase();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AddOrbitMeshPlane(FSBOrbitActorData ActorData);

	UFUNCTION(BlueprintCallable)
	void AddOrbitPlaneInstance(FSBOrbitData OrbitData) const;

	UFUNCTION(BlueprintCallable)
	void UpdateActorPosition(FSBOrbitActorData ActorData, float NewTime) const;

	UFUNCTION(BlueprintCallable)
	static FVector CalculateOrbitPosition(FVector Center, FRotator Direction, float Angle, float WidthX, float LengthY);
	
	UFUNCTION(BlueprintCallable)
	static FVector GetRandomVectorInRangeFromStream(FVector MinimumVector, FVector MaximumVector, const FRandomStream &RandomStream);
	
	UFUNCTION(BlueprintCallable)
	static FRotator GetRandomRotatorInRangeFromStream(FRotator MinimumRotator, FRotator MaximumRotator, const FRandomStream &RandomStream);

	UFUNCTION(BlueprintCallable)
	static FSBOrbitData ConvertActorDataToOrbitData(FSBOrbitActorData ActorData);

	UFUNCTION(BlueprintCallable)
	static TArray<FSBOrbitData> GenerateRandomData(const FRandomStream& GenerationStream, int32 Amount, FSBOrbitRandomization Randomization, FRotator DefaultOrbitRotation, FRotator DefaultRotation, FVector DefaultScale, bool bRotateRelativeToOrbit);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsEnabled() const { return bEnabled; };

	UFUNCTION(BlueprintCallable, Category=Components)
	UInstancedStaticMeshComponent* GetOverflowOrbitInstancedMeshes() const { return OverflowOrbitInstancedMeshes; };
	
	/** Specify an actor which this system should follow. Useful for Moon Orbits */
	UPROPERTY()
	TObjectPtr<AActor> FollowActor;

	/** Whenever orbit enabled or not */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Orbit)
	uint8 bEnabled : 1;

	/** Enable the debug Orbit Rings during play */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Orbit)
	uint8 bShowDebugOrbits : 1;
	
	/** This option enables the visibility of the Debug Orbits while editing */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Orbit)
	uint8 bShowDebugOrbitsInEditor : 1;

protected:
	virtual void BeginPlay() override;
	
	//~Begin ASBOrbitManagerBase interface
	virtual void Initiate();
	virtual void Update(float NewTime);

	UFUNCTION(BlueprintCallable)
	virtual void DrawDebugOrbits();
	//~End ASBOrbitManagerBase interface
	
	void CalculateObjectTransform(FRotator OrbitRotation, FVector2D OrbitRadius, FVector OrbitOffset, float StartAngle,
	float MovementSpeed, FRotator DefaultRotation, FRotator RotationSpeed, bool bRelativeRotation, float NewTime,
	FVector& NewLocation, FRotator& NewRotation) const;
	
private:
	
	void UpdatePosition();

	/** */
	UPROPERTY(EditDefaultsOnly, Category=Components)
	TObjectPtr<UBillboardComponent> Billboard;
	
	/** */
	UPROPERTY(EditDefaultsOnly, Category=Components)
	TObjectPtr<USceneComponent> OrbitMeshes;

	/** */
	UPROPERTY(EditDefaultsOnly, Category=Components)
	TObjectPtr<UInstancedStaticMeshComponent> OverflowOrbitInstancedMeshes;
	
};
