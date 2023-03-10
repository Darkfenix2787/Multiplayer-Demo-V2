// This is a task for gamescan made by Daniel Corredor.


#include "Actors/Weapon/TaskProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskProjectile::ATaskProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);

	// Projectile Static Mesh Component
	ProjectileStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileStaticMesh"));
	ProjectileStaticMeshComponent->SetupAttachment(CollisionComp);

	if(HasAuthority())
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);		// set up a notification for when this component hits something blocking		
	}

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	Damage = 10.f;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if(OwnerCharacter)
		{
			AController* OwnerController = OwnerCharacter->Controller;
			if(OwnerController)
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			}
		}
	}
	
	// Only add impulse and destroy projectile if we hit a physics
	if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{	
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());		
	}

	Destroy();
}

