// This is a task for gamescan made by Daniel Corredor.


#include "Actors/FlagZone/TaskFlagZone.h"

#include "Actors/Flag/TaskFlag.h"
#include "Character/TaskCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/GameModes/TaskGameplayGameMode.h"


/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskFlagZone::ATaskFlagZone()
{ 	
	PrimaryActorTick.bCanEverTick = false;

	FlagZoneCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FlagZoneCapsuleComponent"));
	SetRootComponent(FlagZoneCapsuleComponent);

	FlagZoneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagZoneMeshComponent"));
	FlagZoneMeshComponent->SetupAttachment(RootComponent);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlagZone::BeginPlay()
{
	Super::BeginPlay();
	check(FlagZoneCapsuleComponent);

	FlagZoneCapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	if(HasAuthority())
	{
		FlagZoneCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnFlagZoneCapsuleBeginOverlap);
	}
	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlagZone::OnFlagZoneCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	ATaskCharacter* TaskCharacter = Cast<ATaskCharacter>(OtherActor);
	if (TaskCharacter)
	{
		ATaskFlag* CharacterFlag = TaskCharacter->GetEquippedFlag();
		
		if(CharacterFlag && CharacterFlag->GetTeam() != ZoneTeam)
		{
			const ATaskGameplayGameMode* GameMode = GetWorld()->GetAuthGameMode<ATaskGameplayGameMode>();
			
			if (GameMode)
			{
				GameMode->PlayerCapturedFlag(ZoneTeam);
				TaskCharacter->UnEquippFlag();
				CharacterFlag->SetIsInZone(true);
				CharacterFlag->ResetFlag();
				CharacterFlag->Client_ResetFlag();
			}			
		}
	}
}
