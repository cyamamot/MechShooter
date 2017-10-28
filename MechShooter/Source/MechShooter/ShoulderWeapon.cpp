// Fill out your copyright notice in the Description page of Project Settings.

#include "ShoulderWeapon.h"

// Sets default values
AShoulderWeapon::AShoulderWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShoulderMesh"));
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = true;

	RootComponent = Mesh;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	Muzzle->SetupAttachment(Mesh);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(2.15f, -17.0f, 14.0f));
	Muzzle->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("MuzzleSocket"));

	GettingReady = false;
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
	if ((ProjectileClass != NULL) && (GettingReady == true) && (ReadyToFire == true))
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = Muzzle->GetComponentRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = Muzzle->GetComponentLocation();

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
	if (GettingReady == false)
	{
		GettingReady = true;
	}
}

void AShoulderWeapon::MoveDown()
{
	if (GettingReady == true)
	{
		GettingReady = false;
	}
}

void AShoulderWeapon::Activate()
{
	if (GettingReady == false)
	{
		MoveUp();
		return;
	}
	else if (GettingReady == true)
	{
		MoveDown();
		return;
	}
}


