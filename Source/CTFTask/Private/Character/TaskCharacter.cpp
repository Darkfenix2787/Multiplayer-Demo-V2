// This is a task for gamescan made by Daniel Corredor.


#include "Character/TaskCharacter.h"

#include "Actors/Flag/TaskFlag.h"
#include "Actors/Weapon/TaskProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModes/TaskGameplayGameMode.h"
#include "GameFramework/PlayerController/TaskPlayerController.h"
#include "GameFramework/PlayerState/TaskPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskCharacter::ATaskCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));	

	// Create a gun mesh component to '1st person' view (when controlling this pawn)
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;	
	FP_Gun->SetupAttachment(Mesh1P);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	
	// Create a gun mesh component to '1st person' view (when not controlling this pawn)
	OP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OP_Gun"));
	OP_Gun->SetOwnerNoSee(true);		// only the others players will see this mesh
	OP_Gun->bCastDynamicShadow = false;
	OP_Gun->CastShadow = false;	
	OP_Gun->SetupAttachment(GetMesh());	

	GunSocketName = "Gun_Socket";
	Health = 100.f;
	MaxHealth = 100.f;

	bHasFlag = false;
	EquippedFlag = nullptr;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(FP_Gun);
	check(Mesh1P);
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	//Attach Gun mesh component to the Body Skeleton only if the character is not locally controlled
	if(!IsLocallyControlled())
	{
		check(OP_Gun);
		OP_Gun->AttachToComponent(GetMesh(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), GunSocketName);		
	}

	if(HasAuthority())		
	{
		OnTakeAnyDamage.AddDynamic(this,&ATaskCharacter::ReceiveDamage);		
	}

	InitializeHUD();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ThisClass::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ThisClass::FireWeapon);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ThisClass::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ThisClass::LookUpAtRate);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::Jump()
{
	bIsJumping = true;
	Super::Jump();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::StopJumping()
{
	bIsJumping = false;
	Super::StopJumping();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	Server_SyncAimRotation();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::Server_SyncAimRotation_Implementation()
{
	SyncAimRotation();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
bool ATaskCharacter::Server_SyncAimRotation_Validate()
{
	return true;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::SyncAimRotation()
{
	AimRotation = GetControlRotation();
	
	check(FirstPersonCameraComponent)
	FirstPersonCameraComponent->SetWorldRotation(AimRotation);	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::FireWeapon()
{
	Server_SpawnProjectile();

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		check(Mesh1P);
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}	
	
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::Server_SpawnProjectile_Implementation()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{			
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Owner = this;
			ActorSpawnParams.Instigator = Cast<APawn>(this);

			// spawn the projectile at the muzzle
			World->SpawnActor<ATaskProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);			
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
bool ATaskCharacter::Server_SpawnProjectile_Validate()
{
	return true;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::SetTeamColor(ETeam Team) const
{
	switch (Team)
	{
	case ETeam::ET_BlueTeam:
		Mesh1P->SetMaterial(0,BlueMaterial);
		GetMesh()->SetMaterial(0,BlueMaterial);		
		break;
	case ETeam::ET_RedTeam:
		Mesh1P->SetMaterial(0,RedMaterial);
		GetMesh()->SetMaterial(0,RedMaterial);		
		break;
	case ETeam::ET_MaxTeam:
	case ETeam::ET_NoTeam:
		break;
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaskCharacter, AimRotation);
	DOREPLIFETIME(ATaskCharacter, bIsJumping);
	DOREPLIFETIME(ATaskCharacter, Health);
	DOREPLIFETIME(ATaskCharacter, bWasEliminated);
	DOREPLIFETIME(ATaskCharacter, bHasFlag);	
	DOREPLIFETIME(ATaskCharacter, EquippedFlag);	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::EnableCharacterInput(const bool IsEnable)
{
	PlayerController = PlayerController == nullptr ? Cast<ATaskPlayerController>(Controller) : PlayerController;

	if(PlayerController)
	{
		if(IsEnable)
		{
			EnableInput(PlayerController);			
		}
		else
		{
			DisableInput(PlayerController);			
		}	
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::Multicast_SetMeshTeamColor_Implementation(ETeam Team)
{
	SetTeamColor(Team);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
ETeam ATaskCharacter::GetTeam()
{
	ATaskPlayerState* TaskPlayerState = GetPlayerState<ATaskPlayerState>();
	if(TaskPlayerState)
	{
		return TaskPlayerState->GetTeam();
	}

	return ETeam::ET_NoTeam;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::EquipFlag(ATaskFlag* FlagToEquip)
{
	bHasFlag = true;
	EquippedFlag = FlagToEquip;
	EquippedFlag->SetIsEquipped(true);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::UnEquippFlag()
{
	if(EquippedFlag != nullptr)
	{
		bHasFlag = false;
		EquippedFlag->SetIsEquipped(false);
		EquippedFlag = nullptr;
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();

	if (Health <= 0.f)
	{
		Eliminated();
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::OnRep_Health()
{
	UpdateHUDHealth();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
inline void ATaskCharacter::Eliminated()
{
	UnEquippFlag();	
	bWasEliminated = true;	
	OnTakeAnyDamage.RemoveDynamic(this,&ThisClass::ReceiveDamage);
	UpdateHUDDead(bWasEliminated);	

	if(GetWorld() && BodyDeathAnimation)
	{
		FTimerHandle RespawnTimerHandle;		
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle,this,&ThisClass::RespawnCharacter,BodyDeathAnimation->GetMaxCurrentTime() - 0.5f,false);

		GetMesh()->PlayAnimation(BodyDeathAnimation,false);
	}	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::OnRep_bWasEliminated()
{
	UpdateHUDDead(bWasEliminated);	
	
	if(BodyDeathAnimation && bWasEliminated)
	{
		GetMesh()->PlayAnimation(BodyDeathAnimation,false);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::RespawnCharacter()
{
	ATaskGameplayGameMode* GameModeRef = GetWorld()->GetAuthGameMode<ATaskGameplayGameMode>();
	if(GameModeRef)
	{
		GameModeRef->RequestRespawn(this,Controller);
	}

	Health = MaxHealth;	
	InitializeHUD();
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::InitializeHUD()
{
	UpdateHUDHealth();
	UpdateHUDFlag(false);
	UpdateHUDDead(false);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::UpdateHUDHealth()
{
	PlayerController = PlayerController == nullptr ? Cast<ATaskPlayerController>(Controller) : PlayerController;
	if(PlayerController)
	{
		PlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::UpdateHUDFlag(bool bIsActive)
{
	PlayerController = PlayerController == nullptr ? Cast<ATaskPlayerController>(Controller) : PlayerController;
	if(PlayerController)
	{
		PlayerController->SetHUDFlagVisibility(bIsActive);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskCharacter::UpdateHUDDead(const bool bIsDead)
{
	PlayerController = PlayerController == nullptr ? Cast<ATaskPlayerController>(Controller) : PlayerController;
	if(PlayerController)
	{	
		PlayerController->UpdateHUDDead(bIsDead);
	}
}
