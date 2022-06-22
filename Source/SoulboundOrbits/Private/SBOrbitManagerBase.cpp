// Copyright Cvtx. All Rights Reserved.


#include "SBOrbitManagerBase.h"
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/KismetMathLibrary.h"


ASBOrbitManagerBase::ASBOrbitManagerBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bEnabled = true;
	bShowDebugOrbits = false;
	bShowDebugOrbitsInEditor = true;
	
	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	if (Billboard)
	{
		SetRootComponent(Billboard.Get());
		Billboard->bIsScreenSizeScaled = true;
		Billboard->ScreenSize = 0.005f;
	}
	
	OrbitMeshes = CreateDefaultSubobject<USceneComponent>("OrbitMeshes");
	if (OrbitMeshes)
	{
		OrbitMeshes->SetupAttachment(RootComponent);
	}
	
	OverflowOrbitInstancedMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>("OverflowOrbitInstancedMeshes");
	if (OverflowOrbitInstancedMeshes)
	{
		OverflowOrbitInstancedMeshes->SetupAttachment(RootComponent);
		OverflowOrbitInstancedMeshes->SetCollisionProfileName("NoCollision");
		OverflowOrbitInstancedMeshes->SetGenerateOverlapEvents(false);
	}

	// Paste this in derived classes constructor
	Initiate(); 

	// Make this call in derived Blueprints constructor
	if (bShowDebugOrbitsInEditor)
	{
		DrawDebugOrbits();
	}
}

void ASBOrbitManagerBase::BeginPlay()
{
	Super::BeginPlay();

	if (bShowDebugOrbits == false)
	{
		OverflowOrbitInstancedMeshes->ClearInstances();
	}
	
	Initiate();

	if (bShowDebugOrbits)
	{
		DrawDebugOrbits();
	}
	
	//bEnableDebugOrbits ? DrawDebugOrbits() : OverflowOrbitInstancedMeshes->ClearInstances(); 
}

//~Begin ASBOrbitManagerBase interface
void ASBOrbitManagerBase::Initiate() { }
void ASBOrbitManagerBase::Update(float NewTime) { }
void ASBOrbitManagerBase::DrawDebugOrbits() { }
//~End ASBOrbitManagerBase interface

void ASBOrbitManagerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bEnabled)
	{
		UpdatePosition();
		Update(GetGameTimeSinceCreation());
	}
}

FVector ASBOrbitManagerBase::CalculateOrbitPosition(FVector Center, FRotator Direction, float Angle, float WidthX, float LengthY)
{
	const FVector Vector = FVector(UKismetMathLibrary::DegCos(Angle) * WidthX, UKismetMathLibrary::DegSin(Angle) * LengthY, 0.f);
	const FVector RotatedVector = Direction.RotateVector(Vector);
	return RotatedVector + Center;
}

void ASBOrbitManagerBase::UpdateActorPosition(FSBOrbitActorData ActorData, float NewTime) const
{
	if (IsValid(ActorData.Actor) == false) return;

	FVector NewLocation;
	FRotator NewRotation;
	CalculateObjectTransform(ActorData.OrbitRotation, ActorData.OrbitRadius, ActorData.OrbitOffset, ActorData.StartAngle,
		ActorData.MovementSpeed, ActorData.DefaultRotation, ActorData.RotationSpeed, ActorData.bRotateRelativeToOrbit, NewTime,
		NewLocation, NewRotation);
	ActorData.Actor->SetActorLocationAndRotation(NewLocation, NewRotation);
}

void ASBOrbitManagerBase::CalculateObjectTransform(FRotator OrbitRotation, FVector2D OrbitRadius, FVector OrbitOffset,
	float StartAngle, float MovementSpeed, FRotator DefaultRotation, FRotator RotationSpeed, bool bRelativeRotation, float NewTime,
	FVector& NewLocation, FRotator& NewRotation) const
{
	const float Angle = MovementSpeed * NewTime + StartAngle;
	const FVector OrbitPosition = CalculateOrbitPosition(GetActorLocation(), OrbitRotation, Angle, OrbitRadius.X, OrbitRadius.Y);
	NewLocation = OrbitPosition + FTransform(OrbitRotation).TransformPosition(OrbitOffset);

	const FRotator Rotation = (bRelativeRotation) ? FRotator(0, Angle, 0) : FRotator();
	const FRotator TransformedRotation = UKismetMathLibrary::TransformRotation(FTransform(Rotation), UKismetMathLibrary::TransformRotation(FTransform(RotationSpeed * NewTime), DefaultRotation)); 
	NewRotation = UKismetMathLibrary::TransformRotation(FTransform(OrbitRotation), TransformedRotation);
}

void ASBOrbitManagerBase::AddOrbitMeshPlane(FSBOrbitActorData ActorData)
{
	const FVector DefaultVector = FVector(4.f);
	const FVector Scale = FVector(DefaultVector.X * ActorData.OrbitRadius.X / 100, DefaultVector.Y * ActorData.OrbitRadius.Y / 100 , 1.f);
	const FTransform Transform = FTransform(ActorData.OrbitRotation, ActorData.OrbitOffset, Scale);

	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, Transform, true));
	StaticMeshComponent->AttachToComponent(OrbitMeshes, FAttachmentTransformRules::KeepRelativeTransform);
}

