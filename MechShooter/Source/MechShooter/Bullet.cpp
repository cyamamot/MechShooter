// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ABullet::ABullet()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));
	//CollisionComp->RegisterComponent();
	CollisionComp->InitBoxExtent(FVector(3.0f, 1.5f, 1.5f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::OnHit);		// set up a notification for when this component hits something blocking
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
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;


	// Die after 3 seconds by default
	InitialLifeSpan = 5.0f;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor->ActorHasTag(FName(TEXT("Bullet"))))) return;
	if (HitEffect != NULL)
	{
		FVector Up(0.0f, 0.0f, 1.0f);
		FVector Axis = FVector::CrossProduct(Up, Hit.ImpactNormal);
		Axis.Normalize();
		float Angle = acosf(FVector::DotProduct(Up, Hit.ImpactNormal));
		FQuat Quat(Axis, Angle);
		FRotator Rotator = Quat.Rotator();
		UWorld* World = GetWorld();
		if (World == NULL) return;
		UGameplayStatics::SpawnEmitterAtLocation(World, HitEffect, FTransform(Rotator, Hit.Location, FVector(0.5f, 0.5f, 0.5f)), true);
	}
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		Destroy();
	}
	Destroy();
}


