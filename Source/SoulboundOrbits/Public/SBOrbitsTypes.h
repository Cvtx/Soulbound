// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBOrbitsTypes.generated.h"

USTRUCT(BlueprintType)
struct SOULBOUNDORBITS_API FSBOrbitActorData
{
	GENERATED_BODY()

	FSBOrbitActorData()
		: OrbitRadius(1000.f, 1000.f)
		, MovementSpeed(1.f)
		, bRotateRelativeToOrbit(true)
	{
	};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> Actor;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator OrbitRotation;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D OrbitRadius;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector OrbitOffset;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StartAngle;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator DefaultRotation;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator RotationSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bRotateRelativeToOrbit : 1;
};

USTRUCT(BlueprintType)
struct SOULBOUNDORBITS_API FSBOrbitData
{
	GENERATED_BODY()

	FSBOrbitData()
		: Radius(1000.f, 1000.f)
		, MovementSpeed(1.f)
		, bRotateRelativeToOrbit(true)
		, DefaultScale(FVector::OneVector)
	{
	};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D Radius;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Offset;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StartAngle;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator DefaultRotation;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator RotationSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bRotateRelativeToOrbit : 1;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector DefaultScale;
};

USTRUCT(BlueprintType)
struct SOULBOUNDORBITS_API FSBOrbitRandomization
{
	GENERATED_BODY()

	FSBOrbitRandomization()
		: MinimumOrbitRotation(-180.f)
		, MaximumOrbitRotation(180.f)
		, MinimumOrbitRadius(1000.f)
		, MaximumOrbitRadius(2000.f)
		, MaximumStartAngle(360.f)
		, MinimumMovementSpeed(-180.f)
		, MaximumMovementSpeed(180.f)
	{
	};

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator MinimumOrbitRotation;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator MaximumOrbitRotation;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bUniformRadius : 1;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D MinimumOrbitRadius;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D MaximumOrbitRadius;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector MinimumOrbitOffset;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector MaximumOrbitOffset;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinimumStartAngle;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaximumStartAngle;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinimumMovementSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaximumMovementSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator MinimumRotationSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator MaximumRotationSpeed;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector MinimumScale;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector MaximumScale;
};
