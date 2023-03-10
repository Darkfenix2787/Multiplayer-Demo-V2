// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "TaskDefinitions.h"
#include "GameFramework/Character.h"
#include "TaskCharacter.generated.h"

class ATaskFlag;
class ATaskPlayerController;
class ATaskProjectile;
class USphereComponent;
class UCameraComponent;

UCLASS(config=Game)
class CTFTASK_API ATaskCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	//-------------------------------------------------- Protected Properties -------------------------------------------------------//
#pragma region Properties
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Gun mesh: 1st person view (seen only by others) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* OP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Flag to know if the character is jumping */
	UPROPERTY(Transient, Replicated)
	uint8 bIsJumping : 1;

	/** location where the projectile will be fired */
	UPROPERTY(Transient)
	FVector MuzzleSocketLocation;

	/** Player Max health */
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	/** Player current health */
	UPROPERTY(EditDefaultsOnly,ReplicatedUsing = OnRep_Health)
	float Health;

	/** Flag to know when the player was eliminated */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_bWasEliminated)
	uint8 bWasEliminated : 1;

	/** Flag to know when the character has a equipped flag */
	UPROPERTY(Replicated)
	uint8 bHasFlag : 1;

	/** Reference to the equipped flag */
	UPROPERTY(Replicated)
	ATaskFlag* EquippedFlag;

	/** Player Controller reference */
	UPROPERTY(Transient)
	ATaskPlayerController* PlayerController;

#pragma endregion

	//-------------------------------------------------- Protected Functions --------------------------------------------------------//
#pragma region Functions

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Jump function override
	virtual void Jump() override;

	// Stop Jump function override
	virtual void StopJumping() override;	

	/** Fires a projectile. */
	UFUNCTION()
	void FireWeapon();

	/** Fires a projectile. */
	UFUNCTION(Server, WithValidation, Reliable)
	void Server_SpawnProjectile();
		
	/** Handles moving forward/backward */
	void MoveForward(float Value);

	/** Handles stafing movement, left and right */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Server Function to sync the aim rotation */
	UFUNCTION(Server, WithValidation, Reliable)
	void Server_SyncAimRotation ();

	/** APawn interface */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;		

	/** For replicates properties */
	virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty>& OutLifetimeProps) const override;	

	/** Function to sync the aim rotation */
	void SyncAimRotation();

	/** Health OnRep Call */
	UFUNCTION()
	void OnRep_Health();

	/** bWasEliminated OnRep Call */
	UFUNCTION()
	void OnRep_bWasEliminated();
	
	/** Function to respawn the character when dies*/
	void RespawnCharacter();

	/** Function to update the health in the HUD player */
	void UpdateHUDHealth();

	/** Function to update the Dead Hud */
	void UpdateHUDDead(const bool bIsDead);

#pragma endregion

public:
	
	//-------------------------------------------------- Public Properties -------------------------------------------------------//

#pragma region Properties

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Aim Rotation Offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category=Camera)
	FRotator AimRotation;

	/** Socket Name where the Gun will be attached */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FName GunSocketName;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<ATaskProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire (arms; seen only by self)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** AnimMontage to play each time the character Dies (Body; seen only by others) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimationAsset* BodyDeathAnimation;
	
	/** Material for the red team character mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInstance* RedMaterial;

	/** Material for the blue team character mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInstance* BlueMaterial;

#pragma endregion 

	//-------------------------------------------------- Public Functions --------------------------------------------------------//

#pragma region Functions
	
	// Sets default values for this character's properties
	ATaskCharacter();

	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** bIsJumping Getter */
	UFUNCTION(BlueprintCallable)
	bool IsJumping () const { return bIsJumping; }

	/** Sets the Character Color Based on its team */
	void SetTeamColor(ETeam Team) const;	

	/** Enable/disable the character movement (input) */
	void EnableCharacterInput(const bool IsEnable);

	/** Multicast Function to set the character Team Color */
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_SetMeshTeamColor(ETeam Team);

	/** Health Getter */
	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	/** MaxHealth Getter */
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return MaxHealth; }

	/** Gets the character Team */
	ETeam GetTeam();

	/** Equips the flag */
	UFUNCTION()
	void EquipFlag(ATaskFlag* FlagToEquip);

	/** UnEquip the Flag */
	void UnEquippFlag();

	/** Function to update the Flag Image in the HUD player */
	void UpdateHUDFlag(bool bIsActive);

	/** bHasFlag Getter */
	bool HasFlagEquipped() const { return bHasFlag; }

	/** FlagEquipped Getter */
	ATaskFlag* GetEquippedFlag() const { return EquippedFlag; }

	/** Function to receive damage */
	UFUNCTION()
	void ReceiveDamage (AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	/** Function to handle the character's dead */
	void Eliminated();

	/** bWasEliminated Getter */
	UFUNCTION(BlueprintCallable)
	bool WasEliminated () const { return bWasEliminated; }
	
	/** Function To Initialize the players Hud */
	void InitializeHUD();

#pragma endregion

};




