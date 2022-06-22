// Copyright Cvtx. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBOrbitActorConnector.generated.h"

UCLASS()
class SOULBOUNDORBITS_API ASBOrbitActorConnector : public AActor
{
	GENERATED_BODY()
	
public:	
	ASBOrbitActorConnector();
	
	virtual void Tick(float DeltaTime) override;

	/** */
	UFUNCTION(BlueprintCallable, Category=Attach)
	void SetAttachActor(AActor* ActorToAttach);

	/** */
	UFUNCTION(BlueprintCallable, Category=Attach)
	void SetFollowActor(AActor* ActorToFollow);
	
private:
	/** */
	void UpdatePositions();

	/** */
	UPROPERTY(EditAnywhere, Category="Attach Settings")
	TObjectPtr<AActor> AttachActor;

	/** */
	UPROPERTY(EditAnywhere, Category="Attach Settings")
	TObjectPtr<AActor> FollowActor;

};