void ASBOrbitManagerBase::AddOrbitPlaneInstance(FSBOrbitData OrbitData) const
{
	const FVector Location = FTransform(OrbitData.Rotation).TransformPosition(OrbitData.Offset);
	const FVector DefaultVector = FVector(4.f);
	const FVector Scale = FVector(DefaultVector.X * OrbitData.Radius.X / 100, DefaultVector.Y * OrbitData.Radius.Y / 100 , 1.f); 
	const FTransform InstanceTransform = FTransform(OrbitData.Rotation, Location, Scale);
	OverflowOrbitInstancedMeshes->AddInstance(InstanceTransform);
}

void ASBOrbitManagerBase::UpdatePosition()
{
	if (IsValid(FollowActor))
	{
		SetActorLocation(FollowActor->GetActorLocation());
	}
}

FVector ASBOrbitManagerBase::GetRandomVectorInRangeFromStream(FVector MinimumVector, FVector MaximumVector,
	const FRandomStream& RandomStream)
{
	const float X = UKismetMathLibrary::RandomFloatInRangeFromStream(MinimumVector.X, MaximumVector.X, RandomStream);
	const float Y = UKismetMathLibrary::RandomFloatInRangeFromStream(MinimumVector.Y, MaximumVector.Y, RandomStream);
	const float Z = UKismetMathLibrary::RandomFloatInRangeFromStream(MinimumVector.Z, MaximumVector.Z, RandomStream);
	return FVector(X, Y, Z);
}

FRotator ASBOrbitManagerBase::GetRandomRotatorInRangeFromStream(FRotator MinimumRotator, FRotator MaximumRotator,
	const FRandomStream& RandomStream)
{
	const float Roll = UKismetMathLibrary::RandomFloatInRangeFromStream(MinimumRotator.Roll, MaximumRotator.Roll, RandomStream);
	const float Pitch = UKismetMathLibrary::RandomFloatInRangeFromStream(MinimumRotator.Pitch, MaximumRotator.Pitch, RandomStream);
	const float Yaw = UKismetMathLibrary::RandomFloatInRangeFromStream(MinimumRotator.Yaw, MaximumRotator.Yaw, RandomStream);
	return FRotator(Pitch, Yaw, Roll);
}

FSBOrbitData ASBOrbitManagerBase::ConvertActorDataToOrbitData(FSBOrbitActorData ActorData)
{
	FSBOrbitData OrbitData;
	OrbitData.Rotation = ActorData.OrbitRotation;
	OrbitData.Radius = ActorData.OrbitRadius;
	OrbitData.Offset = ActorData.OrbitOffset;
	OrbitData.StartAngle = ActorData.StartAngle;
	OrbitData.MovementSpeed = ActorData.MovementSpeed;
	OrbitData.DefaultRotation = ActorData.DefaultRotation;
	OrbitData.RotationSpeed = ActorData.RotationSpeed;
	OrbitData.bRotateRelativeToOrbit = ActorData.bRotateRelativeToOrbit;
	return OrbitData;
}

TArray<FSBOrbitData> ASBOrbitManagerBase::GenerateRandomData(const FRandomStream& GenerationStream, int32 Amount,
	FSBOrbitRandomization Randomization, FRotator DefaultOrbitRotation, FRotator DefaultRotation, FVector DefaultScale,
	bool bRotateRelativeToOrbit)
{
	TArray<FSBOrbitData> OrbitsData;
	OrbitsData.SetNum(Amount);
	
	for (int i = 0; i < Amount; ++i)
	{
		FSBOrbitData OrbitData;
		OrbitData.Rotation = UKismetMathLibrary::TransformRotation(FTransform(DefaultOrbitRotation), GetRandomRotatorInRangeFromStream(Randomization.MinimumOrbitRotation, Randomization.MaximumOrbitRotation, GenerationStream));
		OrbitData.Radius = Randomization.bUniformRadius ? FVector2D(UKismetMathLibrary::RandomFloatInRangeFromStream(Randomization.MinimumOrbitRadius.X, Randomization.MaximumOrbitRadius.X, GenerationStream)) : FVector2D(GetRandomVectorInRangeFromStream(FVector(Randomization.MinimumOrbitRadius, 0),FVector(Randomization.MaximumOrbitRadius, 0), GenerationStream));
		OrbitData.Offset = GetRandomVectorInRangeFromStream(Randomization.MinimumOrbitOffset, Randomization.MaximumOrbitOffset, GenerationStream);
		OrbitData.StartAngle = UKismetMathLibrary::RandomFloatInRangeFromStream(Randomization.MinimumStartAngle, Randomization.MaximumStartAngle, GenerationStream);
		OrbitData.MovementSpeed = UKismetMathLibrary::RandomFloatInRangeFromStream(Randomization.MinimumMovementSpeed, Randomization.MaximumMovementSpeed, GenerationStream);
		OrbitData.DefaultRotation = DefaultRotation;
		OrbitData.RotationSpeed = GetRandomRotatorInRangeFromStream(Randomization.MinimumRotationSpeed, Randomization.MaximumRotationSpeed, GenerationStream);
		OrbitData.bRotateRelativeToOrbit = bRotateRelativeToOrbit;
		OrbitData.DefaultScale = DefaultScale + GetRandomVectorInRangeFromStream(Randomization.MinimumScale, Randomization.MaximumScale,GenerationStream);
		OrbitsData.Add(OrbitData);
	}
	
	return OrbitsData;
}
