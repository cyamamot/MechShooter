// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ARifle::ARifle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	RootComponent = FP_Gun;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));
	CollisionComp->SetupAttachment(RootComponent);

	WeaponRange = 5000.0f;
	GunType = 0;
}

// Called when the game starts or when spawned
void ARifle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
/*void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}*/

void ARifle::Fire()
{
	// Now send a trace from the end of our gun to see if we should hit anything
	APlayerController* PlayerController = Cast<APlayerController>(((APawn*)GetOwner())->GetController());

	// Calculate the direction of fire and the start location for trace
	FVector CamLoc;
	FRotator CamRot;
	PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector ShootDir = CamRot.Vector();

	FVector StartTrace = FVector::ZeroVector;
	if (PlayerController)
	{
		FRotator UnusedRot;
		PlayerController->GetPlayerViewPoint(StartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
	}

	// Calculate endpoint of trace
	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;

	// Check for impact
	const FHitResult CameraImpact = WeaponTrace(StartTrace, EndTrace);

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			WeaponDamage = FMath::RandRange(100.0f, 150.0f);
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SocketLocation = FP_Gun->GetSocketLocation("Muzzle");
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, CameraImpact.ImpactPoint);

			if ((CameraImpact.GetActor() != NULL) && (CameraImpact.GetComponent() != NULL))
			{
				FTransform Transform(LookAtRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				Projectile->SetOwner(GetOwner());
				Projectile->Damage = WeaponDamage;
				Projectile->SetLifeSpan(3.0f);
			}
			else
			{
				FRotator ControlRotation = ((APawn*)GetOwner())->GetControlRotation();
				FTransform Transform(ControlRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				Projectile->SetOwner(GetOwner());
				Projectile->Damage = WeaponDamage;
				Projectile->SetLifeSpan(3.0f);
			}
		}
	}
}


