// This is a task for gamescan made by Daniel Corredor.


#include "Actors/Flag/TaskFlag.h"

#include "Character/TaskCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskFlag::ATaskFlag()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	FlagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMeshComponent"));
	SetRootComponent(FlagMeshComponent);	

	AreaCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AreaCapsuleComponent"));
	AreaCapsuleComponent->SetupAttachment(FlagMeshComponent);

	OwnerCharacter = nullptr;
	bIsEquipped = false;
	bIsInZone = true;
	FlagSocketName = "Flag_Socket";

}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::BeginPlay()
{
	Super::BeginPlay();

	check(AreaCapsuleComponent);
	check(FlagMeshComponent);

	FlagMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Block);
	FlagMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	AreaCapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	

	if(HasAuthority())
	{
		AreaCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ATaskFlag::OnAreaSphereBeginOverlap);		
	}

	InitialTransform = GetActorTransform();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaskFlag, OwnerCharacter);
	DOREPLIFETIME(ATaskFlag, bIsEquipped);
	DOREPLIFETIME(ATaskFlag, bIsInZone);
	DOREPLIFETIME(ATaskFlag, InitialTransform);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::OnAreaSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OwnerCharacter = Cast<ATaskCharacter>(OtherActor);

	if(OwnerCharacter)
	{	
		if(OwnerCharacter->GetTeam() != Team && bIsInZone)
		{
			OwnerCharacter->EquipFlag(this);
			bIsInZone = false;
			return;
		}

		if(OwnerCharacter->GetTeam() == Team && !bIsInZone)
		{
			bIsInZone = true;
			ResetFlag();
			Client_ResetFlag();
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::SetIsEquipped(bool bNewValue)
{
	bIsEquipped = bNewValue;
	SetUpEquipped(bIsEquipped);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::OnRep_bIsEquipped()
{
	SetUpEquipped(bIsEquipped);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::SetUpEquipped(const bool bNewState)
{
	check(AreaCapsuleComponent);
	check(FlagMeshComponent);
	if(!OwnerCharacter)
	{
		return;
	}
	
	AreaCapsuleComponent->SetCollisionEnabled(bNewState ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly );	

	FlagMeshComponent->SetSimulatePhysics(!bNewState);	
	FlagMeshComponent->SetCollisionEnabled(bNewState ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

	
	SetOwner(bNewState ? OwnerCharacter : nullptr); 

	if(bNewState)
	{
		USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
		if(OwnerMesh)
		{
			FlagMeshComponent->AttachToComponent(OwnerMesh,FAttachmentTransformRules::SnapToTargetIncludingScale,FlagSocketName);
		}					
	}
	else
	{
		FlagMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	OwnerCharacter->UpdateHUDFlag(bNewState);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::ResetFlag()
{
	SetActorTransform(InitialTransform);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskFlag::Client_ResetFlag()
{
	ResetFlag();
}
