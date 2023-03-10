// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TaskDefinitions.h"
#include "TaskFlagZone.generated.h"

class UCapsuleComponent;
class USphereComponent;
class UStaticMeshComponent;
/**
 *  Zone that handle the score and base for the flag an the team
 */
UCLASS()
class CTFTASK_API ATaskFlagZone : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* FlagZoneCapsuleComponent;	

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FlagZoneMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	ETeam ZoneTeam;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnFlagZoneCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
public:
	
	// Sets default values for this actor's properties
	ATaskFlagZone();


};
