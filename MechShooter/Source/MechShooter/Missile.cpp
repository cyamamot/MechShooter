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
	//CollisionComp->RegisterComponent();
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
	//ProjectileMovement->RegisterComponent();
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	// Die after 3 seconds by default
	InitialLifeSpan = 5.0f;
	Damage = 500.0f;
}

void AMissile::BeginPlay()
{
	Super::BeginPlay();
}


void AMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HitEffect != NULL)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, FTransform(FRotator(0.0f, 0.0f, 0.0f), Hit.Location, FVector(6.0f, 6.0f, 6.0f)), true);
		TArray<AActor*> Empty;
		AController* Instigator = ((APawn*)GetOwner())->GetController();
		if (Instigator) UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, 1.0f, Hit.Location, 50.0f, 200.0f, 1.0f, NULL, Empty, this, Instigator);
		else UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, 1.0f, Hit.Location, 50.0f, 200.0f, 1.0f, NULL, Empty, this, NULL);
	}
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		Destroy();
	}
	Destroy();
}

