// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AMissile::AMissile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(8.0f, 4.0f, 4.0f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMissile::OnHit);		// set up a notification for when this component hits something blocking
																			// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->SetNotifyRigidBodyCollision(true);

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	TrailingLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TrailingEffectLocation"));
	TrailingLocation->SetupAttachment(CollisionComp);
	TrailingLocation->AttachToComponent(CollisionComp, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

	// Die after 3 seconds by default
	InitialLifeSpan = 5.0f;
}

void AMissile::BeginPlay()
{
	Super::BeginPlay();
	if (TrailingEffect != NULL)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailingEffect, FTransform(FRotator(0.0f, 0.0f, 0.0f), TrailingLocation->GetComponentLocation(), FVector(1.0f, 1.0f, 1.0f)), true);
	}
}


void AMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HitEffect != NULL)
	{
		//FX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FX"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, FTransform(FRotator(0.0f, 0.0f, 0.0f), Hit.Location, FVector(3.0f, 3.0f, 3.0f)), true);
	}
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		Destroy();
	}
	Destroy();
}

