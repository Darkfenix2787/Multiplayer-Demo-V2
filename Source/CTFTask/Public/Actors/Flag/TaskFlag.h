// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "TaskDefinitions.h"
#include "GameFramework/Actor.h"
#include "TaskFlag.generated.h"

class ATaskCharacter;
class UCapsuleComponent;

/** Class that handle the Flag Behaviour */
UCLASS()
class CTFTASK_API ATaskFlag : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, Category = "Flag Properties")
	UStaticMeshComponent* FlagMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Flag Properties")
	UCapsuleComponent* AreaCapsuleComponent;

	UPROPERTY(Replicated)
	ATaskCharacter* OwnerCharacter;

	/** Socket Name where the Flag will be attached */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FName FlagSocketName;

	UPROPERTY(ReplicatedUsing = OnRep_bIsEquipped)
	uint8 bIsEquipped : 1;

	UPROPERTY(Replicated)
	uint8 bIsInZone : 1;

	UPROPERTY(EditDefaultsOnly)
	ETeam Team;

	UPROPERTY(Replicated)
	FTransform InitialTransform;

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Area Sphere Begin Overlap Function */
	UFUNCTION()
	virtual void OnAreaSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult );

	/** Replication properties conditions */
	virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:	
	/** Sets default values for this actor's properties */
	ATaskFlag();

	/** bIsEquipped Setter */
	void SetIsEquipped(bool bNewValue );

	/** Function to SetUp the flag behaviour when is Equipped/UnEquipped */
	void SetUpEquipped(const bool bNewState);

	UFUNCTION()
	void OnRep_bIsEquipped();

	/** Team Getter */
	ETeam GetTeam() const { return Team; }

	/** Function to reset the flag position */
	void ResetFlag();

	/** Client Function to reset the flag position */
	void Client_ResetFlag();

	/** bIsInZone Setter */
	void SetIsInZone(bool bNewState) { bIsInZone = bNewState; }	
};
