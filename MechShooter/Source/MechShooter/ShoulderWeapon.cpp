// Fill out your copyright notice in the Description page of Project Settings.

#include "ShoulderWeapon.h"

// Sets default values
AShoulderWeapon::AShoulderWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;

	RootComponent = FP_Gun;

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(2.15f, -17.0f, 14.0f));
	FP_MuzzleLocation->AttachToComponent(FP_Gun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("MuzzleSocket"));

	MovingToPosition = false;
	ReadyToFire = false;
}

// Called when the game starts or when spawned
void AShoulderWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShoulderWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShoulderWeapon::Fire()
{
	// try and fire a projectile
	if (MovingToPosition == false)
	{
		MoveUp();
		return;
	}
	if ((ProjectileClass != NULL) && (MovingToPosition == true) && (ReadyToFire == true))
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

}

void AShoulderWeapon::MoveUp()
{
	if (MovingToPosition == false)
	{
		MovingToPosition = true;
	}
}

void AShoulderWeapon::MoveDown()
{
	if (MovingToPosition == true)
	{
		MovingToPosition = false;
	}
}

void AShoulderWeapon::Activate()
{
	if (MovingToPosition == true)
	{
		MoveDown();
	}
	else if (MovingToPosition == false)
	{
		MoveUp();
	}
}


